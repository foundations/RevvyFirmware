import chevron

from tools.generator_common import TypeCollection, copy, chevron_list_mark_last, dict_to_chevron_list
from tools.runtime import RuntimePlugin, Runtime, SignalType, SignalConnection


def lookup_member(types: TypeCollection, data_type, member_list):
    if not member_list:
        return data_type

    type_data = types.get(data_type)

    if type_data['type'] == TypeCollection.STRUCT:
        return lookup_member(types, type_data['fields'][member_list[0]], member_list[1:])
    elif type_data['type'] == TypeCollection.UNION:
        return lookup_member(types, type_data['members'][member_list[0]], member_list[1:])
    else:
        raise Exception('Trying to access member of non-struct type {}'.format(data_type))


def create_member_accessor(member):
    return '.' + member


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
                'init_value':  types.render_value(data_type, init_value, 'initialization'),
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
        return {
            provider_name: {
                'used_arguments': argument_names,
                'body':           chevron.render(template, {
                    'signal_name': connection.name,
                    'value':       argument_names[0]
                })
            }
        }

    def generate_consumer(self, context, connection: SignalConnection, consumer_name, attributes):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(connection.provider)
        source_data_type = provider_port_data['data_type']

        consumer_port_data = runtime.get_port(consumer_name)
        data_type = consumer_port_data['data_type']

        if 'member' in attributes:
            member_list = attributes['member'].split('.')
            source_data_type = lookup_member(runtime.types, source_data_type, member_list)
            member_accessor = create_member_accessor(attributes['member'])
        else:
            member_accessor = ''

        if data_type != source_data_type:
            raise Exception(
                'Port data types don\'t match (Provider: {} Consumer: {})'.format(source_data_type, data_type))

        function = context['functions'][consumer_name]
        argument_names = list(function.arguments.keys())
        mods = {
            consumer_name: {}
        }
        if runtime.types.passed_by(data_type) == TypeCollection.PASS_BY_VALUE:
            ctx = {
                'template': '{{ data_type}} return_value = {{ signal_name }}{{ member_accessor }};',
                'data':     {
                    'data_type':       data_type,
                    'signal_name':     connection.name,
                    'member_accessor': member_accessor
                }
            }
            mods[consumer_name]['body'] = chevron.render(**ctx)
            mods[consumer_name]['return_statement'] = 'return_value'
        else:
            ctx = {
                'template': '*{{ out_name }} = {{ signal_name }}{{ member_accessor }};',
                'data':     {
                    'signal_name':     connection.name,
                    'out_name':        argument_names[0],
                    'member_accessor': member_accessor
                }
            }

            mods[consumer_name]['used_arguments'] = [argument_names[0]]
            mods[consumer_name]['body'] = chevron.render(**ctx)

        return mods


class ArraySignal(SignalType):
    def __init__(self):
        super().__init__(consumers='multiple')

    def create(self, context, connection: SignalConnection):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(connection.provider)
        data_type = provider_port_data['data_type']
        count = provider_port_data['count']

        try:
            # either all init values are specified
            init_values = connection.attributes['init_values']
        except KeyError:
            # ... or a single one is
            default_value = runtime.types.default_value(data_type)
            init_value = connection.attributes.get('init_value', default_value)
            init_values = [runtime.types.render_value(data_type, init_value, 'initialization')] * count

        if type(init_values) is list:
            if len(init_values) != count:
                raise Exception('Array initializer count ({}) does not match size ({}) - signal provided by {}'
                                .format(len(init_values), count, connection.provider))

            init_values = ', '.join(init_values)

        ctx = {
            'template': 'static {{ data_type }} {{ signal_name }}[{{ size }}] = { {{ init_value }} };',
            'data':     {
                'init_value':  init_values,
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
        return {
            provider_name: {
                'used_arguments': [argument_names[0], argument_names[1]],
                'body':           chevron.render(template, {
                    'signal_name': connection.name,
                    'index':       argument_names[0],
                    'value':       argument_names[1]
                })
            }
        }

    def generate_consumer(self, context, connection: SignalConnection, consumer_name, attributes):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(connection.provider)
        source_data_type = provider_port_data['data_type']

        consumer_port_data = runtime.get_port(consumer_name)
        data_type = consumer_port_data['data_type']

        if 'member' in attributes:
            member_list = attributes['member'].split('.')
            source_data_type = lookup_member(runtime.types, source_data_type, member_list)
            member_accessor = create_member_accessor(attributes['member'])
        else:
            member_accessor = ''

        if data_type != source_data_type:
            raise Exception('Port data types don\'t match')

        function = context['functions'][consumer_name]
        argument_names = list(function.arguments.keys())

        mods = {
            consumer_name: {
                'used_arguments': []
            }
        }
        if runtime.types.passed_by(data_type) == TypeCollection.PASS_BY_VALUE:

            if 'count' not in consumer_port_data:
                # single read, index should be next to consumer name
                index = attributes['index']
            else:
                if consumer_port_data['count'] > provider_port_data['count']:
                    raise Exception('{} signal count ({}) is incompatible with {} ({})'
                                    .format(consumer_name, consumer_port_data['count'],
                                            connection.provider, provider_port_data['count']))
                index = argument_names[0]
                mods[consumer_name]['used_arguments'] = [argument_names[0]]

            ctx = {
                'template': '{{ data_type}} return_value = {{ signal_name }}[{{ index }}]{{ member_accessor }};',
                'data':     {
                    'data_type':       data_type,
                    'signal_name':     connection.name,
                    'index':           index,
                    'member_accessor': member_accessor
                }
            }
            mods[consumer_name]['body'] = chevron.render(**ctx)
            mods[consumer_name]['return_statement'] = 'return_value'
        else:

            if 'count' not in consumer_port_data:
                # single read, index should be next to consumer name
                index = connection.attributes['index']
                out_name = argument_names[0]
            else:
                if consumer_port_data['count'] > provider_port_data['count']:
                    raise Exception('{} signal count ({}) is incompatible with {} ({})'
                                    .format(consumer_name, consumer_port_data['count'],
                                            connection.provider, provider_port_data['count']))
                index = argument_names[0]
                out_name = argument_names[1]
                mods[consumer_name]['used_arguments'] = [argument_names[0], argument_names[1]]

            ctx = {
                'template': '*{{ out_name }} = {{ signal_name }}[{{ index }}]{{ member_accessor }};',
                'data':     {
                    'signal_name':     connection.name,
                    'index':           index,
                    'out_name':        out_name,
                    'member_accessor': member_accessor
                }
            }
            mods[consumer_name]['body'] = chevron.render(**ctx)

        return mods


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
        return {
            connection.provider: {
                'used_arguments': [argument_names[0]],
                'body':           chevron.render(template, {
                    'queue_length': connection.attributes['queue_length'],
                    'signal_name':  connection.name,
                    'value':        argument_names[0] if passed_by_value else '*{}'.format(argument_names[0])
                })
            }
        }

    def generate_consumer(self, context, connection: SignalConnection, consumer_name, attributes):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(connection.provider)
        source_data_type = provider_port_data['data_type']

        if 'member' in attributes:
            member_list = attributes['member'].split('.')
            source_data_type = lookup_member(runtime.types, source_data_type, member_list)
            member_accessor = create_member_accessor(attributes['member'])
        else:
            member_accessor = ''

        if connection.attributes['queue_length'] == 1:
            template = \
                "QueueStatus_t return_value = QueueStatus_Empty;\n" \
                "bool was_overflow = {{ signal_name }}_overflow;\n" \
                "if ({{ signal_name }}_data_valid)\n" \
                "{\n" \
                "    {{ signal_name }}_overflow = false;\n" \
                "    {{ out_name }} = {{ signal_name }}{{ member_accessor }};\n" \
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
                "    {{ out_name }} = {{ signal_name }}[idx]{{ member_accessor }};\n" \
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
        passed_by_value = runtime.types.passed_by(source_data_type) == TypeCollection.PASS_BY_VALUE
        data = {
            'queue_length':    connection.attributes['queue_length'],
            'signal_name':     connection.name,
            'out_name':        argument_names[0] if passed_by_value else '*{}'.format(argument_names[0]),
            'member_accessor': member_accessor
        }

        return {
            consumer_name: {
                'used_arguments':   [argument_names[0]],
                'body':             chevron.render(template, data),
                'return_statement': 'return_value'
            }
        }


class ConstantSignal(SignalType):
    def __init__(self):
        super().__init__(consumers='multiple')

    def create(self, context, connection: SignalConnection):
        pass

    def generate_provider(self, context, connection: SignalConnection, provider_name):
        pass

    def generate_consumer(self, context, connection: SignalConnection, consumer_name, attributes):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(connection.provider)
        source_data_type = provider_port_data['data_type']

        consumer_port_data = runtime.get_port(consumer_name)
        data_type = consumer_port_data['data_type']

        if 'member' in attributes:
            member_list = attributes['member'].split('.')
            source_data_type = lookup_member(runtime.types, source_data_type, member_list)
            member_accessor = create_member_accessor(attributes['member'])
        else:
            member_accessor = ''

        if data_type != source_data_type:
            raise Exception('Port data types don\'t match')

        function = context['functions'][consumer_name]
        argument_names = list(function.arguments.keys())

        component_name, port_name = connection.provider.split('/')
        constant_provider_name = '{}_Constant_{}'.format(component_name, port_name)
        mods = {
            consumer_name: {}
        }

        if runtime.types.passed_by(data_type) == TypeCollection.PASS_BY_VALUE:
            ctx = {
                'template': '{{ constant_provider }}(){{ member_accessor }}',
                'data':     {
                    'constant_provider': constant_provider_name,
                    'member_accessor':   member_accessor
                }
            }
            mods[consumer_name]['return_statement'] = chevron.render(**ctx)
        else:
            if member_accessor:
                ctx = {
                    'template': '{{ data_type }} tmp;\n'
                                '{{ constant_provider }}(&tmp);\n'
                                '{{ out_name }} = tmp{{ member_accessor }};',
                    'data':     {
                        'constant_provider': constant_provider_name,
                        'out_name':          argument_names[0],
                        'member_accessor':   member_accessor,
                        'data_type':         provider_port_data['data_type']
                    }
                }
            else:
                ctx = {
                    'template': '{{ constant_provider }}({{ out_name }});',
                    'data':     {
                        'constant_provider': constant_provider_name,
                        'out_name':          argument_names[0]
                    }
                }

            mods[consumer_name]['used_arguments'] = [argument_names[0]]
            mods[consumer_name]['body'] = chevron.render(**ctx)

        return mods


class ConstantArraySignal(SignalType):
    def __init__(self):
        super().__init__(consumers='multiple')

    def create(self, context, connection: SignalConnection):
        pass

    def generate_provider(self, context, connection: SignalConnection, provider_name):
        pass

    def generate_consumer(self, context, connection: SignalConnection, consumer_name, attributes):
        runtime = context['runtime']
        provider_port_data = runtime.get_port(connection.provider)
        source_data_type = provider_port_data['data_type']

        consumer_port_data = runtime.get_port(consumer_name)
        data_type = consumer_port_data['data_type']

        if 'member' in attributes:
            member_list = attributes['member'].split('.')
            source_data_type = lookup_member(runtime.types, source_data_type, member_list)
            member_accessor = create_member_accessor(attributes['member'])
        else:
            member_accessor = ''

        if data_type != source_data_type:
            raise Exception('Port data types don\'t match')

        function = context['functions'][consumer_name]
        argument_names = list(function.arguments.keys())

        component_name, port_name = provider_port_data['short_name'].split('/')
        constant_provider_name = '{}_Constant_{}'.format(component_name, port_name)
        mods = {
            consumer_name: {}
        }

        if runtime.types.passed_by(data_type) == TypeCollection.PASS_BY_VALUE:

            if 'count' not in consumer_port_data:
                # single read, index should be next to consumer name
                index = attributes['index']
            else:
                if consumer_port_data['count'] > provider_port_data['count']:
                    raise Exception('{} signal count ({}) is incompatible with {} ({})'
                                    .format(consumer_name, consumer_port_data['count'],
                                            connection.provider, provider_port_data['count']))
                index = argument_names[0]
                mods[consumer_name]['used_arguments'] = [argument_names[0]]

            ctx = {
                'template': '{{ data_type}} return_value = {{ constant_provider }}({{ index }}){{ member_accessor }};',
                'data':     {
                    'data_type':         data_type,
                    'constant_provider': constant_provider_name,
                    'index':             index,
                    'member_accessor':   member_accessor
                }
            }
            mods[consumer_name]['body'] = chevron.render(**ctx)
            mods[consumer_name]['return_statement'] = 'return_value'
        else:

            if 'count' not in consumer_port_data:
                # single read, index should be next to consumer name
                index = connection.attributes['index']
                out_name = argument_names[0]
            else:
                if consumer_port_data['count'] > provider_port_data['count']:
                    raise Exception('{} signal count ({}) is incompatible with {} ({})'
                                    .format(consumer_name, consumer_port_data['count'],
                                            connection.provider, provider_port_data['count']))
                index = argument_names[0]
                out_name = argument_names[1]
                mods[consumer_name]['used_arguments'] = [argument_names[0], argument_names[1]]

            if member_accessor:
                ctx = {
                    'template': '{{ data_type }} tmp;\n'
                                '{{ constant_provider }}({{ index }}, &tmp);\n'
                                '{{ out_name }} = tmp{{ member_accessor }};',
                    'data':     {
                        'constant_provider': constant_provider_name,
                        'index':             index,
                        'out_name':          out_name,
                        'member_accessor':   member_accessor,
                        'data_type':         provider_port_data['data_type']
                    }
                }
            else:
                ctx = {
                    'template': '{{ constant_provider }}({{ index }}, {{ out_name }});',
                    'data':     {
                        'constant_provider': constant_provider_name,
                        'index':             index,
                        'out_name':          out_name
                    }
                }
            mods[consumer_name]['body'] = chevron.render(**ctx)

        return mods


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


def render_union_typedef(type_collection: TypeCollection, type_name):
    context = {
        'template': "\n"
                    "typedef union {\n"
                    "    {{# members }}\n"
                    "    {{ type }} {{ name }};\n"
                    "    {{/ members }}\n"
                    "} {{ type_name }};",

        'data':     {
            'type_name': type_name,
            'members':   dict_to_chevron_list(type_collection[type_name]['members'], key_name='name', value_name='type')
        }
    }

    return chevron.render(**context)


def struct_formatter(types: TypeCollection, type_name, type_data, struct_value, context):
    if type(struct_value) is str:
        return struct_value

    values = ['.{} = {}'.format(field_name, types.render_value(field_type, struct_value.get(field_name, None), 'initialization'))
              for field_name, field_type in type_data['fields'].items()]

    if context == 'initialization':
        return '{{ {} }}'.format(', '.join(values))
    else:
        return '({}) {{ {} }}'.format(type_name, ', '.join(values))


def union_formatter(types: TypeCollection, type_name, type_data, union_value, context):
    if type(union_value) is str:
        return union_value

    if len(union_value) != 1:
        raise Exception('Only a single union member can be assigned')

    values = ['.{} = {}'.format(name, types.render_value(type_data['members'][name], value, 'initialization'))
              for name, value in union_value.items()]

    if context == 'initialization':
        return '{{ {} }}'.format(', '.join(values))
    else:
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
            'static':   {
                'type': TypeCollection.EXTERNAL_DEF
            }
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
    },

    TypeCollection.UNION:        {
        'typedef_renderer': render_union_typedef,
        'value_formatter':  union_formatter,
        'attributes':       {
            'required': ['members', 'default_value'],
            'optional': {'pass_semantic': TypeCollection.PASS_BY_POINTER},
            'static':   {
                'type': TypeCollection.UNION
            }
        }
    }
}


def process_type_def(type_name, type_def):
    type_data = type_def.copy()
    # determine type of definition
    if 'type' in type_data:
        type_category = type_data['type']
        del type_data['type']
    else:
        if 'defined_in' in type_data:
            type_category = TypeCollection.EXTERNAL_DEF
        elif 'aliases' in type_data:
            type_category = TypeCollection.ALIAS
        elif 'fields' in type_data:
            type_category = TypeCollection.STRUCT
        elif 'members' in type_data:
            type_category = TypeCollection.UNION
        else:
            raise Exception('Invalid type definition for {}'.format(type_name))

    try:
        attrs = type_info[type_category]['attributes']
        return {
            **attrs['static'],
            **copy(type_data, required=attrs['required'], optional=attrs['optional'])
        }
    except KeyError:
        print('Unknown type category {} set for {}'.format(type_category, type_name))
        raise
    except Exception as e:
        raise Exception('Type {} ({}) has unexpected attribute set: {}'.format(type_name, type_category, e))


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
        return types.render_value(port_data['data_type'], types.default_value(port_data['data_type']))
    else:
        return specified_return_value


def get_default_value_formatted(types: TypeCollection, port_data):
    default_value = types.default_value(port_data['data_type'])
    return types.render_value(port_data['data_type'], default_value)


port_type_data = {
    'ReadValue':        {
        'order':          3,
        'consumes':       {
            'array':    'single',
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
                'return_type':    'void',
                'arguments':      {'value': {'direction': 'out', 'data_type': port_data['data_type']}},
                'used_arguments': ['value'],
                'asserts':        'value != NULL',
                'body':           '*value = {};'.format(get_default_value_formatted(types, port_data))
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
                'func_name_pattern': '{}_Read_{}',
                'arguments':      {'value': {'direction': 'out', 'data_type': port_data['data_type']}}
            },
            'value':   lambda types, port_data: {
            },
            'pointer': lambda types, port_data: {
                'asserts':        'value != NULL',
                'used_arguments': ['value']
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
                'return_type':    port_data['data_type'],
                'arguments':      {'index': {'direction':'in', 'data_type': 'uint32_t'}},
                'return_value':   get_default_value(types, port_data),
                'asserts':        'index < {}'.format(port_data['count']),
                'used_arguments': ['index'],
            },
            'pointer': lambda types, port_data: {
                'return_type':    'void',
                'arguments':      {
                    'index': {'direction':'in', 'data_type': 'uint32_t'},
                    'value': {'direction': 'out', 'data_type': port_data['data_type']},
                },
                'asserts':        [
                    'index < {}'.format(port_data['count']),
                    'value != NULL'
                ],
                'used_arguments': ['value', 'index'],
                'body':           '*value = {};'.format(get_default_value_formatted(types, port_data))
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
                'arguments': {'value': {'direction': 'in', 'data_type': port_data['data_type']}},
            },
            'pointer': lambda types, port_data: {
                'arguments':      {'value': {'direction': 'in', 'data_type': port_data['data_type']}},
                'asserts':        'value != NULL',
                'used_arguments': ['value']
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
                'return_type':       'void',
                'arguments': {
                    'index': {'direction':'in', 'data_type': 'uint32_t'},
                    'value': {'direction': 'in', 'data_type': port_data['data_type']}
                }
            },
            'value':   lambda types, port_data: {
                'asserts':   'index < {}'.format(port_data['count']),
            },
            'pointer': lambda types, port_data: {
                'used_arguments': ['value'],
                'asserts':        [
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
                'return_type':    'void',
                'arguments':      {
                    'value': {'direction': 'out', 'data_type': port_data['data_type']}
                },
                'body':           '*value = {};'.format(types.render_value(port_data['data_type'], port_data['value'])),
                'asserts':        'value != NULL',
                'used_arguments': ['value']
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
                    'index': {'direction': 'in', 'data_type': 'uint32_t'}
                },
                'body':         'static const {} data[{}] = {{ {} }};'.format(port_data['data_type'],
                                                                              port_data['count'],
                                                                              ', '.join(port_data['value'])),
                'return_value': 'data[index]'
            },
            'pointer': lambda types, port_data: {
                'return_type':    'void',
                'arguments':      {
                    'index': {'direction': 'in', 'data_type': 'uint32_t'},
                    'value': {'direction': 'out', 'data_type': port_data['data_type']}
                },
                'used_arguments': ['value', 'index'],
                'body':           'static const {} data[{}] = {{ {} }};\n'
                                  '*value = data[index];'.format(port_data['data_type'],
                                                                 port_data['count'],
                                                                 ', '.join(port_data['value'])),
                'asserts':        [
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
    value_formatter = type_type_info.get('value_formatter')
    owner.types.add(type_name, type_type_data, type_type_info['typedef_renderer'], value_formatter)


def process_project_types(owner: Runtime, project_config):
    for type_name, type_data in project_config.get('types', {}).items():
        add_type_def(owner, type_name, type_data)


def process_component_ports_and_types(owner: Runtime, component_name, component_config):
    if 'ports' not in component_config:
        component_config['ports'] = {}

    for type_name, type_data in component_config.get('types', {}).items():
        add_type_def(owner, type_name, type_data)


def create_component_ports(owner: Runtime, component_name, component_data, context):
    for port_name, port_data in component_data['ports'].items():
        port_type = port_data['port_type']
        if port_type in port_type_data:
            function_data = impl_data_lookup(owner.types, port_data)
            function = owner.create_function_for_port(component_name, port_name, function_data)

            for argument in function_data.get('used_arguments', []):
                function.mark_argument_used(argument)

            for attribute in function_data.get('attributes', []):
                function.add_attribute(attribute)

            function.add_body(function_data.get('body', []))
            if function.return_type != 'void':
                return_value = function_data.get('return_value', owner.types.default_value(function.return_type))
                return_value = owner.types.render_value(port_data['data_type'], return_value)
                function.set_return_statement(return_value)

            context['functions']['{}/{}'.format(component_name, port_name)] = function


def sort_functions(owner: Runtime, context):
    def sort_by_name(fn):
        # only sort functions of known port types
        port = owner.get_port(fn)
        if port['port_type'] in port_type_data:
            return fn
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
