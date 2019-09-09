import json

type_includes = {
    'uint8_t':  '<stdint.h>',
    'uint16_t': '<stdint.h>',
    'uint32_t': '<stdint.h>',
    'int8_t':   '<stdint.h>',
    'int16_t':  '<stdint.h>',
    'int32_t':  '<stdint.h>',
    'float':    '<stdint.h>',
    'size_t':   '<stdio.h>',
    'bool':     '<stdbool.h>'
}

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

            processed_port_connections.append({
                'provider':  provider,
                'consumers': [parse_port(consumer) for consumer in consumers]
            })

        project_config['runtime']['runnables'] = processed_runnables
        project_config['runtime']['port_connections'] = processed_port_connections
    return project_config
