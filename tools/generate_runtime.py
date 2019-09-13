"""
Generate a c/h file pair that contains runnable calls and port connections based on project.json
"""
import argparse
import os
import sys
from json import JSONDecodeError

import pystache

from tools.generator_common import component_folder_pattern, component_file_pattern, load_component_config, \
    load_project_config, to_underscore, collect_type_aliases, TypeCollection, change_file, \
    empty_component, parse_port_reference, process_port_def


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
    "queue":    "",
    "queue_1":  """static {{data_type}} {{ buffer_name }};
static bool {{ buffer_name }}_overflow = false;
static bool {{ buffer_name }}_data_valid = false;""",
    "constant": None
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
    "queue":    {
        "value":   "",
        "pointer": ""
    },
    "queue_1":  {
        "value":   """{{ buffer_name }}_overflow = {{ buffer_name }}_data_valid;
    {{ buffer_name }} = {{ value }};
    {{ buffer_name }}_data_valid = true;""",
        "pointer": """{{ buffer_name }}_overflow = {{ buffer_name }}_data_valid;
    {{ buffer_name }} = *{{ value }};
    {{ buffer_name }}_data_valid = true;""",
    },
    "constant": None,
    "call":     None,
    "event":    None,
}

provider_port_types = {
    "Constant":         ['constant'],
    "WriteData":        ["variable", "queue", "queue_1"],
    "WriteIndexedData": ["array"],
    "WriteDataToIndex": ["array"],  # virtual type to support complex connections
    "ServerCall":       ["call"],
    "Event":            ["event"],
}

consumer_port_types = {
    "ReadValue":  ["variable", "constant"],
    "ReadIndexedValue": ["array"],
    "ReadQueuedValue": ["queue"],  # queue_1 is an optimization so it's omitted
    "ReadValueFromIndex": ["array"],  # virtual type to support complex connections
    "Runnable": ["event", "call"]
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
        "queue": "",
        "queue_1": """bool was_overflow = {{ buffer_name }}_overflow;
    if ({{ buffer_name }}_data_valid)
    {
        {{ buffer_name }}_overflow = false;
        *{{ out_name }} = {{ buffer_name }};
        {{ buffer_name }}_data_valid = false;
        
        if (was_overflow)
        {
            return QueueStatus_Overflow;
        }
        else
        {
            return QueueStatus_Ok;
        }
    }
    else
    {
        return QueueStatus_Empty;
    }""",
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
                    print('Component {} does not have a runnable called {}'
                          .format(provider_component_name, runnable_name))
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


def load_types(project_config, component_data, type_data: TypeCollection):
    # load types defined in project config
    for type_name in project_config['types']:
        type_data.add(type_name, project_config['types'][type_name])

    # load types defined in component config
    for component in component_data:
        for data_type in component_data[component]['types']:
            type_data.add(data_type, component_data[component]['types'][data_type])


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


    def create_runtime_events(project_config, component_data):
        """
        Gather runnable groups and create a virtual component that provides events for them
        """
        if 'Runtime' in component_data:
            raise Exception('Runtime component already exists')

        component_data['Runtime'] = empty_component('Runtime')

        event_connections = []
        for runnable_group in project_config['runtime']['runnables']:
            def create_port_ref(runnable):
                return {
                    'short_name': runnable['short_name'],
                    'component':  runnable['component'],
                    'port':       runnable['runnable']
                }

            log('Creating runtime event: {}'.format(runnable_group))
            event_port = process_port_def('Runtime', runnable_group, {'port_type': 'Event'})
            component_data['Runtime']['ports'][runnable_group] = event_port
            event_connections.append({
                'providers': [parse_port_reference(event_port['short_name'])],
                'consumers': [create_port_ref(runnable_ref) for runnable_ref in
                              project_config['runtime']['runnables'][runnable_group]]
            })

        project_config['runtime']['port_connections'] += event_connections


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
        possible_types = []
        for provider in raw_connection['providers']:
            try:
                provider_port = get_port(provider, component_data)
                port_type = provider_port['port_type']
                possible_types.extend(provider_port_types[port_type])

                if not port_allows_multiple_consumers[port_type]:
                    if len(raw_connection['consumers']) > 1:
                        raise Exception(
                            'Provider port {} does not allow multiple consumers'.format(provider_port['short_name']))
            except KeyError as e:
                print(e)
                raise

        for consumer in raw_connection['consumers']:
            consumer_component = component_data[consumer['component']]

            if consumer['port'] in consumer_component['ports']:
                compatible_types = []
                consumer_port = consumer_component['ports'][consumer['port']]
                for consumer_type in consumer_port_types[consumer_port['port_type']]:
                    if consumer_type in possible_types:
                        compatible_types.append(consumer_type)
                possible_types = compatible_types
            elif consumer['port'] in consumer_component['runnables']:
                if 'call' not in possible_types and 'event' not in possible_types:
                    possible_types = []
            else:
                raise Exception('Unknown port or runnable: {}'.format(consumer['short_name']))

            if not possible_types:
                provider_names = [provider['short_name'] for provider in raw_connection['providers']]
                provider_name_list = ', '.join(provider_names)
                raise Exception('Connection with providers {} has no compatible type. Caused by consumer {}'
                                .format(provider_name_list, consumer['short_name']))

        if len(possible_types) > 1:
            raise Exception('Multiple possible port connection types')

        return possible_types[0]


    def classify_connections(project_config, component_data):
        connections = {}

        for connection in project_config['runtime']['port_connections']:
            connection_type = determine_connection_type(connection, component_data)

            try:
                if connection_type == 'queue' and connection['queue_length'] == 1:
                    connection_type = 'queue_1'
            except KeyError as e:
                print(connection)
                print(e)
                raise

            try:
                connections[connection_type].append(connection)
            except KeyError:
                connections[connection_type] = [connection]

        return connections


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

    type_data = TypeCollection()

    log('')
    log('Load types')
    load_types(project_config, component_data, type_data)

    create_runtime_events(project_config, component_data)
    classified_connections = classify_connections(project_config, component_data)


    def are_runnables_compatible(provider, consumer):
        try:
            provider_port_data = get_port(provider, component_data)
            consumer_port_data = get_runnable(consumer, component_data)

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
        try:
            return port['port'] in component_data[port['component']]['ports']
        except KeyError as e:
            log('{}: {}'.format(type(e).__name__, e))
            return False


    def runnable_ref_valid(port):
        try:
            return port['port'] in component_data[port['component']]['runnables']
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


    def create_function(caller, port_functions, component_data):
        if caller['short_name'] not in port_functions:
            caller_port = get_port(caller, component_data)
            function_name = '{}_Call_{}'.format(caller['component'], caller['port'])
            arguments = [{'name': arg, 'type': caller_port['arguments'][arg]} for arg in caller_port['arguments']]
            if arguments:
                arguments[len(arguments) - 1]['last'] = True
            port_functions[caller['short_name']] = {
                'port':          caller,
                'type':          'runnable_connection',
                'function_name': function_name,
                'return_type':   caller_port['return_type'],
                'arguments':     arguments,
                'body':          []
            }
            log('Created function: {}'.format(function_name))
            log('  Return type: {}'.format(caller_port['return_type']))
            log('  Arguments: {}'.format(", ".join(["{}: {}".format(arg['name'], arg['type']) for arg in arguments])))
        return port_functions[caller['short_name']]


    def create_port_function(port, port_functions, component_data):
        if port['short_name'] not in port_functions:
            port_data = component_data[port['component']]['ports'][port['port']]
            port_type = port_data['port_type']

            data_type = port_data.get('data_type', 'void')
            port_data_templates = {
                "WriteData":
                    lambda: {
                        "name":        "{}_Write_{}",
                        "return_type": "void",
                        "arguments":   [{
                            'name': 'value',
                            'type': data_type if type_data[data_type]['pass_semantic'] == 'value'
                                    else "const {}*".format(data_type)}],
                    },
                "WriteIndexedData":
                    lambda: {
                        "name":        "{}_Write_{}",
                        "return_type": "void",
                        "arguments":   [
                            {'name': 'index', 'type': 'uint32_t'},
                            {'name': 'value', 'type': data_type}
                        ]
                    },
                "ReadValue":
                    lambda: {
                        "name":        "{}_Read_{}",
                        "return_type": data_type,
                        "arguments":   []
                    } if type_data[data_type]['pass_semantic'] == 'value' else {
                        "name":        "{}_Read_{}",
                        "return_type": 'void',
                        "arguments":   [{'name': 'value', 'type': "{}*".format(data_type)}]
                    },
                "ReadQueuedValue":
                    lambda: {
                        "name":        "{}_Read_{}",
                        "return_type": 'QueueStatus_t',
                        "arguments":   [{'name': 'value', 'type': "{}*".format(data_type)}]
                    },
                "ReadIndexedValue":
                    lambda: {
                        "name":        "{}_Read_{}",
                        "return_type": data_type,
                        "arguments":   [{'name': 'index', 'type': 'uint32_t'}]
                    },
                "Constant":
                    lambda: {
                        "name":        "{}_Constant_{}",
                        "return_type": data_type,
                        "arguments":   []
                    } if type_data[data_type]['pass_semantic'] == 'value' else {
                        "name":        "Constant_{}",
                        "return_type": 'void',
                        "arguments":   [{'name': 'value', 'type': "{}*".format(data_type)}]
                    }
            }

            data = port_data_templates[port_type]()
            function_name = data['name'].format(port['component'], port['port'])
            arguments = data['arguments']
            if arguments:
                arguments[len(arguments) - 1]['last'] = True
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


    for runnable_connections in classified_connections['event']:
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

    for server_call in classified_connections['call']:
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
    for variable_connection in classified_connections['variable']:
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
                provider_function = create_port_function(provider, port_functions, component_data)

                ctx = {
                    'data_type':   provider_port['data_type'],
                    'buffer_name': databuffer_name,
                    'init_value':  provider.get('init_value', type_data.default_value(provider_port['data_type'])),
                    'value':       provider_function['arguments'][0]['name']
                }
                variables[name] = pystache.render(databuffer_buffer_templates['variable'], ctx)
                write_template = databuffer_write_templates['variable'][pass_by]
                provider_function['body'].append(pystache.render(write_template, ctx))

            for consumer in variable_connection['consumers']:
                consumer_function = create_port_function(consumer, port_functions, component_data)

                consumer_port_type = get_port(consumer, component_data)['port_type']
                read_template = consumer_templates[consumer_port_type]['variable'][pass_by]
                function_body = pystache.render(read_template, {
                    'buffer_name': databuffer_name,
                    'out_name':    consumer_function['arguments'][0]['name'] if pass_by == 'pointer' else ''
                })
                consumer_function['body'].append(function_body)
        except KeyError:
            print(variable_connection)
            raise

    for variable_connection in classified_connections['array']:
        try:
            # a single databuffer is generated for each write port for now, TODO lift this restriction
            assert len(variable_connection['providers']) == 1
            provider = variable_connection['providers'][0]
            provider_port = get_port(provider, component_data)

            databuffer_name = pystache.render(databuffer_name_template, {
                'component_name': provider['component'],
                'port_name':      provider['port']
            })

            pass_by = type_data[provider_port['data_type']]['pass_semantic']
            name = 'array_{}'.format(provider['short_name'])
            assert_template = 'ASSERT({{index}} < ARRAY_SIZE({{ buffer_name }}));'
            count = provider_port['count']

            if name not in variables:
                provider_function = create_port_function(provider, port_functions, component_data)
                ctx = {
                    'data_type':   provider_port['data_type'],
                    'buffer_name': databuffer_name,
                    'size':        count,
                    'index':       provider_function['arguments'][0]['name'],
                    'value':       provider_function['arguments'][1]['name'],
                    'init_values': [{'value': value} for value in provider.get('init_values',
                                                [type_data.default_value(provider_port['data_type'])] * count)]
                }
                ctx['init_values'][len(ctx['init_values']) - 1]['last'] = True

                if not provider_function['body']:
                    provider_function['body'].append(pystache.render(assert_template, ctx))

                variables[name] = pystache.render(databuffer_buffer_templates['array'], ctx)
                write_template = databuffer_write_templates['array'][pass_by]
                provider_function['body'].append(pystache.render(write_template, ctx))

            for consumer in variable_connection['consumers']:
                consumer_function = create_port_function(consumer, port_functions, component_data)

                consumer_port_type = get_port(consumer, component_data)['port_type']
                write_template = consumer_templates[consumer_port_type]['array'][pass_by]

                ctx = {
                    'buffer_name': databuffer_name,
                    'index':       consumer_function['arguments'][0]['name'],
                    'out_name':    consumer_function['arguments'][1]['name'] if pass_by == 'pointer' else ''
                }

                if not consumer_function['body']:
                    consumer_function['body'].append(pystache.render(assert_template, ctx))

                function_body = pystache.render(write_template, ctx)
                consumer_function['body'].append(function_body)
        except KeyError:
            print(variable_connection)
            raise

    for variable_connection in classified_connections['queue_1']:
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
                provider_function = create_port_function(provider, port_functions, component_data)

                ctx = {
                    'data_type':   provider_port['data_type'],
                    'buffer_name': databuffer_name,
                    'init_value':  provider.get('init_value', type_data.default_value(provider_port['data_type'])),
                    'value':       provider_function['arguments'][0]['name']
                }
                variables[name] = pystache.render(databuffer_buffer_templates['queue_1'], ctx)
                write_template = databuffer_write_templates['queue_1'][pass_by]
                provider_function['body'].append(pystache.render(write_template, ctx))

            for consumer in variable_connection['consumers']:
                consumer_function = create_port_function(consumer, port_functions, component_data)

                consumer_port_type = get_port(consumer, component_data)['port_type']
                read_template = consumer_templates[consumer_port_type]['queue_1']
                function_body = pystache.render(read_template, {
                    'buffer_name': databuffer_name,
                    'out_name':    consumer_function['arguments'][0]['name']
                })
                consumer_function['body'].append(function_body)
        except KeyError:
            print(variable_connection)
            raise

    for constant_connection in classified_connections['constant']:
        try:
            assert len(constant_connection['providers']) == 1
            provider = constant_connection['providers'][0]
            provider_port = get_port(provider, component_data)
            pass_by = type_data[provider_port['data_type']]['pass_semantic']

            for consumer in constant_connection['consumers']:
                consumer_function = create_port_function(consumer, port_functions, component_data)
                consumer_port_type = get_port(consumer, component_data)['port_type']
                const_template = consumer_templates[consumer_port_type]['constant'][pass_by]
                function_body = pystache.render(const_template, {
                    'constant_provider': '{}_Constant_{}'.format(provider['component'], provider['port']),
                    'out_name':          consumer_function['arguments'][0]['name'] if pass_by == 'pointer' else ''
                })
                consumer_function['body'].append(function_body)
        except KeyError:
            print(constant_connection)
            raise

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
