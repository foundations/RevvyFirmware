from tools.generator_common import TypeCollection
from tools.runtime import RuntimePlugin


def copy(src, required, optional):
    """This function makes sure src contains required and optional keys and nothing else"""

    dst = {**optional}
    required_keys_found = 0
    for key, value in src.items():
        if key in required:
            required_keys_found += 1
            dst[key] = value
        elif key in optional:
            dst[key] = value
        else:
            raise Exception(key)

    return dst


def process_type_def(type_name, type_def):
    # determine type of definition
    if 'type' in type_def:
        type_type = type_def['type']
        del type_def['type']
    else:
        if 'defined_in' in type_def:
            type_type = TypeCollection.EXTERNAL_DEF
        elif 'aliases' in type_def:
            type_type = TypeCollection.ALIAS
        else:
            raise Exception('Invalid type definition for {}'.format(type_name))

    required = {
        TypeCollection.ALIAS: ['aliases'],
        TypeCollection.EXTERNAL_DEF: ['defined_in', 'default_value'],
        TypeCollection.ENUM: ['values', 'default_value'],
        TypeCollection.STRUCT: ['fields', 'default_value']
    }
    optional = {
        TypeCollection.ALIAS: {'default_value': None, 'pass_semantic': None},
        TypeCollection.EXTERNAL_DEF: {'pass_semantic': TypeCollection.PASS_BY_VALUE},
        TypeCollection.ENUM: {'pass_semantic': TypeCollection.PASS_BY_VALUE},
        TypeCollection.STRUCT: {'pass_semantic': TypeCollection.PASS_BY_POINTER}
    }
    constants = {
        TypeCollection.ALIAS: {'type': TypeCollection.ALIAS},
        TypeCollection.EXTERNAL_DEF: {'type': TypeCollection.EXTERNAL_DEF},
        TypeCollection.ENUM: {'type': TypeCollection.ENUM},
        TypeCollection.STRUCT: {'type': TypeCollection.STRUCT}
    }

    try:
        return {
            **constants[type_type],
            **copy(type_def, required=required[type_type], optional=optional[type_type])
        }
    except KeyError:
        print('Unknown type {} set for {}'.format(type_type, type_name))
        raise
    except Exception as e:
        raise Exception('Type {} ({}) has unexpected attribute set: {}'.format(type_name, type_type, e))


def process_port_def(component_name, port_name, port):
    short_name = '{}/{}'.format(component_name, port_name)
    port_type = port['port_type']
    del port['port_type']

    required_attributes = {
        "Event":            [],
        "ServerCall":       [],
        "ReadValue":        ['data_type'],
        "ReadQueuedValue":  ['data_type'],
        "ReadIndexedValue": ['data_type', 'count'],
        "WriteData":        ['data_type'],
        "WriteIndexedData": ['data_type', 'count'],
        "Constant":         ['data_type', 'value']
    }
    optional_attributes = {
        "Event":            {'arguments': {}},
        "ServerCall":       {'arguments': {}, 'return_type': 'void'},
        "ReadValue":        {'default_value': None},
        "ReadQueuedValue":  {'default_value': None},
        "ReadIndexedValue": {'default_value': None},
        "WriteData":        {},
        "WriteIndexedData": {},
        "Constant":         {}
    }
    constant_attributes = {
        "Event":            {'return_type': 'void'},
        "ServerCall":       {},
        "ReadValue":        {},
        "ReadQueuedValue":  {},
        "ReadIndexedValue": {},
        "WriteData":        {},
        "WriteIndexedData": {},
        "Constant":         {}
    }

    try:
        required = required_attributes[port_type]
        optional = optional_attributes[port_type]

        processed = {
            'short_name': short_name,
            'port_type':  port_type,
            **constant_attributes[port_type],
            **copy(port, required, optional)
        }
    except KeyError:
        print('Unknown port type {} found in port {}'.format(port_type, short_name))
        raise
    except Exception as e:
        raise Exception('Port {} ({}) has unexpected attribute set: {}'.format(short_name, port_type, e))

    return processed


def process_project_types(owner, project_config):
    defs = project_config.get('types', {})
    project_config['types'] = {type_name: process_type_def(type_name, type_def)
                               for type_name, type_def in defs.items()}


def process_component_ports_and_types(owner, component_name, component_config):
    try:
        ports = component_config['ports']
    except KeyError:
        print('Warning: {} has no ports'.format(component_name))
        ports = {}

    defs = component_config.get('types', {})

    component_config['types'] = {type_name: process_type_def(type_name, type_def)
                                 for type_name, type_def in defs.items()}
    component_config['ports'] = {port_name: process_port_def(component_name, port_name, config)
                                 for port_name, config in ports.items()}


def builtin_types():
    return RuntimePlugin("BuiltinTypes", {
        'load_project_config': process_project_types,
        'load_component_config': process_component_ports_and_types
    })
