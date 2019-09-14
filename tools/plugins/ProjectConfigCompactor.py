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


def process_type_def_shorthand(type_def):
    processed_type_def = {}

    if 'defined_in' in type_def:
        processed_type_def['type'] = 'external_type_def'
        processed_type_def['defined_in'] = type_def['defined_in']
        processed_type_def['default_value'] = type_def['default_value']
        processed_type_def['pass_semantic'] = type_def.get('pass_semantic', 'value')

    elif 'aliases' in type_def:
        processed_type_def['type'] = 'type_alias'
        processed_type_def['aliases'] = type_def['aliases']

    else:
        processed_type_def['type'] = type_def['type']
        if type_def['type'] == 'enum':
            processed_type_def['values'] = type_def['values']
            processed_type_def['default_value'] = type_def['default_value']
            processed_type_def['pass_semantic'] = type_def.get('pass_semantic', 'value')

        elif type_def['type'] == 'struct':
            processed_type_def['fields'] = type_def['fields']
            processed_type_def['default_values'] = type_def.get('default_values', {})
            processed_type_def['pass_semantic'] = type_def.get('pass_semantic', 'pointer')

        else:
            raise Exception('Unsupported type {}'.format(type_def['type']))

    return processed_type_def


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

    defs = project_config.get('types', {})
    project_config['types'] = {type_name: process_type_def_shorthand(defs[type_name]) for type_name in defs}
    project_config['runtime']['runnables'] = processed_runnables
    project_config['runtime']['port_connections'] = processed_port_connections


def expand_port_connection(port_connection):
    def move_single(dictionary, single, multiple):
        if multiple not in dictionary:
            try:
                dictionary[multiple] = [dictionary[single]]
                del dictionary[single]
            except KeyError:
                dictionary[multiple] = []

    connection = dict(port_connection)

    move_single(connection, 'provider', 'providers')
    move_single(connection, 'consumer', 'consumers')

    connection['providers'] = [process_port_ref_shorthand(provider) for provider in connection['providers']]
    connection['consumers'] = [process_port_ref_shorthand(consumer) for consumer in connection['consumers']]
    return connection


def project_config_compactor():
    """Plugin that expands project configuration on load and compacts it on save"""
    handlers = {
        'load_project_config': expand_project_config
    }
    return RuntimePlugin("ProjectConfigCompactor", handlers)
