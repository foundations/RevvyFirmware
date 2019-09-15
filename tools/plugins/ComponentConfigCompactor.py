from tools.runtime import RuntimePlugin


def process_runnable_def(component, runnable_name, runnable):
    short_name = '{}/{}'.format(component, runnable_name)

    try:
        args = runnable['arguments']
        if type(args) is list:
            print('Warning: converting argument list to dict in {}'.format(short_name))
            args = {}
    except KeyError:
        args = {}

    return {
        'short_name':  short_name,
        'arguments':   args,
        'return_type': runnable.get('return_type', 'void')
    }


def process_port_def(component, port_name, port):
    short_name = '{}/{}'.format(component, port_name)
    if port['port_type'] == 'Event':
        return {
            'short_name':  short_name,
            'port_type':   port['port_type'],
            'return_type': 'void',
            'arguments':   port.get('arguments', {})
        }

    if port['port_type'] == 'ServerCall':
        return {
            'short_name':  short_name,
            'port_type':   port['port_type'],
            'return_type': port.get('return_type', 'void'),
            'arguments':   port.get('arguments', {})
        }

    elif port['port_type'] == 'ReadValue':
        return {
            'short_name':    short_name,
            'port_type':     port['port_type'],
            'data_type':     port['data_type'],
            'default_value': port.get('default_value', None)
        }

    else:
        port['short_name'] = short_name
        return port


def process_type_def(type_def):
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


def process_runnable_defs(component, runnable_config):
    return {runnable: process_runnable_def(component, runnable, runnable_config[runnable]) for runnable in
            runnable_config}


def process_port_defs(component, port_config):
    return {port: process_port_def(component, port, port_config[port]) for port in port_config}


def process_type_defs(type_defs):
    return {type_name: process_type_def(type_defs[type_name]) for type_name in type_defs}


def expand_component_config(owner, component_name, component_config):
    """Expand shorthand forms in component configuration"""
    component_config['runnables'] = process_runnable_defs(component_name, component_config.get('runnables', {}))
    component_config['ports'] = process_port_defs(component_name, component_config.get('ports', {}))
    component_config['types'] = process_type_defs(component_config.get('types', {}))


def component_config_compactor():
    """Plugin that expands component configuration on load and compacts it on save"""
    handlers = {
        'load_component_config': expand_component_config
    }
    return RuntimePlugin("ComponentConfigCompactor", handlers)
