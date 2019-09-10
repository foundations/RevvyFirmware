import json
import re

type_default_values = {
    'uint8_t':  '0u',
    'uint16_t': '0u',
    'uint32_t': '0u',
    'int8_t':   '0',
    'int16_t':  '0',
    'int32_t':  '0',
    'float':    '0.0f',
    'bool':     'false',
    'void':     None,
    'void*':    'NULL'
}

component_folder_pattern = 'rrrc/components/{}'
component_file_pattern = 'rrrc/components/{}/{}'


def process_runnable(runnable):
    return {
        'arguments':   runnable.get('arguments', {}),
        'return_type': runnable.get('return_type', 'void')
    }


def process_port(port):
    if port['port_type'] == 'Event':
        return {
            'port_type':   port['port_type'],
            'return_type': 'void',
            'arguments':   port.get('arguments', {})
        }
    if port['port_type'] == 'ServerCall':
        return {
            'port_type':   port['port_type'],
            'return_type': port.get('return_type', 'void'),
            'arguments':   port.get('arguments', {})
        }
    elif port['port_type'] == 'ReadValue':
        return {
            'port_type':     port['port_type'],
            'data_type':     port['data_type'],
            'default_value': port.get('default_value', type_default_values[port['data_type']])
        }
    else:
        return port


def process_runnables(runnable_config):
    return {runnable: process_runnable(runnable_config[runnable]) for runnable in runnable_config}


def process_ports(port_config):
    return {port: process_port(port_config[port]) for port in port_config}


def load_component_config(path):
    with open(path, 'r') as component_config_file:
        component_config = json.load(component_config_file)
        component_config['runnables'] = process_runnables(component_config.get('runnables', {}))
        component_config['ports'] = process_ports(component_config.get('ports', {}))
        component_config['types'] = component_config.get('types', {})
    return component_config


def parse_runnable(runnable):
    """Parse shorthand form of runnable reference into a dictionary"""
    if type(runnable) is str:
        parts = runnable.split('/')
        runnable = {
            'component': parts[0],
            'runnable':  parts[1]
        }

    return runnable


def parse_port(port):
    """Parse shorthand form of port reference into a dictionary"""
    if type(port) is str:
        parts = port.split('/')
        port = {
            'component': parts[0],
            'port':      parts[1]
        }

    return port


def load_project_config(project_config_file):
    """Load configuration file and expand shorthand forms for the later stages"""
    with open(project_config_file, "r") as f:
        project_config = json.load(f)

        processed_runnables = {}
        for runnable_group in project_config['runtime'].get('runnables', {}):
            processed_runnables[runnable_group] = []
            for runnable in project_config['runtime']['runnables'][runnable_group]:
                processed_runnables[runnable_group].append(parse_runnable(runnable))

        processed_port_connections = []
        for port_connection in project_config['runtime'].get('port_connections', []):
            provider = parse_port(port_connection['provider'])

            if 'consumers' not in port_connection:
                try:
                    consumers = [port_connection['consumer']]
                except KeyError:
                    consumers = []
            else:
                consumers = port_connection['consumers']

            processed_port_connections.append({
                'provider':  provider,
                'consumers': [parse_port(consumer) for consumer in consumers]
            })

        project_config['types'] = project_config.get('types', {})
        project_config['runtime']['runnables'] = processed_runnables
        project_config['runtime']['port_connections'] = processed_port_connections

    return project_config


def compact_project_config(config):
    """Simplify parts that don't need to remain in their expanded forms"""
    compacted = {
        'sources':    config['sources'],
        'includes':   config['includes'],
        'components': config['components'],
        'types':      config['types'],
        'runtime':    {
            'runnables':        {},
            'port_connections': []
        }
    }

    def compact_port_ref(port):
        if type(port) is str:
            return port
        else:
            return "{}/{}".format(port['component'], port['port'])

    def compact_runnable_ref(runnable):
        if type(runnable) is str:
            return runnable
        else:
            return "{}/{}".format(runnable['component'], runnable['runnable'])

    for group in config['runtime']['runnables']:
        compacted['runtime']['runnables'][group] = []

        for runnable in config['runtime']['runnables'][group]:
            compacted_runnable = compact_runnable_ref(runnable)
            compacted['runtime']['runnables'][group].append(compacted_runnable)

    for port_connection in config['runtime']['port_connections']:
        compacted_port_connection = {
            'provider': compact_port_ref(port_connection['provider'])
        }

        consumers = []
        for consumer in port_connection['consumers']:
            consumers.append(compact_port_ref(consumer))

        if len(consumers) == 1:
            compacted_port_connection['consumer'] = consumers[0]
        else:
            compacted_port_connection['consumers'] = consumers

        compacted['runtime']['port_connections'].append(compacted_port_connection)

    return compacted


def resolve_type(type_name, type_data, resolved_types, past=None):
    if type_name not in type_data:
        raise Exception('Incomplete type: {}'.format(type_name))

    if type_name in resolved_types:  # cache
        return resolved_types[type_name]

    # record visited types to detect circular definition
    if past is None:
        past = []
    elif type_name in past:
        raise Exception('Circular type definition for {}'.format(type_name))

    if 'aliases' in type_data[type_name]:
        past.append(type_name)
        resolved = resolve_type(type_data[type_name]['aliases'], type_data, resolved_types, past)

        resolved_types[type_name] = resolved

        return resolved
    elif 'defined_in' in type_data[type_name]:
        resolved_types[type_name] = type_name
        return type_name


def add_data_type(type_name, info, type_data, resolved_types):
    if type_name in type_data:
        # type already exists, check if they are the same
        resolved_known = resolve_type(type_name, type_data, resolved_types)

        if 'aliases' in info:
            resolved_new = resolve_type(info['aliases'], type_data, resolved_types)
            if resolved_known != resolved_new:
                raise Exception('Type {} is already defined'.format(type_name))
        elif 'defined_in' in info:
            if info['defined_in'] != type_data[resolved_known]['defined_in']:
                raise Exception('Type {} can\'t override a type from a different source'.format(type_name))
        else:
            raise Exception('Invalid type definition {}'.format(type_name))

    else:
        type_data[type_name] = info


def collect_type_aliases(types, type_data, resolved_types):
    aliases = []
    for type_name in types:
        resolved_type = resolve_type(type_name, type_data, resolved_types)
        if resolved_type != type_name:
            aliases.append({
                'type':    type_name,
                'aliased': resolved_type
            })
        else:
            aliases.append({
                'type':       type_name,
                'defined_in': type_data[resolved_type]['defined_in']
            })

    return aliases


def to_underscore(name):
    """
    >>> to_underscore('ADC')
    'adc'
    >>> to_underscore('FizzBuzz')
    'fizz_buzz'
    """
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()
