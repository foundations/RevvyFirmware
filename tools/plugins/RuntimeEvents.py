from tools.generator_common import create_port_ref, create_empty_component_data
from tools.runtime import RuntimePlugin, Runtime, FunctionDescriptor

signal_types = {
    'event': {
        'consumers': 'multiple'
    },
    'call': {
        'consumers': 'single'
    }
}

port_type_data = {
    'Runnable':   {
        'consumers':      ['call', 'event'],
        'def_attributes': {
            'required': [],
            'optional': {'arguments': {}},
            'static':   {'return_type': 'void'}
        },
        'default_impl':   lambda types, runnable_data: {
            'func_name_pattern': '{}_Run_{}',
            'return_type':       'void',
            'arguments':         runnable_data['arguments']
        }
    },
    'Event':      {
        'provides':       ['event'],
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
        'provides':       ['call'],
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
            'component': 'Runtime',
            'port':      event,
            'port_type': 'Event'
        }
        runtime_component['ports'][event] = event_port
        event_connections.append({
            'provider':  create_port_ref('/'.join(['Runtime', event])),
            'consumers': [create_port_ref(runnable_ref) for runnable_ref in handlers]
        })

    owner._components['Runtime'] = runtime_component
    runtime_config['port_connections'] += event_connections


def init(owner):
    add_event_to = ['WriteData', 'WriteIndexedData']
    for port_type, known_port_type in owner.port_types.items():
        if port_type in add_event_to:
            known_port_type['provides'].append('event')

    for port_type_name, data in port_type_data.items():
        owner.add_port_type(port_type_name, data)


def create_component_runnables(owner: Runtime, component_name, component_data):
    for runnable_name, runnable_data in component_data['runnables'].items():
        implementation_data = port_type_data['Runnable']['default_impl']
        function_data = implementation_data(owner.types, runnable_data)

        fn_name = function_data['func_name_pattern'].format(component_name, runnable_name)
        function = FunctionDescriptor.create(fn_name, function_data)

        owner.add_function(runnable_data['short_name'], function)

    for port_name, port_data in component_data['ports'].items():
        port_type = port_data['port_type']
        if port_type in port_type_data:
            implementation_data = port_type_data[port_type]['default_impl']
            function_data = implementation_data(owner.types, port_data)

            fn_name = function_data['func_name_pattern'].format(component_name, port_name)
            function = FunctionDescriptor.create(fn_name, function_data)

            function.add_input_assert(function_data.get('asserts', []))
            function.add_body(function_data.get('body', []))
            function.set_return_statement(function_data.get('return_value'))

            owner.add_function(port_data['short_name'], function)


def runtime_events():
    """Plugin that provides support for simple runtime event creation and configuration"""
    return RuntimePlugin("RuntimeEvents", {
        'init':                   init,
        'project_config_loaded':  expand_runtime_events,
        'create_component_ports': create_component_runnables
    }, requires=['BuiltinDataTypes'])
