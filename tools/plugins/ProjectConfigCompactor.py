from tools.runtime import RuntimePlugin


def process_runnable_ref_shorthand(runnable):
    """Parse shorthand form of runnable reference into a dictionary"""
    if type(runnable) is str:
        parts = runnable.split('/')
        runnable = {
            'short_name': runnable,
            'component':  parts[0],
            'runnable':   parts[1]
        }
    else:
        runnable['short_name'] = "{}/{}".format(runnable['component'], runnable['port'])

    return runnable


def process_port_ref_shorthand(port):
    """Parse shorthand form of port reference into a dictionary"""
    if type(port) is str:
        parts = port.split('/')
        port = {
            'short_name': port,
            'component':  parts[0],
            'port':       parts[1]
        }
    else:
        port['short_name'] = "{}/{}".format(port['component'], port['port'])

    return port


def expand_port_connection(port_connection):
    connection = {}

    attrs = {k: v for k, v in port_connection.items() if k not in ['provider', 'consumer', 'consumers']}

    if 'consumer' in port_connection:
        connection['consumers'] = [{
            **process_port_ref_shorthand(port_connection['consumer']),
            "attributes": {}
        }]
    else:
        if type(port_connection['consumers']) is list:
            connection['consumers'] = [{
                **process_port_ref_shorthand(consumer),
                "attributes": {}
            } for consumer in port_connection['consumers']]
        else:
            connection['consumers'] = [{
                **process_port_ref_shorthand(consumer),
                "attributes": c_attrs
            } for consumer, c_attrs in port_connection['consumers'].items()]
    connection.update(attrs)

    connection['provider'] = process_port_ref_shorthand(port_connection['provider'])
    return connection


def expand_project_config(owner, project_config):
    """Expand shorthand forms in project configuration"""
    processed_runnables = {}
    for runnable_group in project_config['runtime'].get('runnables', {}):
        processed_runnables[runnable_group] = []
        for runnable in project_config['runtime']['runnables'][runnable_group]:
            processed_runnables[runnable_group].append(process_runnable_ref_shorthand(runnable))

    processed_port_connections = []
    for port_connection in project_config['runtime'].get('port_connections', []):
        processed_port_connections.append(expand_port_connection(port_connection))

    project_config['runtime']['runnables'] = processed_runnables
    project_config['runtime']['port_connections'] = processed_port_connections


def compact_project_config(owner, config):
    """Simplify parts that don't need to remain in their expanded forms"""
    expanded_runtime = config['runtime'].copy()

    compacted_runtime = {
        'runnables':        {},
        'port_connections': []
    }

    def compact_ref(ref):
        if type(ref) is str:
            return ref
        else:
            if not set(ref.keys()).difference(['short_name', 'component', 'port', 'runnable']):
                return ref['short_name']
            else:
                return {key: ref[key] for key in ref if key != 'short_name'}

    for group, runnables in expanded_runtime['runnables'].items():
        compacted_runtime['runnables'][group] = [compact_ref(runnable) for runnable in runnables]

    for connection in expanded_runtime['port_connections']:
        compacted_connection = {key: connection[key] for key in connection if key not in ['provider', 'consumers']}
        compacted_connection['provider'] = compact_ref(connection['provider'])

        consumers = [compact_ref(port) for port in connection['consumers']]
        if len(consumers) == 1:
            compacted_connection['consumer'] = consumers[0]
        else:
            compacted_connection['consumers'] = consumers

        compacted_runtime['port_connections'].append(compacted_connection)

    config['runtime'] = compacted_runtime
    config['types'] = owner.types.export()


def project_config_compactor():
    """Plugin that expands project configuration on load and compacts it on save"""
    handlers = {
        'load_project_config': expand_project_config,
        'save_project_config': compact_project_config
    }
    return RuntimePlugin("ProjectConfigCompactor", handlers)
