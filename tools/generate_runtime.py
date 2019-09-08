"""
Generate a c/h file pair that contains runnable calls and port connections based on project.json
"""
import argparse
import json
import os
import sys
from json import JSONDecodeError

import pystache

port_compatibility = {
    "WriteData": {
        "ReadValue": {"multiple_consumers": True}
    }
}

databuffer_templates = {
    "WriteData": "static {{data_type}} {{component_name}}_{{port_name}}_databuffer = {{{ init_value }}};"
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

port_templates = {
    "WriteData": port_template_write_data,
    "ReadValue": port_template_read_value
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


def parse_runnable(runnable):
    """Parse shorthand form of runnable reference into a dictionary"""
    if type(runnable) is str:
        parts = runnable.split('/')
        runnable = {
            'component': parts[0],
            'runnable': parts[1]
        }

    return runnable


def parse_port(port):
    """Parse shorthand form of port reference into a dictionary"""
    if type(port) is str:
        parts = port.split('/')
        port = {
            'component': parts[0],
            'port': parts[1]
        }

    return port


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', help='Name of project config json file', default="project.json")
    parser.add_argument('--validate-only', help='Skip generating code, only check the config json', action="store_true")
    parser.add_argument('--output', help='File name of generated files', default="rrrc/generated_runtime")

    args = parser.parse_args()

    with open(args.config, "r") as f:
        config = json.load(f)

    component_data = {}

    # validate components
    valid = True
    for component in config['components']:
        component_valid = True
        if not os.path.isdir('rrrc/components/{}'.format(component)):
            print('Component folder for {} does not exist'.format(component))
            component_valid = False
        else:
            required_files = ['config.json', component + '.c', component + '.h']
            for file in required_files:
                if not os.path.isfile('rrrc/components/{}/{}'.format(component, file)):
                    print('{} does not exist in component {}'.format(file, component))
                    component_valid = False

        if component_valid:
            component_config_file = 'rrrc/components/{}/config.json'.format(component)
            with open(component_config_file, "r") as f:
                try:
                    component_data[component] = json.load(f)
                except JSONDecodeError:
                    print("Could not read config for {}".format(component))
                    component_valid = False

        valid = valid and component_valid

    # validate runnables
    for runnable_group in config['runtime']['runnables']:
        for runnable in config['runtime']['runnables'][runnable_group]:
            runnable = parse_runnable(runnable)
            component_name = runnable['component']
            if component_name not in config['components']:
                print('Component {} does not exist'.format(component_name))
                valid = False
            else:
                component_config_file = 'rrrc/components/{}/config.json'.format(component_name)
                component_config = component_data[component_name]
                runnable_name = runnable['runnable']
                if runnable_name not in component_config.get('runnables', {}):
                    print('Component {} does not have a runnable called {}'.format(component_name, runnable_name))
                    valid = False
                elif component_config['runnables'][runnable_name]['arguments']:
                    print('{}_Run_{} must not have arguments'.format(component_name, runnable_name))
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
        except KeyError:
            return False


    def port_ref_valid(port):
        component_ports = component_data[port['component']].get('ports', [])
        return port['component'] in component_data and port['port'] in component_ports

    data_buffers = []
    port_functions = []

    # validate ports
    for port_connection in config['runtime'].get('port_connections', []):
        port_valid = True
        provider = parse_port(port_connection['provider'])
        if not port_ref_valid(provider):
            print('Provider port invalid: {}/{}'.format(provider['component'], provider['port']))
            port_valid = False

        allow_multiple = provider.get('multiple_consumers', False)
        if not allow_multiple:
            if len(port_connection['consumers']) > 1:
                print('Port {}/{} requires at most one consumer'.format(provider['component'], provider['port']))
                port_valid = False

        for consumer in port_connection['consumers']:
            consumer = parse_port(consumer)
            if not port_ref_valid(consumer):
                print('Consumer of {}/{} invalid: {}/{}'.format(provider['component'], provider['port'],
                                                                consumer['component'], consumer['port']))
                port_valid = False

            if not are_ports_compatible(provider, consumer):
                print('Consumer port {}/{} is incompatible with {}/{}'.format(consumer['component'], consumer['port'],
                                                                              provider['component'], provider['port']))
                port_valid = False

        if port_valid:
            component_name = provider['component']
            port_name = provider['port']

            port_data = component_data[component_name]['ports'][port_name]
            data_type = port_data['data_type']

            provider_port_type = port_data['port_type']
            data_buffer_ctx = {
                'data_type': data_type,
                'component_name': component_name,
                'port_name': port_name,
                'init_value': 0
            }
            data_buffer = pystache.render(databuffer_templates[provider_port_type], data_buffer_ctx)
            provider_port = pystache.render(port_templates[provider_port_type], data_buffer_ctx)

            data_buffers.append(data_buffer)
            port_functions.append(provider_port)

            for consumer in port_connection['consumers']:
                consumer = parse_port(consumer)

                consumer_component_name = consumer['component']
                consumer_port_name = consumer['port']

                consumer_port_data = component_data[consumer_component_name]['ports'][consumer_port_name]
                consumer_port_type = consumer_port_data['port_type']

                consumer_ctx = {
                    'data_type': data_type,
                    'provider_port_name': port_name,
                    'provider_component_name': component_name,
                    'consumer_port_name': consumer_port_name,
                    'consumer_component_name': consumer_component_name
                }

                consumer_port = pystache.render(port_templates[consumer_port_type], consumer_ctx)
                port_functions.append(consumer_port)
        valid = valid and port_valid

    if not valid:
        sys.exit(1)

    if args.validate_only:
        if valid:
            print("Runtime configuration is valid")
        sys.exit(0)

    template_ctx = {
        'output_filename': args.output[args.output.rfind('/') + 1:],
        'includes':        ['components/{0}/{0}'.format(component) for component in config['components']],
        'runnable_groups': [],
        'data_buffers':    data_buffers,
        'port_functions':  port_functions
    }

    for runnable_group in config['runtime']['runnables']:
        group = {'group_name': runnable_group, 'runnables': []}

        for runnable in config['runtime']['runnables'][runnable_group]:
            group['runnables'].append(runnable)

        template_ctx['runnable_groups'].append(group)

    with open(args.output + ".h", "w+") as header:
        header.write(pystache.render(header_template, template_ctx))

    with open(args.output + ".c", "w+") as source:
        source.write(pystache.render(source_template, template_ctx))
