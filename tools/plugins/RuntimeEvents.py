import chevron

from tools.generator_common import TypeCollection
from tools.runtime import RuntimePlugin, Runtime, SignalType, SignalConnection


def collect_arguments(attributes, consumer_name, consumer_arguments, function):
    user_arguments = {}
    if 'arguments' in attributes:
        user_arguments = attributes['arguments']
        for arg in user_arguments:
            if arg not in consumer_arguments:
                print("Warning: Runnable {} does not have an argument named '{}'".format(consumer_name, arg))

    passed_arguments = {}
    for arg_name, arg_type in consumer_arguments.items():
        if arg_name in function.arguments:
            function.mark_argument_used(arg_name)

        if arg_name in user_arguments:
            passed_arguments[arg_name] = user_arguments[arg_name]
        elif arg_name in function.arguments:
            if type(arg_type) is str:
                arg_type = {'direction': 'in', 'data_type': arg_type}

            if arg_type != function.arguments[arg_name]:
                raise Exception(
                    'Caller of {} has matching argument {} but types are different'.format(consumer_name, arg_name))
            passed_arguments[arg_name] = arg_name
        else:
            raise Exception('Unable to connect argument {} of {}'.format(arg_name, consumer_name))

    missing_arguments = set(passed_arguments.keys()) - set(consumer_arguments)
    if missing_arguments:
        raise Exception("The following arguments are missing from {}: {}"
                        .format(consumer_name, ", ".join(missing_arguments)))

    return passed_arguments


class EventSignal(SignalType):
    def __init__(self):
        super().__init__(consumers='multiple')

    def create(self, context, connection: SignalConnection):
        pass

    def generate_provider(self, context, connection: SignalConnection, provider_name):
        pass

    def generate_consumer(self, context, connection: SignalConnection, consumer_name, attributes):
        runtime = context['runtime']

        consumer_port_data = runtime.get_port(consumer_name)
        function = context['functions'][connection.provider]

        component_name, port_name = consumer_name.split('/')
        passed_arguments = collect_arguments(attributes, consumer_name, consumer_port_data['arguments'], function)

        ctx = {
            'template': "{{ component }}_Run_{{ runnable }}({{ arguments }});",
            'data':     {
                'component': component_name,
                'runnable':  port_name,
                'arguments': ', '.join([str(v) for v in passed_arguments.values()])
            }
        }

        return {
            connection.provider: {
                'body': chevron.render(**ctx)
            }
        }


class ServerCallSignal(SignalType):
    def __init__(self):
        super().__init__(consumers='single')

    def create(self, context, connection: SignalConnection):
        pass

    def generate_provider(self, context, connection: SignalConnection, provider_name):
        pass

    def generate_consumer(self, context, connection: SignalConnection, consumer_name, attributes):
        runtime = context['runtime']

        consumer_port_data = runtime.get_port(consumer_name)
        function = context['functions'][connection.provider]

        component_name, port_name = consumer_name.split('/')
        passed_arguments = collect_arguments(attributes, consumer_name, consumer_port_data['arguments'], function)

        data = {
            'component': component_name,
            'runnable':  port_name,
            'arguments': ', '.join([str(v) for v in passed_arguments.values()]),
            'data_type': consumer_port_data['return_type']
        }

        if function.return_type == 'void':
            template = "{{ component }}_Run_{{ runnable }}({{ arguments }});"

            return {
                connection.provider: {
                    'body': chevron.render(template, data)
                }
            }

        else:
            if function.return_type != consumer_port_data['return_type']:
                raise Exception('Callee return type is incompatible ({} instead of {})'
                                .format(consumer_port_data['return_type'], function.return_type))

            template = "{{ data_type }} return_value = {{ component }}_Run_{{ runnable }}({{ arguments }});"

            return {
                connection.provider: {
                    'body':             chevron.render(template, data),
                    'return_statement': 'return_value'
                }
            }


signal_types = {
    'event': EventSignal(),
    'call':  ServerCallSignal()
}

port_type_data = {
    'Runnable':   {
        'order':          0,
        'consumes':       {
            'call':  'multiple',
            'event': 'multiple'
        },
        'def_attributes': {
            'required': [],
            'optional': {'arguments': {}, 'return_type': 'void'},
            'static':   {}
        },
        'default_impl':   lambda types, runnable_data: {
            'func_name_pattern': '{}_Run_{}',
            'return_type':       runnable_data['return_type'],
            'arguments':         runnable_data['arguments']
        }
    },
    'Event':      {
        'order':          1,
        'provides':       {'event'},
        'def_attributes': {
            'required': [],
            'optional': {'arguments': {}},
            'static':   {'return_type': 'void'}
        },
        'default_impl':   lambda types, port_data: {
            'func_name_pattern': '{}_Call_{}',
            'attributes':        ['weak'],
            'return_type':       port_data['return_type'],
            'arguments':         port_data['arguments']
        }
    },
    'ServerCall': {
        'order':          2,
        'provides':       {'call'},
        'def_attributes': {
            'required': [],
            'optional': {
                'return_type': 'void',
                'arguments':   {}
            },
            'static':   {}
        },
        'default_impl':   lambda types, port_data: {
            'func_name_pattern': '{}_Call_{}',
            'attributes':        ['weak'],
            'return_type':       port_data['return_type'],
            'arguments':         port_data['arguments'],
            'return_value':      port_data.get('value', types.default_value(port_data['return_type']))
        }
    }
}


def create_port_ref(port):
    if type(port) is str:
        parts = port.split('/')
        return {
            'short_name': port,
            'component':  parts[0],
            'port':       parts[1]
        }
    elif type(port) is dict:
        return {
            'short_name': port['short_name'],
            'component':  port['component'],
            'port':       port.get('runnable', port.get('port'))
        }
    else:
        raise TypeError("port must either be a dict or a str")


def expand_runtime_events(owner: Runtime, project_config):
    runtime_config = project_config['runtime']
    events_key = 'runnables'

    runtime_component = {
        'name':         'Runtime',
        'source_files': [],
        'runnables':    {},
        'ports':        {},
        'types':        {}
    }

    event_connections = []
    for event, handlers in runtime_config.get(events_key, {}).items():
        event_port = {
            'port_type': 'Event'
        }
        runtime_component['ports'][event] = event_port
        event_connections.append({
            'provider':  create_port_ref('/'.join(['Runtime', event])),
            'consumers': handlers
        })

    owner.add_component('Runtime', runtime_component)
    runtime_config['port_connections'] += event_connections


def impl_data_lookup(types: TypeCollection, port_data):
    port_type = port_data['port_type']
    if port_type not in port_type_data:
        return None

    return port_type_data[port_type]['default_impl'](types, port_data)


def init(owner: Runtime):
    add_event_to = ['WriteData', 'WriteIndexedData']
    for port_type, known_port_type in owner.port_types.items():
        if port_type in add_event_to:
            known_port_type['provides'].add('event')

    for port_type_name, data in port_type_data.items():
        owner.add_port_type(port_type_name, data, impl_data_lookup)

    for signal_name, signal in signal_types.items():
        owner.add_signal_type(signal_name, signal)


def create_runnable_ports(owner: Runtime, component_name, component_data):
    for runnable_name, runnable_data in component_data['runnables'].items():
        component_data['ports'][runnable_name] = {
            'port_type': 'Runnable',
            **runnable_data
        }


def create_component_runnables(owner: Runtime, component_name, component_data, context):
    for port_name, port_data in component_data['ports'].items():
        port_type = port_data['port_type']
        if port_type in port_type_data:
            function_data = impl_data_lookup(owner.types, port_data)

            function = owner.create_function_for_port(component_name, port_name, function_data)

            if 'weak' not in function_data.get('attributes', []):
                used_arguments = function_data.get('arguments', [])
            else:
                used_arguments = function_data.get('used_arguments', [])

                if function_data['return_type'] != 'void':
                    return_value = function_data.get('return_value', owner.types.default_value(function.return_type))
                    return_value = owner.types.render_value(port_data['return_type'], return_value)
                    function.set_return_statement(return_value)

            for argument in used_arguments:
                function.mark_argument_used(argument)

            for attribute in function_data.get('attributes', []):
                function.add_attribute(attribute)

            function.add_input_assert(function_data.get('asserts', []))
            function.add_body(function_data.get('body', []))

            context['functions']['{}/{}'.format(component_name, port_name)] = function


def add_exported_declarations(owner: Runtime, context):
    runtime_funcs = [short_name for short_name in context['functions'].keys() if short_name.startswith('Runtime/')]
    context['exported_function_declarations'] += runtime_funcs

    sort_functions(owner, context)


def sort_functions(owner: Runtime, context):
    def sort_by_port_type(fn):
        if fn.startswith('Runtime/'):
            weight = 0
        else:

            port = owner.get_port(fn)
            try:
                weight = port_type_data[port['port_type']]['order']
            except KeyError:
                weight = 3

        return weight

    by_port_type = sorted(context['functions'], key=sort_by_port_type)
    context['functions'] = {fn: context['functions'][fn] for fn in by_port_type}


def remove_runtime_component(owner: Runtime, config):
    del owner._components['Runtime']
    port_connections = []
    for connection in config['runtime']['port_connections']:
        provider = connection['provider']
        if type(provider) is str:
            if not provider.startswith('Runtime/'):
                port_connections.append(connection)
        else:
            if provider['component'] != 'Runtime':
                port_connections.append(connection)

    config['runtime']['port_connections'] = port_connections


def cleanup_component(owner: Runtime, component_name, ctx):
    # remove automatically generated runnable ports
    component_data = owner._components[component_name]
    component_data['ports'] = {name: port for name, port in component_data['ports'].items() if
                               name not in component_data['runnables']}

    for port in component_data['ports'].values():
        if port['port_type'] == 'Event':
            try:
                del port['return_type']
            except KeyError:
                pass

            if not port['arguments']:
                del port['arguments']

    sort_functions(owner, ctx)


def runtime_events():
    """Plugin that provides support for simple runtime event creation and configuration"""
    return RuntimePlugin("RuntimeEvents", {
        'init':                        init,
        'load_component_config':       create_runnable_ports,
        'project_config_loaded':       expand_runtime_events,
        'create_component_ports':      create_component_runnables,
        'before_generating_component': cleanup_component,
        'before_generating_runtime':   add_exported_declarations,
        'save_project_config':         remove_runtime_component
    }, requires=['BuiltinDataTypes'])
