from tools.generator_common import create_port_ref, create_empty_component_data
from tools.runtime import RuntimePlugin, Runtime


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


def runtime_events():
    """Plugin that provides support for simple runtime event creation and configuration"""
    handlers = {
        'project_config_loaded': expand_runtime_events
    }
    return RuntimePlugin("RuntimeEvents", handlers)
