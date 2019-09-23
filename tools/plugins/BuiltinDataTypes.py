import chevron

from tools.generator_common import TypeCollection, copy, chevron_list_mark_last, dict_to_chevron_list
from tools.runtime import RuntimePlugin, Runtime, SignalType, SignalConnection


class VariableSignal(SignalType):
    def __init__(self):
        super().__init__(consumers='multiple')

    def create(self, context, connection: SignalConnection):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(connection.provider)
        data_type = provider_port_data['data_type']
        types = runtime.types
        init_value = connection.attributes.get('init_value', types.default_value(data_type))
        ctx = {
            'template': 'static {{ data_type }} {{ signal_name }} = {{ init_value }};',
            'data':     {
                'init_value':  init_value,
                'data_type':   data_type,
                'signal_name': connection.name
            }
        }
        context['declarations'].append(chevron.render(**ctx))

    def generate_provider(self, context, connection: SignalConnection, provider_name):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(provider_name)
        data_type = provider_port_data['data_type']

        if runtime.types.passed_by(data_type) == TypeCollection.PASS_BY_VALUE:
            template = '{{ signal_name }} = {{ value }};'
        else:
            template = '{{ signal_name }} = *{{ value }};'

        function = context['functions'][provider_name]
        argument_names = list(function.arguments.keys())
        data = {
            'signal_name': connection.name,
            'value':       argument_names[0]
        }
        function.add_body(chevron.render(template, data))

    def generate_consumer(self, context, connection: SignalConnection, consumer_name):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(connection.provider)
        expected_data_type = provider_port_data['data_type']

        consumer_port_data = runtime.get_port(consumer_name)
        data_type = consumer_port_data['data_type']

        if data_type != expected_data_type:
            raise Exception('Port data types don\'t match')

        function = context['functions'][consumer_name]
        argument_names = list(function.arguments.keys())
        if runtime.types.passed_by(data_type) == TypeCollection.PASS_BY_VALUE:
            ctx = {
                'template': '{{ signal_name }}',
                'data':     {
                    'signal_name': connection.name
                }
            }
            function.set_return_statement(chevron.render(**ctx))
        else:
            ctx = {
                'template': '*{{ out_name} = {{ signal_name }};',
                'data':     {
                    'signal_name': connection.name,
                    'out_name':    argument_names[0]
                }
            }

            function.add_body(chevron.render(**ctx))


class ArraySignal(SignalType):
    def __init__(self):
        super().__init__(consumers='multiple')

    def create(self, context, connection: SignalConnection):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(connection.provider)
        data_type = provider_port_data['data_type']
        count = provider_port_data['count']
        init_value = connection.attributes.get('init_value',
                                               ', '.join([runtime.types.default_value(data_type)] * count))
        ctx = {
            'template': 'static {{ data_type }} {{ signal_name }}[{{ size }}] = { {{ init_value }} };',
            'data':     {
                'init_value':  init_value,
                'data_type':   data_type,
                'signal_name': connection.name,
                'size':        count
            }
        }
        context['declarations'].append(chevron.render(**ctx))

    def generate_provider(self, context, connection: SignalConnection, provider_name):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(provider_name)
        data_type = provider_port_data['data_type']

        if runtime.types.passed_by(data_type) == TypeCollection.PASS_BY_VALUE:
            template = '{{ signal_name }}[{{ index }}] = {{ value }};'
        else:
            template = '{{ signal_name }}[{{ index }}] = *{{ value }};'

        function = context['functions'][provider_name]
        argument_names = list(function.arguments.keys())
        data = {
            'signal_name': connection.name,
            'index':       argument_names[0],
            'value':       argument_names[1]
        }
        function.add_body(chevron.render(template, data))

    def generate_consumer(self, context, connection: SignalConnection, consumer_name):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(connection.provider)
        expected_data_type = provider_port_data['data_type']

        consumer_port_data = runtime.get_port(consumer_name)
        data_type = consumer_port_data['data_type']

        if data_type != expected_data_type:
            raise Exception('Port data types don\'t match')

        function = context['functions'][consumer_name]
        argument_names = list(function.arguments.keys())
        if runtime.types.passed_by(data_type) == TypeCollection.PASS_BY_VALUE:
            ctx = {
                'template': '{{ signal_name }}[{{ index }}]',
                'data':     {
                    'signal_name': connection.name,
                    'index':       argument_names[0]
                }
            }
            function.set_return_statement(chevron.render(**ctx))
        else:
            ctx = {
                'template': '*{{ out_name} = {{ signal_name }}[{{ index }};',
                'data':     {
                    'signal_name': connection.name,
                    'index':       argument_names[0],
                    'out_name':    argument_names[1]
                }
            }

            function.add_body(chevron.render(**ctx))


class QueueSignal(SignalType):
    def __init__(self):
        super().__init__(consumers='multiple_signals', required_attributes=['queue_length'])

    def create(self, context, connection: SignalConnection):
        if connection.attributes['queue_length'] == 1:
            template = \
                "static {{ data_type }} {{ signal_name }};\n" \
                "static bool {{ signal_name }}_overflow = false;\n" \
                "static bool {{ signal_name }}_data_valid = false;"
        else:
            template = \
                "static {{ data_type }} {{ signal_name }}[{{ queue_length }}u];\n" \
                "static size_t {{ signal_name}}_count = 0u;\n" \
                "static size_t {{ signal_name}}_write_index = 0u;\n" \
                "static bool {{ signal_name }}_overflow = false;"

        runtime = context['runtime']
        provider_port_data = runtime.get_port(connection.provider)
        data_type = provider_port_data['data_type']

        data = {
            'data_type':    data_type,
            'signal_name':  connection.name,
            'queue_length': connection.attributes['queue_length']
        }
        context['declarations'].append(chevron.render(template, data))

    def generate_provider(self, context, connection: SignalConnection, provider_name):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(provider_name)
        data_type = provider_port_data['data_type']

        if connection.attributes['queue_length'] == 1:
            template = \
                "{{ signal_name }}_overflow = {{ signal_name }}_data_valid;\n" \
                "{{ signal_name }} = {{ value }};\n" \
                "{{ signal_name }}_data_valid = true;"
        else:
            template = \
                "if ({{ signal_name }}_count < {{ queue_length }}u)\n" \
                "{\n" \
                "    ++{{ signal_name }}_count;\n" \
                "}\n" \
                "else\n" \
                "{\n" \
                "    {{ signal_name }}_overflow = true;\n" \
                "}\n" \
                "size_t idx = {{ signal_name }}_write_index;\n" \
                "{{ signal_name }}_write_index = ({{ signal_name }}_write_index + 1u) % {{ queue_length }}u;\n" \
                "{{ signal_name }}[idx] = {{ value }};"

        function = context['functions'][provider_name]
        argument_names = list(function.arguments.keys())
        passed_by_value = runtime.types.passed_by(data_type) == TypeCollection.PASS_BY_VALUE
        data = {
            'signal_name': connection.name,
            'value':       argument_names[0] if passed_by_value else '*{}'.format(argument_names[0])
        }
        function.add_body(chevron.render(template, data))

    def generate_consumer(self, context, connection: SignalConnection, consumer_name):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(connection.provider)
        data_type = provider_port_data['data_type']

        if connection.attributes['queue_length'] == 1:
            template = \
                "QueueStatus_t return_value = QueueStatus_Empty;\n" \
                "bool was_overflow = {{ signal_name }}_overflow;\n" \
                "if ({{ signal_name }}_data_valid)\n" \
                "{\n" \
                "    {{ signal_name }}_overflow = false;\n" \
                "    {{ out_name }} = {{ signal_name }};\n" \
                "    {{ signal_name }}_data_valid = false;\n" \
                "    if (was_overflow)\n" \
                "    {\n" \
                "        return_value = QueueStatus_Overflow;\n" \
                "    }\n" \
                "    else\n" \
                "    {\n" \
                "        return_value = QueueStatus_Ok;\n" \
                "    }\n" \
                "}"
        else:
            template = \
                "QueueStatus_t return_value = QueueStatus_Empty;\n" \
                "if ({{ signal_name }}_count > 0u)\n" \
                "{\n" \
                "    size_t idx = ({{ signal_name }}_write_index - {{ signal_name }}_count) % {{ queue_length }}u;\n" \
                "    --{{ signal_name }}_count;\n" \
                "    {{ out_name }} = {{ signal_name }}[idx];\n" \
                "    \n" \
                "    if ({{ signal_name }}_overflow)\n" \
                "    {\n" \
                "        {{ signal_name }}_overflow = false;\n" \
                "        return_value = QueueStatus_Overflow;\n" \
                "    }\n" \
                "    else\n" \
                "    {\n" \
                "        return_value = QueueStatus_Ok;\n" \
                "    }\n" \
                "}"

        function = context['functions'][consumer_name]
        argument_names = list(function.arguments.keys())
        passed_by_value = runtime.types.passed_by(data_type) == TypeCollection.PASS_BY_VALUE
        data = {
            'signal_name': connection.name,
            'out_name':    argument_names[0] if passed_by_value else '*{}'.format(argument_names[0])
        }
        function.add_body(chevron.render(template, data))
        function.set_return_statement('return_value')


class ConstantSignal(SignalType):
    def __init__(self):
        super().__init__(consumers='multiple')

    def create(self, context, connection: SignalConnection):
        pass

    def generate_provider(self, context, connection: SignalConnection, provider_name):
        pass

    def generate_consumer(self, context, connection: SignalConnection, consumer_name):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(connection.provider)
        expected_data_type = provider_port_data['data_type']

        consumer_port_data = runtime.get_port(consumer_name)
        data_type = consumer_port_data['data_type']

        if data_type != expected_data_type:
            raise Exception('Port data types don\'t match')

        function = context['functions'][consumer_name]
        argument_names = list(function.arguments.keys())

        component_name, port_name = provider_port_data['short_name'].split('/')
        constant_provider_name = '{}_Constant_{}'.format(component_name, port_name)
        if runtime.types.passed_by(data_type) == TypeCollection.PASS_BY_VALUE:
            ctx = {
                'template': '{{ constant_provider }}()',
                'data':     {
                    'constant_provider': constant_provider_name
                }
            }
            function.set_return_statement(chevron.render(**ctx))
        else:
            ctx = {
                'template': '{{ constant_provider }}({{ out_name}});',
                'data':     {
                    'constant_provider': constant_provider_name,
                    'out_name':          argument_names[0]
                }
            }

            function.add_body(chevron.render(**ctx))


class ConstantArraySignal(SignalType):
    def __init__(self):
        super().__init__(consumers='multiple')

    def create(self, context, connection: SignalConnection):
        pass

    def generate_provider(self, context, connection: SignalConnection, provider_name):
        pass

    def generate_consumer(self, context, connection: SignalConnection, consumer_name):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(connection.provider)
        expected_data_type = provider_port_data['data_type']

        consumer_port_data = runtime.get_port(consumer_name)
        data_type = consumer_port_data['data_type']

        if data_type != expected_data_type:
            raise Exception('Port data types don\'t match')

        function = context['functions'][consumer_name]
        argument_names = list(function.arguments.keys())

        component_name, port_name = provider_port_data['short_name'].split('/')
        constant_provider_name = '{}_Constant_{}'.format(component_name, port_name)
        if runtime.types.passed_by(data_type) == TypeCollection.PASS_BY_VALUE:
            ctx = {
                'template': '{{ constant_provider }}({{ index }})',
                'data':     {
                    'constant_provider': constant_provider_name,
                    'index':             argument_names[0]
                }
            }
            function.set_return_statement(chevron.render(**ctx))
        else:
            ctx = {
                'template': '{{ constant_provider }}({{ index }}, {{ out_name }});',
                'data':     {
                    'constant_provider': constant_provider_name,
                    'index':             argument_names[0],
                    'out_name':          argument_names[1]
                }
            }

            function.add_body(chevron.render(**ctx))


signal_types = {
    'variable':       VariableSignal(),
    'array':          ArraySignal(),
    'constant':       ConstantSignal(),
    'constant_array': ConstantArraySignal(),
    'queue':          QueueSignal()
}


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
        'template': "\n"
                    "typedef enum {\n"
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
        'template': "\n"
                    "typedef struct {\n"
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


def struct_formatter(types: TypeCollection, type_name, type_data, struct_value):
    if type(struct_value) is str:
        return struct_value

    values = ['.{} = {}'.format(name, types.render_value(type_data['fields'][name], value)) for name, value in
              struct_value.items()]
    return '({}) {{ {} }}'.format(type_name, ', '.join(values))


type_info = {
    TypeCollection.ALIAS:        {
        'typedef_renderer': render_alias_typedef,
        'attributes':       {
            'required': ['aliases'],
            'optional': {'default_value': None, 'pass_semantic': None},
            'static':   {
                'type': TypeCollection.ALIAS
            }
        }
    },

    TypeCollection.EXTERNAL_DEF: {
        'typedef_renderer': None,
        'attributes':       {
            'required': ['defined_in', 'default_value'],
            'optional': {'pass_semantic': TypeCollection.PASS_BY_VALUE},
            'static':   {'type': TypeCollection.EXTERNAL_DEF}
        }
    },

    TypeCollection.ENUM:         {
        'typedef_renderer': render_enum_typedef,
        'attributes':       {
            'required': ['values', 'default_value'],
            'optional': {'pass_semantic': TypeCollection.PASS_BY_VALUE},
            'static':   {
                'type': TypeCollection.ENUM
            }}
    },

    TypeCollection.STRUCT:       {
        'typedef_renderer': render_struct_typedef,
        'value_formatter':  struct_formatter,
        'attributes':       {
            'required': ['fields', 'default_value'],
            'optional': {'pass_semantic': TypeCollection.PASS_BY_POINTER},
            'static':   {
                'type': TypeCollection.STRUCT
            }
        }
    }
}


def process_type_def(type_name, type_def):
    type_data = type_def.copy()
    # determine type of definition
    if 'type' in type_data:
        type_type = type_data['type']
        del type_data['type']
    else:
        if 'defined_in' in type_data:
            type_type = TypeCollection.EXTERNAL_DEF
        elif 'aliases' in type_data:
            type_type = TypeCollection.ALIAS
        elif 'fields' in type_data:
            type_type = TypeCollection.STRUCT
        else:
            raise Exception('Invalid type definition for {}'.format(type_name))

    try:
        attrs = type_info[type_type]['attributes']
        return {
            **attrs['static'],
            **copy(type_data, required=attrs['required'], optional=attrs['optional'])
        }
    except KeyError:
        print('Unknown type {} set for {}'.format(type_type, type_name))
        raise
    except Exception as e:
        raise Exception('Type {} ({}) has unexpected attribute set: {}'.format(type_name, type_type, e))


def init_constant_array(types: TypeCollection, port_data):
    if port_data['count'] != len(port_data['value']):
        raise Exception('Not enough values provided for {}'.format(port_data['short_name']))

    return {
        'func_name_pattern': '{}_Constant_{}'
    }


def get_default_value(types: TypeCollection, port_data):
    """Return the default value for a particular port

    If a return value is not specified (or is None), return the default value for the data type"""
    specified_return_value = port_data['default_value']
    if specified_return_value is None:
        return types.default_value(port_data['data_type'])
    else:
        return specified_return_value


def get_default_value_formatted(types: TypeCollection, port_data):
    default_value = types.default_value(port_data['data_type'])
    return types.render_value(port_data['data_type'], default_value)


port_type_data = {
    'ReadValue':        {
        'order':          3,
        'consumes':       {
            'variable': 'single',
            'constant': 'single'
        },
        'def_attributes': {
            'required': ['data_type'],
            'optional': {'default_value': None},
            'static':   {}
        },
        'default_impl':   {
            'common':  lambda types, port_data: {
                'attributes':        ['weak'],
                'func_name_pattern': '{}_Read_{}',
            },
            'value':   lambda types, port_data: {
                'return_type':  port_data['data_type'],
                'arguments':    {},
                'return_value': get_default_value(types, port_data)
            },
            'pointer': lambda types, port_data: {
                'return_type': 'void',
                'arguments':   {'value': '{}*'.format(port_data['data_type'])},
                'asserts':     'value != NULL',
                'body':        '*value = {};'.format(get_default_value_formatted(types, port_data))
            }
        }
    },
    'ReadQueuedValue':  {
        'order':          3,
        'consumes':       {'queue': 'single'},
        'def_attributes': {
            'required': ['data_type'],
            'optional': {'default_value': None},
            'static':   {}
        },
        'default_impl':   {
            'common':  lambda types, port_data: {
                'attributes':        ['weak'],
                'return_type':       'QueueStatus_t',
                'func_name_pattern': '{}_Read_{}'
            },
            'value':   lambda types, port_data: {
                'arguments': {'value': '{}*'.format(port_data['data_type'])},
            },
            'pointer': lambda types, port_data: {
                'arguments': {'value': '{}*'.format(port_data['data_type'])},
                'asserts':   'value != NULL'
            }
        }
    },
    'ReadIndexedValue': {
        'order':          3,
        'consumes':       {
            'array':          'multiple',
            'constant_array': 'multiple'
        },
        'def_attributes': {
            'required': ['data_type', 'count'],
            'optional': {'default_value': None},
            'static':   {}
        },
        'default_impl':   {
            'common':  lambda types, port_data: {
                'attributes':        ['weak'],
                'func_name_pattern': '{}_Read_{}'
            },
            'value':   lambda types, port_data: {
                'return_type':  port_data['data_type'],
                'arguments':    {'index': 'uint32_t'},
                'return_value': get_default_value(types, port_data),
                'asserts':      'index < {}'.format(port_data['count'])
            },
            'pointer': lambda types, port_data: {
                'return_type': 'void',
                'arguments':   {
                    'index': 'uint32_t',
                    'value': '{}*'.format(port_data['data_type'])
                },
                'asserts':     [
                    'index < {}'.format(port_data['count']),
                    'value != NULL'
                ],
                'body':        '*value = {};'.format(get_default_value_formatted(types, port_data))
            }
        }
    },
    'WriteData':        {
        'order':          2,
        'provides':       {'variable', 'queue'},
        'def_attributes': {
            'required': ['data_type'],
            'optional': {},
            'static':   {}
        },
        'default_impl':   {
            'common':  lambda types, port_data: {
                'attributes':        ['weak'],
                'func_name_pattern': '{}_Write_{}',
                'return_type':       'void'
            },
            'value':   lambda types, port_data: {
                'arguments': {'value': 'const {}'.format(port_data['data_type'])}
            },
            'pointer': lambda types, port_data: {
                'arguments': {'value': 'const {}*'.format(port_data['data_type'])},
                'asserts':   'value != NULL'
            }
        }
    },
    'WriteIndexedData': {
        'order':          2,
        'provides':       {'array'},
        'def_attributes': {
            'required': ['data_type', 'count'],
            'optional': {},
            'static':   {}
        },
        'default_impl':   {
            'common':  lambda types, port_data: {
                'attributes':        ['weak'],
                'func_name_pattern': '{}_Write_{}',
                'return_type':       'void'
            },
            'value':   lambda types, port_data: {
                'arguments': {
                    'index': 'uint32_t',
                    'value': 'const {}'.format(port_data['data_type'])
                },
                'asserts':   'index < {}'.format(port_data['count']),
            },
            'pointer': lambda types, port_data: {
                'arguments': {
                    'index': 'uint32_t',
                    'value': 'const {}*'.format(port_data['data_type'])
                },
                'asserts':   [
                    'index < {}'.format(port_data['count']),
                    'value != NULL'
                ]
            }
        }
    },
    'Constant':         {
        'order':          1,
        'provides':       {'constant'},
        'def_attributes': {
            'required': ['data_type', 'value'],
            'optional': {},
            'static':   {}
        },
        'default_impl':   {
            'common':  lambda types, port_data: {
                'func_name_pattern': '{}_Constant_{}'
            },
            'value':   lambda types, port_data: {
                'return_type':  port_data['data_type'],
                'arguments':    {},
                'return_value': port_data['value']
            },
            'pointer': lambda types, port_data: {
                'return_type': 'void',
                'arguments':   {'value': '{}*'.format(port_data['data_type'])},
                'body':        '*value = {};'.format(types.render_value(port_data['data_type'], port_data['value'])),
                'asserts':     'value != NULL'
            }
        }
    },
    'ConstantArray':    {
        'order':          1,
        'provides':       {'constant_array'},
        'def_attributes': {
            'required': ['data_type', 'value', 'count'],
            'optional': {},
            'static':   {}
        },
        'default_impl':   {
            'common':  init_constant_array,
            'value':   lambda types, port_data: {
                'return_type':  port_data['data_type'],
                'arguments':    {
                    'index': 'uint32_t'
                },
                'body':         'static const {} data[{}] = {{ {} }};'.format(port_data['data_type'],
                                                                              port_data['count'],
                                                                              ', '.join(port_data['value'])),
                'return_value': 'data[index]'
            },
            'pointer': lambda types, port_data: {
                'return_type': 'void',
                'arguments':   {
                    'index': 'uint32_t',
                    'value': '{}*'.format(port_data['data_type'])
                },
                'body':        'static const {} data[{}] = {{ {} }};\n'
                               '*value = data[index];'.format(port_data['data_type'],
                                                              port_data['count'],
                                                              ', '.join(port_data['value'])),
                'asserts':     [
                    'index < {}'.format(port_data['count']),
                    'value != NULL'
                ]
            }
        }
    }
}


def impl_data_lookup(types: TypeCollection, port_data):
    port_type = port_data['port_type']
    if port_type not in port_type_data:
        return None

    implementation_data = port_type_data[port_type]['default_impl']
    pass_by = types.passed_by(port_data['data_type'])
    # noinspection PyCallingNonCallable
    return {
        **implementation_data['common'](types, port_data),
        **implementation_data[pass_by](types, port_data)
    }


def init(owner: Runtime):
    for port_type_name, data in port_type_data.items():
        owner.add_port_type(port_type_name, data, impl_data_lookup)

    for signal_name, signal in signal_types.items():
        owner.add_signal_type(signal_name, signal)


def add_type_def(owner: Runtime, type_name, type_data):
    type_type_data = process_type_def(type_name, type_data)
    type_type_info = type_info[type_type_data['type']]
    value_formatter = type_type_info.get('value_formatter', lambda types, type_name, type_data, x: str(x))
    owner.types.add(type_name, type_type_data, type_type_info['typedef_renderer'], value_formatter)


def process_project_types(owner: Runtime, project_config):
    for type_name, type_data in project_config.get('types', {}).items():
        add_type_def(owner, type_name, type_data)


def process_component_ports_and_types(owner: Runtime, component_name, component_config):
    if 'ports' not in component_config:
        component_config['ports'] = {}

    if not component_config['ports']:
        print('Warning: {} has no ports'.format(component_name))

    for type_name, type_data in component_config.get('types', {}).items():
        add_type_def(owner, type_name, type_data)


def create_component_ports(owner: Runtime, component_name, component_data):
    for port_name, port_data in component_data['ports'].items():
        port_type = port_data['port_type']
        if port_type in port_type_data:
            function_data = impl_data_lookup(owner.types, port_data)
            function = owner.create_function_for_port(component_name, port_name, function_data)

            for attribute in function_data.get('attributes', []):
                function.add_attribute(attribute)

            function.add_body(function_data.get('body', []))
            if function.return_type != 'void':
                return_value = function_data.get('return_value', owner.types.default_value(function.return_type))
                return_value = owner.types.render_value(port_data['data_type'], return_value)
                function.set_return_statement(return_value)

            owner.add_function(port_data['short_name'], function)


def sort_functions(owner: Runtime, context):
    def sort_by_name(fn):
        port = owner.get_port(fn)
        if port['port_type'] in port_type_data:
            return port['short_name']
        else:
            return '0'

    def sort_by_port_type(fn):
        port = owner.get_port(fn)
        try:
            return port_type_data[port['port_type']]['order']
        except KeyError:
            return 0

    by_name = sorted(context['functions'], key=sort_by_name)
    by_port_type = sorted(by_name, key=sort_by_port_type)
    context['functions'] = {fn: context['functions'][fn] for fn in by_port_type}


def cleanup_component(owner: Runtime, component_name, ctx):
    sort_functions(owner, ctx)

    component_ports = owner._components[component_name]['ports']
    for port in component_ports.values():
        try:
            del port['short_name']
        except KeyError:
            pass


def builtin_data_types():
    return RuntimePlugin("BuiltinDataTypes", {
        'init':                        init,
        'load_project_config':         process_project_types,
        'load_component_config':       process_component_ports_and_types,
        'create_component_ports':      create_component_ports,
        'before_generating_component': cleanup_component,
        'before_generating_runtime':   sort_functions
    })
