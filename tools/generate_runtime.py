"""
Generate a c/h file pair that contains runnable calls and port connections based on project.json
"""
import argparse
import sys

import pystache

from tools.generator_common import to_underscore, collect_type_aliases, change_file, pystache_list_mark_last, \
    dict_to_pystache_list
from tools.plugins.ComponentConfigCompactor import component_config_compactor
from tools.plugins.ProjectConfigCompactor import project_config_compactor
from tools.plugins.RuntimeEvents import runtime_events
from tools.runtime import Runtime

port_allows_multiple_consumers = {
    "WriteData":        True,
    "WriteIndexedData": True,
    "Constant":         True,
    "Event":            True,
    "ServerCall":       False
}

databuffer_name_template = "{{component_name}}_{{port_name}}_databuffer"

databuffer_buffer_templates = {
    "variable": "static {{data_type}} {{ buffer_name }} = {{{ init_value }}};",
    "array":    "static {{data_type}} {{ buffer_name }}[{{ size }}] = { {{ #init_values }}{{value}}{{^last}}, {{/last}}{{ /init_values }} };",
    "constant": None,

    "queue":    "static {{data_type}} {{ buffer_name }}[{{ queue_length }}u];\n"
                "static size_t {{ buffer_name}}_count = 0u;\n"
                "static size_t {{ buffer_name}}_write_index = 0u;\n"
                "static bool {{ buffer_name }}_overflow = false;",

    "queue_1":  "static {{data_type}} {{ buffer_name }};\n"
                "static bool {{ buffer_name }}_overflow = false;\n"
                "static bool {{ buffer_name }}_data_valid = false;"
}

databuffer_write_templates = {
    "variable": {
        "value":   "{{ buffer_name }} = {{ value }};",
        "pointer": "{{ buffer_name }} = *{{ value }};"
    },
    "array":    {
        "value":   "{{ buffer_name }}[{{ index }}] = {{ value }};",
        "pointer": "{{ buffer_name }}[{{ index }}] = *{{ value }};"
    },
    "constant": None,
    "call":     None,
    "event":    None,

    "queue":    {
        "value":   "if ({{ buffer_name }}_count < {{ queue_length }}u)\n"
                   "{\n"
                   "    ++{{ buffer_name }}_count;\n"
                   "}\n"
                   "else\n"
                   "{\n"
                   "    {{ buffer_name }}_overflow = true;\n"
                   "}\n"
                   "size_t writeIdx = {{ buffer_name }}_write_index;\n"
                   "{{ buffer_name }}_write_index = ({{ buffer_name }}_write_index + 1u) % {{ queue_length }}u;\n"
                   "{{ buffer_name }}[writeIdx] = {{ value }};",

        "pointer": "if ({{ buffer_name }}_count < {{ queue_length }}u)\n"
                   "{\n"
                   "    ++{{ buffer_name }}_count;\n"
                   "}\n"
                   "else\n"
                   "{\n"
                   "    {{ buffer_name }}_overflow = true;\n"
                   "}\n"
                   "size_t writeIdx = {{ buffer_name }}_write_index;\n"
                   "{{ buffer_name }}_write_index = ({{ buffer_name }}_write_index + 1u) % {{ queue_length }}u;\n"
                   "{{ buffer_name }}[writeIdx] = *{{ value }};"
    },

    "queue_1":  {
        "value":   "{{ buffer_name }}_overflow = {{ buffer_name }}_data_valid;\n"
                   "{{ buffer_name }} = {{ value }};\n"
                   "{{ buffer_name }}_data_valid = true;",

        "pointer": "{{ buffer_name }}_overflow = {{ buffer_name }}_data_valid;\n"
                   "{{ buffer_name }} = *{{ value }};\n"
                   "{{ buffer_name }}_data_valid = true;"
    }
}

provider_port_types = {
    "Constant":         ["constant"],
    "WriteData":        ["variable", "queue", "queue_1", "event"],
    "WriteIndexedData": ["array", "event"],
    "WriteDataToIndex": ["array", "event"],  # virtual type to support complex connections
    "ServerCall":       ["call", "event"],
    "Event":            ["event"],
}

consumer_port_types = {
    "ReadValue":          ["variable", "constant"],
    "ReadIndexedValue":   ["array"],
    "ReadQueuedValue":    ["queue"],  # queue_1 is an optimization so it's omitted
    "ReadValueFromIndex": ["array"],  # virtual type to support complex connections
    "Runnable":           ["event", "call"]
}

consumer_templates = {
    "ReadValue":          {
        "variable": {
            "value":   "return {{ buffer_name }};",
            "pointer": "*{{ out_name }} = {{ buffer_name }};"
        },
        "constant": {
            "value":   "return {{ constant_provider }}();",
            "pointer": "{{ constant_provider }}({{ out_name }});"
        },
    },
    "ReadIndexedValue":   {
        "array": {
            "value":   "return {{ buffer_name }}[{{ index }}];",
            "pointer": "*{{ out_name }} = {{ buffer_name }}[{{ index }}];"
        }
    },
    "ReadQueuedValue":    {  # read queued value is always done through pointers because of return value
        "queue":   "if ({{ buffer_name }}_count > 0u)\n"
                   "{\n"
                   "    size_t readIndex = ({{ buffer_name }}_write_index - {{ buffer_name }}_count) % {{ queue_length }}u;\n"
                   "    --{{ buffer_name }}_count;\n"
                   "    *{{ out_name }} = {{ buffer_name }}[readIndex];\n"
                   "    \n"
                   "    if ({{ buffer_name }}_overflow)\n"
                   "    {\n"
                   "        {{ buffer_name }}_overflow = false;\n"
                   "        return QueueStatus_Overflow;\n"
                   "    }\n"
                   "    else\n"
                   "    {\n"
                   "        return QueueStatus_Ok;\n"
                   "    }\n"
                   "}\n"
                   "else\n"
                   "{\n"
                   "    return QueueStatus_Empty;\n"
                   "}",

        "queue_1": "bool was_overflow = {{ buffer_name }}_overflow;\n"
                   "if ({{ buffer_name }}_data_valid)\n"
                   "{\n"
                   "    {{ buffer_name }}_overflow = false;\n"
                   "    *{{ out_name }} = {{ buffer_name }};\n"
                   "    {{ buffer_name }}_data_valid = false;\n"
                   "    if (was_overflow)\n"
                   "    {\n"
                   "        return QueueStatus_Overflow;\n"
                   "    }\n"
                   "    else\n"
                   "    {\n"
                   "        return QueueStatus_Ok;\n"
                   "    }\n"
                   "}\n"
                   "else\n"
                   "{\n"
                   "    return QueueStatus_Empty;\n"
                   "}",
    },
    "ReadValueFromIndex": {  # virtual type to support complex connections
        "array": ""
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
void Runtime_Call_{{ . }}(void);
{{/runnable_groups}}

#endif /* GENERATED_RUNTIME_H */
"""

source_template = """#include "{{ output_filename }}.h"
#include "utils.h"

{{ #_variables }}
{{ #is_simple }}
static {{type}} {{ name }} = {{ init_value }};
{{ /is_simple }}
{{ #is_struct }}
static {{type}} {{ name }} = { {{ #fields }} .{{ name }} = {{ value }}{{ ^last }}, {{ /last }} {{ /fields }} };
{{ /is_struct }}
{{ #is_array }}
static {{type}} {{ name }}[{{ size }}] = { {{ #init_values }} {{ value }}{{ ^last }}, {{ /last }} {{ /init_values }} };
{{ /is_array }}
{{ /_variables }}
{{ #variables }}
{{{ . }}}
{{ /variables }}

{{#port_functions}}
{{ return_type }} {{ function_name }}({{ ^arguments }}void{{ /arguments }}{{ #arguments }}{{type}} {{name}}{{^last}}, {{/last}}{{ /arguments }})
{
    {{ #body }}
    {{{ . }}}
    {{ /body }}
}

{{/port_functions}}
"""


def validate_runnables(project_config, component_data):
    """Check runnables in the runtime configuration.

    Validate runnables against their definitions in their respective component configurations"""

    runnables_valid = True
    runtime_config = project_config['runtime']
    for event_name, runnables in runtime_config['runnables'].items():
        for runnable in runnables:
            provider_component_name = runnable['component']
            runnable_name = runnable['runnable']
            try:
                component_config = component_data[provider_component_name]
                try:
                    if component_config['runnables'][runnable_name]['arguments']:
                        print('{}_Run_{} must not have arguments'.format(provider_component_name, runnable_name))
                        runnables_valid = False
                except KeyError:
                    print('Component {} does not have a runnable called {}'
                          .format(provider_component_name, runnable_name))
                    runnables_valid = False
            except KeyError:
                print('Component {} does not exist'.format(provider_component_name))
                runnables_valid = False

    return runnables_valid


def create_runnable_groups(config):
    return list(config.keys())


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


    def get_runnable(runnable_ref, components):
        component = components[runnable_ref['component']]
        try:
            return component['runnables'][runnable_ref['port']]
        except KeyError:
            print('Invalid runnable reference: {}'.format(runnable_ref))
            print('Component runnables: {}'.format(list(component['runnables'].keys())))
            raise


    def get_port(port_ref, components):
        component = components[port_ref['component']]
        try:
            return component['ports'][port_ref['port']]
        except KeyError:
            print('Invalid port reference: {}'.format(port_ref))
            print('Component ports: {}'.format(list(component['ports'].keys())))
            raise


    def determine_connection_type(raw_connection, component_data):
        possible_types = {}
        extras = raw_connection.copy()
        del extras['consumers']

        for provider in raw_connection['providers']:
            try:
                provider_port = get_port(provider, component_data)
                port_type = provider_port['port_type']

                for connection_type in provider_port_types[port_type]:
                    possible_types[connection_type] = []

                if len(raw_connection['consumers']) == 0:
                    if provider['component'] == 'Runtime':
                        if port_type == 'Event':
                            possible_types['event'] = [{**extras, 'consumers': []}]
                        elif port_type == 'ServerCall':
                            possible_types['call'] = [{**extras, 'consumers': []}]
                if len(raw_connection['consumers']) > 1:
                    if not port_allows_multiple_consumers[port_type]:
                        raise Exception(
                            'Provider port {} does not allow multiple consumers'.format(provider_port['short_name']))
            except KeyError as e:
                print(e)
                raise

        for consumer in raw_connection['consumers']:
            consumer_component = component_data[consumer['component']]

            if consumer['port'] in consumer_component['ports']:
                consumer_port = consumer_component['ports'][consumer['port']]
                added = False
                for consumer_type in consumer_port_types[consumer_port['port_type']]:
                    if consumer_type in possible_types:
                        if consumer_type == 'queue' and raw_connection['queue_length'] == 1:
                            consumer_type = 'queue_1'
                        possible_types[consumer_type].append({**extras, 'consumers': [consumer]})
                        added = True
                if not added:
                    raise Exception('Port {} assigned to incompatible provider'.format(consumer['short_name']))

            elif consumer['port'] in consumer_component['runnables']:
                runnable = get_runnable(consumer, component_data)
                if runnable['return_type'] == 'void':
                    consumer_type = 'event'
                else:
                    consumer_type = 'call'

                if consumer_type in possible_types:
                    possible_types[consumer_type].append({**extras, 'consumers': [consumer]})
                else:
                    raise Exception('Runnable {} assigned to incompatible provider port'.format(consumer['short_name']))

            else:
                raise Exception('Unknown port or runnable: {}'.format(consumer['short_name']))

            if not possible_types:
                provider_names = [provider['short_name'] for provider in raw_connection['providers']]
                provider_name_list = ', '.join(provider_names)
                raise Exception('Connection with providers {} has no compatible type. Caused by consumer {}'
                                .format(provider_name_list, consumer['short_name']))

        return possible_types


    def classify_connections(project_config, component_data):
        connections = {}

        for connection in project_config['runtime']['port_connections']:
            filtered_connections = determine_connection_type(connection, component_data)
            for connection_type, f_connections in filtered_connections.items():
                try:
                    connections[connection_type] += f_connections
                except KeyError:
                    connections[connection_type] = f_connections

        return connections


    log('Loading project configuration from {}'.format(args.config))
    rt = Runtime(args.config)
    rt.add_plugin(project_config_compactor())
    rt.add_plugin(runtime_events())
    rt.add_plugin(component_config_compactor())

    rt.load(True)
    project_config = rt._project_config

    log('')
    log('Load components')
    component_data = rt._components
    valid = True

    log('')
    log('Validate runnables')
    valid = valid and validate_runnables(project_config, component_data)

    type_data = rt._types

    classified_connections = classify_connections(project_config, component_data)


    def are_runnables_compatible(provider, consumer):
        try:
            provider_port_data = get_port(provider, component_data)
            consumer_port_data = get_runnable(consumer, component_data)

            provider_args = provider_port_data.get('arguments', {})
            consumer_args = consumer_port_data['arguments']

            # accept cases when more arguments are provided than used
            common_arg_names = consumer_args.keys() & provider_args.keys()
            if len(common_arg_names) != len(consumer_args):
                return False

            # check if argument types are the same
            for arg in common_arg_names:
                if provider_args[arg] != consumer_args[arg]:
                    return False

            return True
        except KeyError as e:
            log('{}: {}'.format(type(e).__name__, e))
            return False


    type_aliases = collect_type_aliases(type_data, type_data)
    type_includes = set()
    for type_alias in type_aliases:
        if 'defined_in' in type_alias and type_alias['defined_in'] is not None:
            type_includes.add(type_alias['defined_in'])

    port_connections = []
    port_functions = {}


    def create_function(port, port_functions, component_data):
        if port['short_name'] not in port_functions:
            port_data = get_port(port, component_data)
            port_type = port_data['port_type']

            data_type = port_data.get('data_type', 'void')
            passed_by = type_data[data_type]['pass_semantic']

            port_data_templates = {
                "WriteData":
                    lambda: {
                        "name":        "{}_Write_{}",
                        "return_type": "void",
                        "arguments":   {
                            'value': data_type if passed_by == 'value' else "const {}*".format(data_type)},
                    },
                "WriteIndexedData":
                    lambda: {
                        "name":        "{}_Write_{}",
                        "return_type": "void",
                        "arguments":   {
                            'index': 'uint32_t',
                            'value': data_type if passed_by == 'value' else "const {}*".format(data_type)
                        }
                    },
                "ReadValue":
                    lambda: {
                        "name":        "{}_Read_{}",
                        "return_type": data_type,
                        "arguments":   {}
                    } if passed_by == 'value' else {
                        "name":        "{}_Read_{}",
                        "return_type": 'void',
                        "arguments":   {'value': "{}*".format(data_type)}
                    },
                "ReadQueuedValue":
                    lambda: {
                        "name":        "{}_Read_{}",
                        "return_type": 'QueueStatus_t',
                        "arguments":   {'value': "{}*".format(data_type)}
                    },
                "ReadIndexedValue":
                    lambda: {
                        "name":        "{}_Read_{}",
                        "return_type": data_type,
                        "arguments":   {'index': 'uint32_t'},
                    } if passed_by == 'value' else {
                        "name":        "{}_Read_{}",
                        "return_type": 'void',
                        "arguments":   {'index': 'uint32_t', 'value': "{}*".format(data_type)},
                    },
                "Constant":
                    lambda: {
                        "name":        "{}_Constant_{}",
                        "return_type": data_type,
                        "arguments":   {}
                    } if passed_by == 'value' else {
                        "name":        "{}_Constant_{}",
                        "return_type": 'void',
                        "arguments":   {'value': "{}*".format(data_type)}
                    },
                "Event":
                    lambda: {
                        'name':        '{}_Call_{}',
                        'return_type': 'void',
                        'arguments':   port_data.get('arguments', {})
                    },
                "ServerCall":
                    lambda: {
                        'name':        '{}_Call_{}',
                        'return_type': port_data.get('return_type', 'void'),
                        'arguments':   port_data.get('arguments', {})
                    }
            }

            data = port_data_templates[port_type]()
            function_name = data['name'].format(port['component'], port['port'])
            arguments = dict_to_pystache_list(data['arguments'],
                                              key_name='name',
                                              value_name='type',
                                              last_key='last')

            pystache_list_mark_last(arguments)
            port_functions[port['short_name']] = {
                'port':          port,
                'type':          'runnable_connection',
                'function_name': function_name,
                'return_type':   data['return_type'],
                'arguments':     arguments,
                'body':          []
            }
            log('Created function: {}'.format(function_name))
            log('  Return type: {}'.format(data['return_type']))
            log('  Arguments: {}'.format(", ".join(["{}: {}".format(arg['name'], arg['type']) for arg in arguments])))
        return port_functions[port['short_name']]


    for server_call in classified_connections.get('call', []):
        for caller in server_call['providers']:
            function = create_function(caller, port_functions, component_data)

            callee = server_call['consumers'][0]
            if not are_runnables_compatible(caller, callee):
                valid = False
            else:
                handler_runnable = get_runnable(callee, component_data)
                handler_args = handler_runnable['arguments']
                if handler_runnable['return_type'] == 'void':
                    template = "{}_Run_{}({});"
                else:
                    template = "return {}_Run_{}({});"
                function['body'].append(template.format(callee['component'], callee['port'], ', '.join(handler_args)))

    variables = {}
    for variable_connection in classified_connections.get('variable', []):
        try:
            # a single databuffer is generated for each write port
            assert len(variable_connection['providers']) == 1
            provider = variable_connection['providers'][0]
            provider_port = get_port(provider, component_data)

            databuffer_name = pystache.render(databuffer_name_template, {
                'component_name': provider['component'],
                'port_name':      provider['port']
            })

            pass_by = type_data[provider_port['data_type']]['pass_semantic']
            name = 'variable_{}'.format(provider['short_name'])
            if name not in variables:
                provider_function = create_function(provider, port_functions, component_data)

                ctx = {
                    'data_type':   provider_port['data_type'],
                    'buffer_name': databuffer_name,
                    'init_value':  provider.get('init_value', type_data.default_value(provider_port['data_type'])),
                    'value':       provider_function['arguments'][0]['name']
                }
                variables[name] = pystache.render(databuffer_buffer_templates['variable'], ctx)
                write_template = databuffer_write_templates['variable'][pass_by].replace('\n', '\n    ')
                provider_function['body'].append(pystache.render(write_template, ctx))

            for consumer in variable_connection['consumers']:
                consumer_function = create_function(consumer, port_functions, component_data)

                consumer_port_type = get_port(consumer, component_data)['port_type']
                read_template = consumer_templates[consumer_port_type]['variable'][pass_by].replace('\n', '\n    ')
                function_body = pystache.render(read_template, {
                    'buffer_name': databuffer_name,
                    'out_name':    consumer_function['arguments'][0]['name'] if pass_by == 'pointer' else ''
                })
                consumer_function['body'].append(function_body)
        except KeyError:
            print(variable_connection)
            raise

    for variable_connection in classified_connections.get('array', []):
        try:
            # a single databuffer is generated for each write port for now, TODO lift this restriction
            assert len(variable_connection['providers']) == 1
            provider = variable_connection['providers'][0]
            provider_port = get_port(provider, component_data)

            databuffer_name = pystache.render(databuffer_name_template, {
                'component_name': provider['component'],
                'port_name':      provider['port']
            })

            data_type = provider_port['data_type']
            pass_by = type_data[data_type]['pass_semantic']
            name = 'array_{}'.format(provider['short_name'])
            assert_template = 'ASSERT({{index}} < ARRAY_SIZE({{ buffer_name }}));'
            count = provider_port['count']

            if name not in variables:
                provider_function = create_function(provider, port_functions, component_data)
                ctx = {
                    'data_type':   data_type,
                    'buffer_name': databuffer_name,
                    'size':        count,
                    'index':       provider_function['arguments'][0]['name'],
                    'value':       provider_function['arguments'][1]['name'],
                    'init_values': [
                        {'value': value} for value in provider.get('init_values',
                                                                   [type_data.default_value(data_type)] * count)]
                }
                pystache_list_mark_last(ctx['init_values'])

                if not provider_function['body']:
                    provider_function['body'].append(pystache.render(assert_template, ctx))

                variables[name] = pystache.render(databuffer_buffer_templates['array'], ctx)
                write_template = databuffer_write_templates['array'][pass_by].replace('\n', '\n    ')
                provider_function['body'].append(pystache.render(write_template, ctx))

            for consumer in variable_connection['consumers']:
                consumer_function = create_function(consumer, port_functions, component_data)

                consumer_port_type = get_port(consumer, component_data)['port_type']
                read_template = consumer_templates[consumer_port_type]['array'][pass_by].replace('\n', '\n    ')

                ctx = {
                    'buffer_name': databuffer_name,
                    'index':       consumer_function['arguments'][0]['name'],
                    'out_name':    consumer_function['arguments'][1]['name'] if pass_by == 'pointer' else ''
                }

                if not consumer_function['body']:
                    consumer_function['body'].append(pystache.render(assert_template, ctx))

                function_body = pystache.render(read_template, ctx)
                consumer_function['body'].append(function_body)
        except KeyError:
            print(variable_connection)
            raise

    for connection_type in ['queue', 'queue_1']:
        for variable_connection in classified_connections.get(connection_type, []):
            try:
                # a single databuffer is generated for each write port
                assert len(variable_connection['providers']) == 1
                provider = variable_connection['providers'][0]
                provider_port = get_port(provider, component_data)

                databuffer_name = pystache.render(databuffer_name_template, {
                    'component_name': provider['component'],
                    'port_name':      provider['port']
                })

                pass_by = type_data[provider_port['data_type']]['pass_semantic']
                name = 'queue1_{}'.format(provider['short_name'])
                if name not in variables:
                    provider_function = create_function(provider, port_functions, component_data)

                    ctx = {
                        'data_type':    provider_port['data_type'],
                        'buffer_name':  databuffer_name,
                        'init_value':   provider.get('init_value', type_data.default_value(provider_port['data_type'])),
                        'value':        provider_function['arguments'][0]['name'],
                        'queue_length': variable_connection['queue_length']
                    }
                    variables[name] = pystache.render(databuffer_buffer_templates[connection_type], ctx)
                    write_template = databuffer_write_templates[connection_type][pass_by].replace('\n', '\n    ')
                    provider_function['body'].append(pystache.render(write_template, ctx))

                for consumer in variable_connection['consumers']:
                    consumer_function = create_function(consumer, port_functions, component_data)

                    consumer_port_type = get_port(consumer, component_data)['port_type']
                    read_template = consumer_templates[consumer_port_type][connection_type].replace('\n', '\n    ')
                    function_body = pystache.render(read_template, {
                        'buffer_name':  databuffer_name,
                        'out_name':     consumer_function['arguments'][0]['name'],
                        'queue_length': variable_connection['queue_length']
                    })
                    consumer_function['body'].append(function_body)
            except KeyError:
                print(variable_connection)
                raise

    for constant_connection in classified_connections.get('constant', []):
        try:
            assert len(constant_connection['providers']) == 1
            provider = constant_connection['providers'][0]
            provider_port = get_port(provider, component_data)
            pass_by = type_data[provider_port['data_type']]['pass_semantic']

            for consumer in constant_connection['consumers']:
                consumer_function = create_function(consumer, port_functions, component_data)
                consumer_port_type = get_port(consumer, component_data)['port_type']
                const_template = consumer_templates[consumer_port_type]['constant'][pass_by].replace('\n', '\n    ')
                function_body = pystache.render(const_template, {
                    'constant_provider': '{}_Constant_{}'.format(provider['component'], provider['port']),
                    'out_name':          consumer_function['arguments'][0]['name'] if pass_by == 'pointer' else ''
                })
                consumer_function['body'].append(function_body)
        except KeyError:
            print(constant_connection)
            raise

    for runnable_connections in classified_connections.get('event', []):
        for provider in runnable_connections['providers']:
            function = create_function(provider, port_functions, component_data)

            for handler in runnable_connections['consumers']:
                if not are_runnables_compatible(provider, handler):
                    valid = False
                else:
                    handler_runnable = get_runnable(handler, component_data)
                    handler_args = handler_runnable['arguments']
                    function['body'].append(
                        "{}_Run_{}({});".format(handler['component'], handler['port'], ', '.join(handler_args)))

    if not valid:
        print("Configuration invalid, exiting")
        sys.exit(1)
    elif args.validate_only:
        print("Runtime configuration is valid, exiting")
        sys.exit(0)

    template_ctx = {
        'output_filename': args.output[args.output.rfind('/') + 1:],
        'includes':        ['components/{0}/{0}'.format(component) for component in project_config['components']],
        'components':      [{
            'name':      component,
            'guard_def': to_underscore(component).upper()
        } for component in project_config['components']],
        'variables':       variables.values(),
        'port_functions':  port_functions.values(),
        'types':           type_aliases,
        'type_includes':   sorted(type_includes),
        'runnable_groups': create_runnable_groups(project_config['runtime']['runnables'])
    }

    change_file(args.output + '.h', pystache.render(header_template, template_ctx))
    change_file(args.output + '.c', pystache.render(source_template, template_ctx))
