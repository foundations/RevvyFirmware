import chevron

from tools.generator_common import TypeCollection, copy, chevron_list_mark_last, dict_to_chevron_list
from tools.runtime import RuntimePlugin, FunctionDescriptor


def render_alias_typedef(type_collection: TypeCollection, type_name):
    context = {
        'template': "typedef {{ aliased }} {{ type_name }};",

        'data':     {
            'type_name': type_name,
            'aliased':   type_collection.resolve(type_name)
        }
    }

    return chevron.render(**context)


def render_enum_typedef(type_collection: TypeCollection, type_name):
    context = {
        'template': "typedef enum {\n"
                    "    {{# values }}\n"
                    "    {{ value }}{{^ last }},{{/ last }}\n"
                    "    {{/ values }}\n"
                    "} {{ type_name }};",

        'data':     {
            'type_name': type_name,
            'values':    chevron_list_mark_last([{'value': value} for value in type_collection[type_name]['values']])
        }
    }

    return chevron.render(**context)


def render_struct_typedef(type_collection: TypeCollection, type_name):
    context = {
        'template': "typedef struct {\n"
                    "    {{# fields }}\n"
                    "    {{ type }} {{ name }};\n"
                    "    {{/ fields }}\n"
                    "} {{ type_name }};",

        'data':     {
            'type_name': type_name,
            'fields':    dict_to_chevron_list(type_collection[type_name]['fields'], key_name='name', value_name='type')
        }
    }

    return chevron.render(**context)


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

    attributes = {
        TypeCollection.ALIAS:        {
            'required': ['aliases'],
            'optional': {'default_value': None, 'pass_semantic': None},
            'static':   {
                'type':           TypeCollection.ALIAS,
                'render_typedef': render_alias_typedef
            }
        },

        TypeCollection.EXTERNAL_DEF: {
            'required': ['defined_in', 'default_value'],
            'optional': {'pass_semantic': TypeCollection.PASS_BY_VALUE},
            'static':   {'type': TypeCollection.EXTERNAL_DEF}
        },

        TypeCollection.ENUM:         {
            'required': ['values', 'default_value'],
            'optional': {'pass_semantic': TypeCollection.PASS_BY_VALUE},
            'static':   {
                'type':           TypeCollection.ENUM,
                'render_typedef': render_enum_typedef
            }
        },

        TypeCollection.STRUCT:       {
            'required': ['fields', 'default_value'],
            'optional': {'pass_semantic': TypeCollection.PASS_BY_POINTER},
            'static':   {
                'type':           TypeCollection.STRUCT,
                'render_typedef': render_struct_typedef
            }
        }
    }

    try:
        attrs = attributes[type_type]
        return {
            **attrs['static'],
            **copy(type_def, required=attrs['required'], optional=attrs['optional'])
        }
    except KeyError:
        print('Unknown type {} set for {}'.format(type_type, type_name))
        raise
    except Exception as e:
        raise Exception('Type {} ({}) has unexpected attribute set: {}'.format(type_name, type_type, e))


port_type_data = {
    'ReadValue':        {
        'def_attributes': {
            'required': ['data_type'],
            'optional': {'default_value': None},
            'static':   {}
        },
        'default_impl': {
            'data_by_value': {},
            'data_by_pointer': {},
        }
    },
    'ReadQueuedValue':  {
        'def_attributes': {
            'required': ['data_type'],
            'optional': {'default_value': None},
            'static':   {}
        },
        'default_impl': {
            'data_by_value': {},
            'data_by_pointer': {},
        }
    },
    'ReadIndexedValue': {
        'def_attributes': {
            'required': ['data_type', 'count'],
            'optional': {'default_value': None},
            'static':   {}
        },
        'default_impl': {
            'data_by_value': {},
            'data_by_pointer': {},
        }
    },
    'WriteData':        {
        'def_attributes': {
            'required': ['data_type'],
            'optional': {},
            'static':   {}
        },
        'default_impl': {
            'data_by_value': {},
            'data_by_pointer': {},
        }
    },
    'WriteIndexedData': {
        'def_attributes': {
            'required': ['data_type', 'count'],
            'optional': {},
            'static':   {}
        },
        'default_impl': {
            'data_by_value': {},
            'data_by_pointer': {},
        }
    },
    'Constant':         {
        'def_attributes': {
            'required': ['data_type', 'value'],
            'optional': {},
            'static':   {}
        },
        'default_impl': {
            'data_by_value': {},
            'data_by_pointer': {},
        }
    }
}


def init(owner):
    for port_type_name, data in port_type_data.items():
        owner.add_port_type(port_type_name, data)


def process_project_types(owner, project_config):
    defs = project_config.get('types', {})
    project_config['types'] = {type_name: process_type_def(type_name, type_def)
                               for type_name, type_def in defs.items()}


def process_component_ports_and_types(owner, component_name, component_config):
    if 'ports' not in component_config:
        component_config['ports'] = {}
        print('Warning: {} has no ports'.format(component_name))

    defs = component_config.get('types', {})

    component_config['types'] = {type_name: process_type_def(type_name, type_def)
                                 for type_name, type_def in defs.items()}


def create_component_ports(owner, component_name, port_name, port_data):
    port_type = port_data['port_type']
    if port_type in port_type_data:
        owner.add_function(port_type_data[port_type]['create_port_default_impl'](port_name, port_data))


def builtin_data_types():
    return RuntimePlugin("BuiltinDataTypes", {
        'init':                   init,
        'load_project_config':    process_project_types,
        'load_component_config':  process_component_ports_and_types,
        'create_component_ports': create_component_ports
    })
