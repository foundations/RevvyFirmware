"""
Generate a c/h file pair that contains runnable calls and port connections based on project.json
"""
import argparse
import json
import os
import sys
from json import JSONDecodeError

import pystache

from tools.generator_common import type_default_values, component_folder_pattern, component_file_pattern, \
    process_runnable, process_port, process_runnables, process_ports, load_component_config, load_project_config

port_compatibility = {
    "WriteData":              {
        "ReadValue": {"multiple_consumers": True}
    },
    "ProvideConstantByValue": {
        "ReadValue": {"multiple_consumers": True}
    },
    "Event":                  {
        "Runnable": {"multiple_consumers": True}
    },
    "ServerCall":             {
        "Runnable": {"multiple_consumers": False}
    }
}

databuffer_templates = {
    "WriteData": "static {{data_type}} {{component_name}}_{{port_name}}_databuffer{{ #init_value }} = {{{ . }}}{{ /init_value }};"
}

port_template_write_data = """void {{component_name}}_Write_{{port_name}}({{data_type}} value)
{
    {{component_name}}_{{port_name}}_databuffer = value;
}
"""

port_template_read_value = """{{data_type}} {{consumer_component_name}}_Read_{{consumer_port_name}}(void)
{
    return {{provider_component_name}}_{{provider_port_name}}_databuffer;
}
"""

port_template_read_constant = """{{data_type}} {{consumer_component_name}}_Read_{{consumer_port_name}}(void)
{
    return {{provider_component_name}}_Constant_{{provider_port_name}}();
}
"""

arg_list_template = "{{ #{0} }}{{ type }} {{name}}{{^last}}, {{/last}}{{ /{0} }}{{ ^{0} }}void{{ /{0} }}" \
    .replace('{{', '{{{{') \
    .replace('}}', '}}}}')

call_arg_list_template = "{{ #{0} }}{{name}}{{^last}}, {{/last}}{{ /{0} }}".replace('{{', '{{{{').replace('}}', '}}}}')

port_template_server_call = """{{ return_type }} {{component_name}}_Call_{{port_name}}(""" + arg_list_template.format(
    'arguments') + """)
{
    {{ #runnables }}
    {{ ^void }}return {{ /void }}{{{ . }}}
    {{ /runnables }}
}
"""

runnable_connection_templates = {
    "Event":      port_template_server_call,
    "ServerCall": port_template_server_call
}

runnable_call_templates = {
    "Runnable": "{{consumer_component_name}}_Run_{{consumer_port_name}}("
                + call_arg_list_template.format('arguments') + ");"
}

provider_port_templates = {
    "WriteData":              port_template_write_data,
    "ProvideConstantByValue": None
}

consumer_port_templates = {
    "ReadValue": {
        "WriteData":              port_template_read_value,
        "ProvideConstantByValue": port_template_read_constant
    }
}

header_template = """#ifndef GENERATED_RUNTIME_H_
#define GENERATED_RUNTIME_H_

{{#includes}}
#include "{{.}}.h"
{{/includes}}

{{#runnable_groups}}
void RunnableGroup_{{ group_name }}(void);
{{/runnable_groups}}

#endif /* GENERATED_RUNTIME_H */
"""

source_template = """#include "{{ output_filename }}.h"

{{#data_buffers}}
{{{.}}}
{{/data_buffers}}
{{#runnable_groups}}

void RunnableGroup_{{ group_name }}(void)
{
    {{ #runnables }}
    {{ component }}_Run_{{ runnable }}();
    {{ /runnables }}
}
{{/runnable_groups}}

{{#port_functions}}
{{{.}}}
{{/port_functions}}
"""


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', help='Name of project config json file', default="project.json")
    parser.add_argument('--validate-only', help='Skip generating code, only check the config json', action="store_true")
    parser.add_argument('--output', help='File name of generated files', default="rrrc/generated_runtime")

    args = parser.parse_args()

    project_config = load_project_config(args.config)

    runtime_config = project_config['runtime']
    component_data = {}
    type_data = {}
    resolved_types = {}

    def resolve_type(type_name, past=None):
        if type_name not in type_data:
            raise Exception('Incomplete type: {}'.format(type_name))

        if type_name in resolved_types:  # cache
            return resolved_types[type_name]

        # record visited types to detect circular definition
        if past is None:
            past = []
        elif type_name in past:
            raise Exception('Circular type definition for {}'.format(type_name))

        if 'aliases' in type_data[type_name]:
            past.append(type_name)
            resolved = resolve_type(type_data[type_name]['aliases'], past)

            resolved_types[type_name] = resolved

            return resolved
        elif 'defined_in' in type_data[type_name]:
            resolved_types[type_name] = type_name
            return type_name

    def add_data_type(type_name, info):
        if type_name in type_data:
            # type already exists, check if they are the same
            resolved_known = resolve_type(type_name)

            if 'aliases' in info:
                resolved_new = resolve_type(info['aliases'])
                if resolved_known != resolved_new:
                    raise Exception('Type {} is already defined'.format(type_name))
            elif 'defined_in' in info:
                if info['defined_in'] != type_data[resolved_known]['defined_in']:
                    raise Exception('Type {} can\'t override a type from a different source'.format(type_name))
            else:
                raise Exception('Invalid type definition {}'.format(type_name))

        else:
            type_data[type_name] = info

    valid = True

    for type_name in project_config['types']:
        add_data_type(type_name, project_config['types'][type_name])

    # load components
    for component in project_config['components']:
        component_valid = True
        if not os.path.isdir(component_folder_pattern.format(component)):
            print('Component folder for {} does not exist'.format(component))
            component_valid = False
        else:
            required_files = ['config.json', component + '.c', component + '.h']
            for file in required_files:
                if not os.path.isfile(component_file_pattern.format(component, file)):
                    print('{} does not exist in component {}'.format(file, component))
                    component_valid = False

        if component_valid:
            component_config_file = component_file_pattern.format(component, 'config.json')
            try:
                component_data[component] = load_component_config(component_config_file)

                for data_type in component_data[component]['types']:
                    add_data_type(data_type, component_data[component]['types'][data_type])

            except JSONDecodeError:
                print("Could not read config for {}".format(component))
                component_valid = False

        valid = valid and component_valid

    # validate runnables
    for runnable_group in runtime_config['runnables']:
        for runnable in runtime_config['runnables'][runnable_group]:
            provider_component_name = runnable['component']
            if provider_component_name not in project_config['components']:
                print('Component {} does not exist'.format(provider_component_name))
                valid = False
            else:
                component_config_file = component_file_pattern.format(provider_component_name, 'config.json')
                component_config = component_data[provider_component_name]
                runnable_name = runnable['runnable']
                if runnable_name not in component_config.get('runnables', {}):
                    print('Component {} does not have a runnable called {}'.format(provider_component_name,
                                                                                   runnable_name))
                    valid = False
                elif component_config['runnables'][runnable_name]['arguments']:
                    print('{}_Run_{} must not have arguments'.format(provider_component_name, runnable_name))
                    valid = False


    def are_ports_compatible(provider, consumer):
        try:
            provider_port_data = component_data[provider['component']]['ports'][provider['port']]
            consumer_port_data = component_data[consumer['component']]['ports'][consumer['port']]

            provider_type = provider_port_data['port_type']
            consumer_type = consumer_port_data['port_type']

            provider_data_type = provider_port_data['data_type']
            consumer_data_type = consumer_port_data['data_type']

            return consumer_type in port_compatibility[provider_type] and provider_data_type == consumer_data_type
        except KeyError as e:
            print(e)
            return False


    def are_runnables_compatible(provider, consumer):
        try:
            provider_port_data = component_data[provider['component']]['ports'][provider['port']]
            consumer_port_data = component_data[consumer['component']]['runnables'][consumer['port']]

            provider_type = provider_port_data['port_type']
            consumer_type = 'Runnable'

            # check if port types are compatible
            if consumer_type not in port_compatibility[provider_type]:
                return False

            provider_args = provider_port_data['arguments']
            consumer_args = consumer_port_data['arguments']

            # accept cases when more arguments are provided than used
            for arg in consumer_args:
                if arg not in provider_args:
                    return False

                if provider_args[arg] != consumer_args[arg]:
                    return False

            return True
        except KeyError as e:
            print(e)
            return False


    def port_ref_valid(port):
        component_ports = component_data[port['component']].get('ports', [])
        return port['component'] in component_data and port['port'] in component_ports


    def runnable_ref_valid(port):
        component_runnables = component_data[port['component']].get('runnables', [])
        return port['component'] in component_data and port['port'] in component_runnables


    def port_type(port):
        component_ports = component_data[port['component']]['ports']
        return component_ports[port['port']]['port_type']


    # validate ports
    port_connections = []
    runnable_connections = []

    for port_connection in runtime_config['port_connections']:
        port_valid = True
        provider = port_connection['provider']

        if not port_ref_valid(provider):
            print('Provider port invalid: {}/{}'.format(provider['component'], provider['port']))
            port_valid = False

        allow_multiple = provider.get('multiple_consumers', False)
        if not allow_multiple:
            if len(port_connection['consumers']) > 1:
                print('Port {}/{} requires at most one consumer'.format(provider['component'], provider['port']))
                port_valid = False

        provider_port_type = port_type(provider)
        if provider_port_type in provider_port_templates:
            for consumer in port_connection['consumers']:
                if not port_ref_valid(consumer):
                    print('Consumer of {}/{} invalid: {}/{}'.format(provider['component'], provider['port'],
                                                                    consumer['component'], consumer['port']))
                    port_valid = False

                if not are_ports_compatible(provider, consumer):
                    print('Consumer port {}/{} is incompatible with {}/{}'.format(
                        consumer['component'], consumer['port'],
                        provider['component'],
                        provider['port']))
                    port_valid = False

            if port_valid:
                port_connections.append(port_connection)

        elif provider_port_type in runnable_connection_templates:
            for consumer in port_connection['consumers']:
                if not runnable_ref_valid(consumer):
                    print('Consumer of {}/{} invalid: {}/{}'.format(provider['component'], provider['port'],
                                                                    consumer['component'], consumer['port']))
                    port_valid = False

                if not are_runnables_compatible(provider, consumer):
                    print('Consumer runnable {}/{} is incompatible with {}/{}'.format(
                        consumer['component'], consumer['port'],
                        provider['component'], provider['port']))
                    port_valid = False

            if port_valid:
                runnable_connections.append(port_connection)
        else:
            print('Unknown provider port type: {}'.format(provider_port_type))
            port_valid = False

        valid = valid and port_valid

    if not valid:
        sys.exit(1)

    if args.validate_only:
        if valid:
            print("Runtime configuration is valid")
        sys.exit(0)

    template_ctx = {
        'output_filename': args.output[args.output.rfind('/') + 1:],
        'includes':        ['components/{0}/{0}'.format(component) for component in project_config['components']],
        'runnable_groups': [],
        'data_buffers':    [],
        'port_functions':  []
    }

    for runnable_group in runtime_config['runnables']:
        group = {'group_name': runnable_group, 'runnables': []}

        for runnable in runtime_config['runnables'][runnable_group]:
            group['runnables'].append(runnable)

        template_ctx['runnable_groups'].append(group)

    for port_connection in port_connections:
        provider = port_connection['provider']

        provider_component_name = provider['component']
        provider_port_name = provider['port']

        provider_port_data = component_data[provider_component_name]['ports'][provider_port_name]
        data_type = provider_port_data['data_type']

        provider_port_type = provider_port_data['port_type']
        data_buffer_ctx = {
            'data_type':      data_type,
            'component_name': provider_component_name,
            'port_name':      provider_port_name,
            'init_value':     provider.get('init_value', type_default_values[data_type])
        }

        try:
            data_buffer = pystache.render(databuffer_templates[provider_port_type], data_buffer_ctx)
            template_ctx['data_buffers'].append(data_buffer)
        except KeyError:
            pass

        try:
            template = provider_port_templates[provider_port_type]
            if template is not None:
                provider_port = pystache.render(template, data_buffer_ctx)
                template_ctx['port_functions'].append(provider_port)
        except KeyError:
            pass

        for consumer in port_connection['consumers']:
            consumer_component_name = consumer['component']
            consumer_port_name = consumer['port']

            consumer_port_data = component_data[consumer_component_name]['ports'][consumer_port_name]
            consumer_port_type = consumer_port_data['port_type']

            consumer_ctx = {
                'data_type':               data_type,
                'provider_port_name':      provider_port_name,
                'provider_component_name': provider_component_name,
                'consumer_port_name':      consumer_port_name,
                'consumer_component_name': consumer_component_name
            }

            consumer_port = pystache.render(consumer_port_templates[consumer_port_type][provider_port_type],
                                            consumer_ctx)
            template_ctx['port_functions'].append(consumer_port)

    for runnable_connection in runnable_connections:
        provider = runnable_connection['provider']

        call_impls = []

        provider_port_data = component_data[provider['component']]['ports'][provider['port']]
        provider_arguments = provider_port_data['arguments']
        arg_map = [{'name': arg, 'type': provider_arguments[arg]} for arg in provider_arguments]
        if arg_map:
            arg_map[len(arg_map) - 1]['last'] = True

        for consumer in runnable_connection['consumers']:
            consumer_component_name = consumer['component']
            consumer_port_name = consumer['port']
            consumer_port_type = 'Runnable'

            consumer_ctx = {
                'consumer_port_name':      consumer_port_name,
                'consumer_component_name': consumer_component_name,
                'arguments':               arg_map
            }

            consumer_port = pystache.render(runnable_call_templates[consumer_port_type], consumer_ctx)
            call_impls.append(consumer_port)

        ctx = {
            'component_name': provider['component'],
            'port_name':      provider['port'],
            'runnables':      call_impls,
            'arguments':      arg_map,
            'return_type':    provider_port_data['return_type'],
            'void':           provider_port_data['return_type'] == 'void'
        }

        template_ctx['port_functions'].append(pystache.render(runnable_connection_templates[port_type(provider)], ctx))

    with open(args.output + ".h", "w+") as header:
        header.write(pystache.render(header_template, template_ctx))

    with open(args.output + ".c", "w+") as source:
        source.write(pystache.render(source_template, template_ctx))
