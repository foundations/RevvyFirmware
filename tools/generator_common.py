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
            'port_type': port['port_type'],
            'arguments': port.get('arguments', {})
        }
    elif port['port_type'] == 'ReadValue':
        return {
            'port_type': port['port_type'],
            'data_type': port['data_type'],
            'default_value': port.get('default_value', type_default_values[port['data_type']])
        }
    else:
        return port


def process_runnables(runnable_config):
    return {runnable: process_runnable(runnable_config[runnable]) for runnable in runnable_config}


def process_ports(port_config):
    return {port: process_port(port_config[port]) for port in port_config}
