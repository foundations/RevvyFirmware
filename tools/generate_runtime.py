"""
Generate a c/h file pair that contains runnable calls and port connections based on project.json
"""
import argparse
import os
import sys
from json import JSONDecodeError

import pystache

from tools.generator_common import component_folder_pattern, component_file_pattern, \
    load_component_config, load_project_config, add_data_type, to_underscore, collect_type_aliases, resolve_type

port_compatibility = {
    "WriteData":        {
        "ReadValue": {"multiple_consumers": True}
    },
    "WriteIndexedData": {
        "ReadValue":        {"multiple_consumers": True},
        "ReadIndexedValue": {"multiple_consumers": True}
    },
    "Constant":         {
        "ReadValue": {"multiple_consumers": True}
    },
    "Event":            {
        "Runnable": {"multiple_consumers": True}
    },
    "ServerCall":       {
        "Runnable": {"multiple_consumers": False}
    }
}

databuffer_name_template = "{{component_name}}_{{port_name}}_databuffer"

databuffer_templates = {
    "variable": "static {{data_type}} " + databuffer_name_template + " = {{{ init_value }}};"
}

port_template_write_data = """void {{component_name}}_Write_{{port_name}}({{data_type}} value)
{
    {{component_name}}_{{port_name}}_databuffer = value;
}
"""

port_template_write_data_pointer = """void {{component_name}}_Write_{{port_name}}(const {{data_type}}* value)
{
    {{component_name}}_{{port_name}}_databuffer = *value;
}
"""

port_template_read_value = """{{data_type}} {{consumer_component_name}}_Read_{{consumer_port_name}}(void)
{
    return {{provider_component_name}}_{{provider_port_name}}_databuffer;
}
"""

port_template_read_value_pointer = """void {{consumer_component_name}}_Read_{{consumer_port_name}}({{data_type}}* value)
{
    *value = {{provider_component_name}}_{{provider_port_name}}_databuffer;
}
"""

port_template_read_constant = """{{data_type}} {{consumer_component_name}}_Read_{{consumer_port_name}}(void)
{
    return {{provider_component_name}}_Constant_{{provider_port_name}}();
}
"""

port_template_read_constant_pointer = """void {{consumer_component_name}}_Read_{{consumer_port_name}}({{data_type}}* value)
{
    {{provider_component_name}}_Constant_{{provider_port_name}}(value);
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
    "WriteData":            port_template_write_data,
    "WriteIndexedData":     port_template_write_data,
    "Constant":             None,
    "WriteData_ptr":        port_template_write_data_pointer,
    "WriteIndexedData_ptr": port_template_write_data_pointer,
    "Constant_ptr":         None
}

consumer_port_templates = {
    "ReadValue":        {
        "WriteData":     port_template_read_value,
        "WriteData_ptr": port_template_read_value_pointer,
        "Constant":      port_template_read_constant,
        "Constant_ptr":  port_template_read_constant_pointer,
    },
    "ReadIndexedValue": {
        "WriteIndexedData": port_template_read_value
    }
}

typedef_template = """{{ #aliased }}
typedef {{ aliased }} {{ type_name }};
{{ /aliased }}
{{ #is_enum }}
typedef enum {
    {{ #values }}
    {{ value }}{{ ^last }},{{ /last }}
    {{ /values }}
} {{ type_name }};
{{ /is_enum }}
{{ #is_struct }}
typedef struct {
    {{ #fields }}
    {{ type }} {{ name }};
    {{ /fields }}
} {{ type_name }};
{{ /is_struct }}"""

header_template = """#ifndef GENERATED_RUNTIME_H_
#define GENERATED_RUNTIME_H_

{{ #type_includes }}
#include {{{.}}}
{{ /type_includes }}

{{ #types }}
""" + typedef_template + """
{{ /types }}

{{ #components }}
#define COMPONENT_TYPES_{{ guard_def }}_H_
{{ /components }}

{{ #components }}
#include "components/{{ name }}/{{ name }}.h"
{{ /components }}

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


def validate_runnables(project_config, component_data):
    """Check runnables in the runtime configuration.

    Validate runnables against their definitions in their respective component configurations"""

    runnables_valid = True
    runtime_config = project_config['runtime']
    for runnable_group in runtime_config['runnables']:
        for runnable in runtime_config['runnables'][runnable_group]:
            provider_component_name = runnable['component']
            if provider_component_name not in project_config['components']:
                print('Component {} does not exist'.format(provider_component_name))
                runnables_valid = False
            else:
                component_config = component_data[provider_component_name]
                runnable_name = runnable['runnable']
                if runnable_name not in component_config.get('runnables', {}):
                    print('Component {} does not have a runnable called {}'.format(provider_component_name,
                                                                                   runnable_name))
                    runnables_valid = False
                elif component_config['runnables'][runnable_name]['arguments']:
                    print('{}_Run_{} must not have arguments'.format(provider_component_name, runnable_name))
                    runnables_valid = False
    return runnables_valid


def load_component(component):
    if not os.path.isdir(component_folder_pattern.format(component)):
        raise Exception('Component folder for {} does not exist'.format(component))

    required_files = ['config.json', component + '.c', component + '.h']
    for file in required_files:
        if not os.path.isfile(component_file_pattern.format(component, file)):
            raise Exception('{} does not exist in component {}'.format(file, component))

    component_config_file = component_file_pattern.format(component, 'config.json')
    try:
        return load_component_config(component_config_file)
    except JSONDecodeError:
        raise Exception("Could not read config for {}".format(component))


def load_types(project_config, component_data, type_data, resolved_types):
    # load types defined in project config
    for type_name in project_config['types']:
        add_data_type(type_name, project_config['types'][type_name], type_data, resolved_types)

    # load types defined in component config
    for component in component_data:
        for data_type in component_data[component]['types']:
            add_data_type(data_type, component_data[component]['types'][data_type], type_data, resolved_types)


def create_runnable_groups(config):
    runnable_groups = []
    for runnable_group in config:
        runnable_groups.append({
            'group_name': runnable_group,
            'runnables':  (config[runnable_group])
        })
    return runnable_groups


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', help='Name of project config json file', default="project.json")
    parser.add_argument('--verbose', help='Print additional information', action="store_true")
    parser.add_argument('--validate-only', help='Skip generating code, only check the config json', action="store_true")
    parser.add_argument('--output', help='File name of generated files', default="rrrc/generated_runtime")

    args = parser.parse_args()

    if args.verbose:
        def log(x):
            print(x)
    else:
        def log(x):
            pass

    log('Loading project configuration from {}'.format(args.config))
    project_config = load_project_config(args.config)

    log('')
    log('Load components')
    try:
        component_data = {}
        for component in project_config['components']:
            log('Loading configuration for {}'.format(component))
            component_data[component] = load_component(component)
        valid = True
    except Exception as e:
        print('Failed to load component data')
        log('{}: {}'.format(type(e).__name__, e))
        component_data = {}
        valid = False

    log('')
    log('Validate runnables')
    valid = valid and validate_runnables(project_config, component_data)

    type_data = {}
    resolved_types = {}

    log('')
    log('Load types')
    load_types(project_config, component_data, type_data, resolved_types)


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
            log('{}: {}'.format(type(e).__name__, e))
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
            log('{}: {}'.format(type(e).__name__, e))
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

    data_buffers = {}
    # data buffer name:
    # - single provider -> derive
    # - multiple different provider ports -> pattern + index (combined_data_buffer_5)
    # port types must match exactly [restriction may be lifted later]
    # write types:
    # write to data buffer (WriteData)
    # write to data buffer index (WriteData)
    # indexed write to data buffer index, direct mapping (WriteIndexedData)
    # indexed write to data buffer index, direct mapping with offset (WriteIndexedData)
    # indexed write to data buffer index, indirect mapping (WriteIndexedData)

    port_types = {
        "WriteData":        'provider',
        "WriteIndexedData": 'provider',
        "Constant":         'provider',
        "Event":            'provider',
        "ServerCall":       'provider',
        "Runnable":         'consumer',
        "ReadValue":        'consumer',
        "ReadIndexedValue": 'consumer',
    }

    log('')
    log('Collect provider ports')
    unconnected_provider_ports = []
    for component in component_data:
        component_ports = component_data[component]['ports']
        for port in component_ports:
            port_data = component_ports[port]
            if port_types[port_data['port_type']] == 'provider':
                unconnected_provider_ports.append(port_data['short_name'])

    log('')
    log('Check port connections')
    for port_connection in project_config['runtime']['port_connections']:

        port_valid = True
        providers = port_connection['providers']

        if len(providers) > 1:
            print('Multiple providers are not yet supported')
            port_valid = False
        else:
            provider = providers[0]
            try:
                unconnected_provider_ports.remove(provider['short_name'])
            except ValueError:
                log('Provider port {} is referenced multiple times'.format(provider['short_name']))
                port_valid = False

            if port_valid:
                if not port_ref_valid(provider):
                    print('Provider port invalid: {}'.format(provider['short_name']))
                    port_valid = False

                elif not provider.get('multiple_consumers', False):
                    # provider reference is valid, check consumer count - TODO change this when implementing indexed
                    if len(port_connection['consumers']) > 1:
                        print('Port {} requires at most one consumer'.format(provider['short_name']))
                        port_valid = False

                if port_valid:
                    # port reference is valid
                    # validate and separate runnable and data connections

                    def validate_port(provider, consumer):
                        if not port_ref_valid(consumer):
                            print('Consumer of {} invalid: {}'
                                  .format(provider['short_name'], consumer['short_name']))
                            return False
                        elif not are_ports_compatible(provider, consumer):
                            print('Consumer port {} is incompatible with {}'
                                  .format(consumer['short_name'], provider['short_name']))
                            return False
                        else:
                            return True


                    def validate_runnable(provider, consumer):
                        if not runnable_ref_valid(consumer):
                            print('Consumer of {} invalid: {}'
                                  .format(provider['short_name'], consumer['short_name']))
                            return False
                        elif not are_runnables_compatible(provider, consumer):
                            print('Consumer runnable {} is incompatible with {}'
                                  .format(consumer['short_name'], provider['short_name']))
                            return False
                        else:
                            return True


                    port_conncetion_types = {
                        "WriteData":        'data',
                        "WriteIndexedData": 'data',
                        "Constant":         'data',
                        "Event":            'runnable',
                        "ServerCall":       'runnable'
                    }
                    validators = {
                        "data":     validate_port,
                        "runnable": validate_runnable,
                    }
                    collections = {
                        "data":     port_connections,
                        "runnable": runnable_connections,
                    }

                    provider_port_type = port_type(provider)
                    try:
                        connection_type = port_conncetion_types[provider_port_type]

                        for consumer in port_connection['consumers']:
                            port_valid = port_valid and validators[connection_type](provider, consumer)

                        if port_valid:
                            collections[connection_type].append(port_connection)
                    except KeyError as e:
                        print('Unknown/invalid provider port type: {}'.format(provider_port_type))
                        log('{}: {}'.format(type(e).__name__, e))
                        port_valid = False

        valid = valid and port_valid

    log('')
    log('Validation summary')
    log('=================')
    if unconnected_provider_ports:
        log('Unconnected provider ports:\n * {}'.format("\n * ".join(unconnected_provider_ports)))

    if not valid:
        print("Configuration invalid, exiting")
        sys.exit(1)
    elif args.validate_only:
        print("Runtime configuration is valid, exiting")
        sys.exit(0)

    type_aliases = collect_type_aliases(type_data, type_data, resolved_types)

    type_includes = set()
    for type_alias in type_aliases:
        if 'defined_in' in type_alias:
            type_includes.add(type_alias['defined_in'])

    template_ctx = {
        'output_filename': args.output[args.output.rfind('/') + 1:],
        'includes':        ['components/{0}/{0}'.format(component) for component in project_config['components']],
        'components':      [{
            'name':      component,
            'guard_def': to_underscore(component).upper()
        } for component in project_config['components']],
        'data_buffers':    [],
        'port_functions':  [],
        'types':           type_aliases,
        'type_includes':   sorted(type_includes),
        'runnable_groups': create_runnable_groups(project_config['runtime']['runnables'])
    }


    def default_value(type_name, given_value):
        if given_value is None:
            resolved = resolve_type(type_name, type_data, resolved_types)
            if type_data[resolved]['type'] == 'struct':
                field_defaults = {field: default_value(type_data[resolved]['fields'][field], None) for field in
                                  type_data[resolved]['fields']}
                field_default_strs = ['.{} = {}'.format(field, field_defaults[field]) for field in field_defaults]
                return '{{ {} }}'.format(", ".join(field_default_strs))

            else:
                return type_data[resolved]['default_value']

        return given_value


    for port_connection in port_connections:
        provider = port_connection['providers'][0]

        provider_component_name = provider['component']
        provider_port_name = provider['port']

        provider_port_data = component_data[provider_component_name]['ports'][provider_port_name]
        data_type = provider_port_data['data_type']
        resolved_data_type = resolve_type(data_type, type_data, resolved_types)

        provider_port_type = provider_port_data['port_type']
        data_buffer_ctx = {
            'data_type':      data_type,
            'component_name': provider_component_name,
            'port_name':      provider_port_name,
            'init_value':     default_value(resolved_data_type, provider.get('init_value', None))
        }

        try:
            databuffer_types = {
                "WriteData": "variable"
            }
            data_buffer = pystache.render(databuffer_templates[databuffer_types[provider_port_type]], data_buffer_ctx)
            template_ctx['data_buffers'].append(data_buffer)
        except KeyError:
            log('No databuffer is generated for {} ({})'.format(provider['short_name'], provider_port_type))

        try:
            if type_data[resolved_data_type]['pass_semantic'] == 'pointer':
                template = provider_port_templates[provider_port_type+"_ptr"]
            else:
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

            if type_data[resolved_data_type]['pass_semantic'] == 'pointer':
                template = consumer_port_templates[consumer_port_type][provider_port_type+"_ptr"]
            else:
                template = consumer_port_templates[consumer_port_type][provider_port_type]
            consumer_port = pystache.render(template,
                                            consumer_ctx)
            template_ctx['port_functions'].append(consumer_port)

    for runnable_connection in runnable_connections:
        provider = runnable_connection['providers'][0]

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
