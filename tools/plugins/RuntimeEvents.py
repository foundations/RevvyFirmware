from tools.generator_common import create_port_ref, create_empty_component_data
from tools.runtime import RuntimePlugin, Runtime


port_type_data = {
    'Event': {
        'def_attributes': {
            'required': [],
            'optional': {'arguments': {}},
            'static': {'return_type': 'void'}
        }
    },
    'ServerCall': {
        'def_attributes': {
            'required': [],
            'optional': {
                'return_type': 'void',
                'arguments': {}
            },
            'static': {}
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
    for port_type_name, data in port_type_data.items():
        owner.add_port_type(port_type_name, data)


def runtime_events():
    """Plugin that provides support for simple runtime event creation and configuration"""
    handlers = {
        'init': init,
        'project_config_loaded': expand_runtime_events
    }
    return RuntimePlugin("RuntimeEvents", handlers, requires=['BuiltinDataTypes'])
