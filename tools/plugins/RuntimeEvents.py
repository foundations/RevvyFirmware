from tools.generator_common import create_port_ref, create_empty_component_data
from tools.runtime import RuntimePlugin, Runtime, FunctionDescriptor, SignalType, SignalConnection


class EventSignal(SignalType):
    def __init__(self):
        super().__init__(consumers='multiple')

    def create(self, context, connection: SignalConnection):
        pass

    def generate_provider(self, context, connection: SignalConnection, provider_name):
        pass

    def generate_consumer(self, context, connection: SignalConnection, consumer_name):
        pass


class ServerCallSignal(SignalType):
    def __init__(self):
        super().__init__(consumers='single')

    def create(self, context, connection: SignalConnection):
        pass

    def generate_provider(self, context, connection: SignalConnection, provider_name):
        pass

    def generate_consumer(self, context, connection: SignalConnection, consumer_name):
        pass


signal_types = {
    'event': EventSignal(),
    'call':  ServerCallSignal()
}

port_type_data = {
    'Runnable':   {
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
            'return_type':       'void',
            'arguments':         runnable_data['arguments']
        }
    },
    'Event':      {
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


def expand_runtime_events(owner: Runtime, project_config):
    runtime_config = project_config['runtime']
    events_key = 'runnables'

    runtime_component = create_empty_component_data('Runtime')

    event_connections = []
    for event, handlers in runtime_config.get(events_key, {}).items():
        event_port = {
            'port_type': 'Event'
        }
        runtime_component['ports'][event] = event_port
        event_connections.append({
            'provider':  create_port_ref('/'.join(['Runtime', event])),
            'consumers': [create_port_ref(runnable_ref) for runnable_ref in handlers]
        })

    owner.add_component('Runtime', runtime_component)
    runtime_config['port_connections'] += event_connections


def impl_data_lookup(types, port_data):
    port_type = port_data['port_type']
    if port_type not in port_type_data:
        return None

    return port_type_data[port_type]['default_impl'](types, port_data)


def init(owner):
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


def create_component_runnables(owner: Runtime, component_name, component_data):
    for port_name, port_data in component_data['ports'].items():
        port_type = port_data['port_type']
        if port_type in port_type_data:
            function_data = impl_data_lookup(owner.types, port_data)

            function = owner.create_function_for_port(component_name, port_name, function_data)

            function.add_input_assert(function_data.get('asserts', []))
            function.add_body(function_data.get('body', []))
            function.set_return_statement(function_data.get('return_value'))

            owner.add_function(port_data['short_name'], function)


def runtime_events():
    """Plugin that provides support for simple runtime event creation and configuration"""
    return RuntimePlugin("RuntimeEvents", {
        'init':                   init,
        'load_component_config':  create_runnable_ports,
        'project_config_loaded':  expand_runtime_events,
        'create_component_ports': create_component_runnables
    }, requires=['BuiltinDataTypes'])
