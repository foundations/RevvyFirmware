import re


def create_empty_component_data(name: str):
    return {
        'name':         name,
        'source_files': [],
        'runnables':    {},
        'ports':        {},
        'types':        {}
    }


def create_port_ref(port):
    if type(port) is str:
        parts = port.split('/')
        return {
            'short_name': port,
            'component':  parts[0],
            'port':       parts[1]
        }
    elif type(port) is dict:
        return {
            'short_name': port['short_name'],
            'component':  port['component'],
            'port':       port.get('runnable', port.get('port'))
        }
    else:
        raise TypeError("port must either be a dict or a str")


def empty_component(name):
    return {
        'component_name': name,
        'runnables':      {},
        'ports':          {},
        'types':          {}
    }


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

    def compact_runnable_ref(runnable_ref):
        if type(runnable_ref) is str:
            return runnable_ref
        else:
            return "{}/{}".format(runnable_ref['component'], runnable_ref['runnable'])

    for group in config['runtime']['runnables']:
        compacted['runtime']['runnables'][group] = []

        for runnable in config['runtime']['runnables'][group]:
            compacted_runnable = compact_runnable_ref(runnable)
            compacted['runtime']['runnables'][group].append(compacted_runnable)

    for port_connection in config['runtime']['port_connections']:
        compacted_port_connection = {'provider': port_connection['provider']}

        consumers = []
        for consumer in port_connection['consumers']:
            consumers.append(compact_port_ref(consumer))

        if len(consumers) == 1:
            compacted_port_connection['consumer'] = consumers[0]
        else:
            compacted_port_connection['consumers'] = consumers

        compacted['runtime']['port_connections'].append(compacted_port_connection)

    return compacted


class TypeCollection:
    BUILTIN = 'builtin'
    ALIAS = 'type_alias'
    EXTERNAL_DEF = 'external_type_def'
    ENUM = 'enum'
    STRUCT = 'struct'

    PASS_BY_VALUE = 'value'
    PASS_BY_POINTER = 'pointer'

    def __init__(self):
        self._type_data = {
            'void': {
                'type': TypeCollection.BUILTIN,
                'pass_semantic': TypeCollection.PASS_BY_VALUE,
                'default_value': None
            }
        }
        self._resolved_names = {'void': 'void'}

    def add(self, type_name, info):
        if type_name in self._type_data:
            # type already exists, check if they are the same
            resolved_known = self.resolve(type_name)

            if info['type'] == TypeCollection.ALIAS:
                resolved_new = self.resolve(info['aliases'])
                if resolved_known != resolved_new:
                    raise Exception('Type {} is already defined'.format(type_name))

            elif info['type'] == TypeCollection.EXTERNAL_DEF:
                if info['defined_in'] != self._type_data[resolved_known]['defined_in']:
                    raise Exception('Type {} can\'t override a type from a different source'.format(type_name))

            elif info['type'] == TypeCollection.ENUM:
                if info['values'] != self._type_data[resolved_known]['values']:
                    raise Exception('Enum {} is incompatible with previous definition'.format(type_name))

            elif info['type'] == TypeCollection.STRUCT:
                if info['fields'] != self._type_data[resolved_known]['fields']:
                    raise Exception('Structure {} is incompatible with previous definition'.format(type_name))

            else:
                raise Exception('Invalid type definition {}'.format(type_name))

        else:
            self._type_data[type_name] = info

    def _resolve(self, type_name, past):
        if type_name not in self._type_data:
            raise Exception('Incomplete type: {}'.format(type_name))

        if type_name in self._resolved_names:  # cache
            return self._resolved_names[type_name]

        # record visited types to detect circular definition
        if past is None:
            past = []
        elif type_name in past:
            raise Exception('Circular type definition for {}'.format(type_name))

        if self._type_data[type_name]['type'] != TypeCollection.EXTERNAL_DEF:
            if self._type_data[type_name]['type'] == TypeCollection.ALIAS:
                past.append(type_name)
                resolved = self._resolve(self._type_data[type_name]['aliases'], past)
            else:
                resolved = type_name

            self._resolved_names[type_name] = resolved

            return resolved

        else:
            self._resolved_names[type_name] = type_name
            return type_name

    def default_value(self, type_name):
        resolved = self.get(type_name)
        while 'default_value' not in resolved or not resolved['default_value']:
            if resolved['type'] == TypeCollection.ALIAS:
                resolved = self.get(resolved['aliases'])
            else:
                break

        if resolved['type'] == TypeCollection.STRUCT:
            return {name: self.default_value(field_type) for name, field_type in resolved['fields'].items()}

        else:
            return resolved['default_value']

    def passed_by(self, type_name):
        resolved = self.get(type_name)
        while not resolved['pass_semantic']:
            if resolved['type'] == TypeCollection.ALIAS:
                resolved = self.get(resolved['aliases'])
            else:
                raise Exception('Pass semantic not defined for {}'.format(type_name))

        return resolved['pass_semantic']

    def resolve(self, type_name):
        return self._resolve(type_name, [])

    def __getitem__(self, type_name):
        return self.get(self.resolve(type_name))

    def get(self, type_name):
        return self._type_data[type_name]

    def __iter__(self):
        return iter(self._type_data.keys())


def collect_type_aliases(types, type_data: TypeCollection):
    aliases = []
    for type_name in types:
        resolved_type = type_data.resolve(type_name)
        type_type = type_data.get(type_name)['type']

        if type_type == TypeCollection.BUILTIN:
            pass

        elif type_type == TypeCollection.ALIAS:
            aliases.append({
                'type':      type_type,
                'type_name': type_name,
                'aliased':   resolved_type
            })

        elif type_type == TypeCollection.EXTERNAL_DEF:
            aliases.append({
                'type':       type_type,
                'type_name':  type_name,
                'defined_in': type_data[resolved_type]['defined_in']
            })

        elif type_type == TypeCollection.ENUM:
            enum_values = [{'value': value} for value in type_data[resolved_type]['values']]
            pystache_list_mark_last(enum_values)
            aliases.append({
                'type':      type_type,
                'is_enum':   True,
                'type_name': type_name,
                'values':    enum_values
            })

        elif type_type == TypeCollection.STRUCT:
            aliases.append({
                'type':      type_type,
                'is_struct': True,
                'type_name': type_name,
                'fields':    dict_to_pystache_list(type_data[resolved_type]['fields'],
                                                   key_name='name',
                                                   value_name='type')
            })

        else:
            raise Exception('Can not generate code for type {}'.format(type_type))

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


def change_file(filename, contents):
    try:
        with open(filename, "r") as f:
            file_changed = contents != f.read()
    except FileNotFoundError:
        file_changed = True

    if file_changed:
        with open(filename, "w+") as f:
            f.write(contents)

    return file_changed


def pystache_list_mark_last(data, last_key='last'):
    if data:
        data[-1][last_key] = True


def dict_to_pystache_list(data, key_name, value_name, last_key=None):
    """Transform a list of key-value pairs to a list of dicts with given key and value names.

    This is useful for passing dictionaries to pystache.
    If the last_key is given, the last item has an extra element with the last_key as key and True as value.

    >>> dict_to_pystache_list({'foo': 'bar'}, 'key', 'value')
    [{'key': 'foo', 'value': 'bar'}]
    >>> dict_to_pystache_list({'foo': 'bar', 'bar': 'baz'}, 'key', 'value', 'last')
    [{'key': 'foo', 'value': 'bar'}, {'key': 'bar', 'value': 'baz', 'last': True}]
    """
    pystache_list = [{key_name: key, value_name: value} for key, value in data.items()]
    if last_key:
        pystache_list_mark_last(pystache_list, last_key)
    return pystache_list
