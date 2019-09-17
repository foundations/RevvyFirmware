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

        resolved = type_name
        if self._type_data[type_name]['type'] != TypeCollection.EXTERNAL_DEF:
            if self._type_data[type_name]['type'] == TypeCollection.ALIAS:
                past.append(type_name)
                resolved = self._resolve(self._type_data[type_name]['aliases'], past)

        self._resolved_names[type_name] = resolved
        return resolved

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


def render_typedefs(types, type_data: TypeCollection):
    typedefs = []
    for type_name in types:
        try:
            renderer = type_data.get(type_name)['render_typedef']
            # noinspection PyCallingNonCallable
            typedefs.append(renderer(type_data, type_name))
        except KeyError:
            pass

    return typedefs


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


def chevron_list_mark_last(data, last_key='last'):
    if data:
        data[-1][last_key] = True
    return data


def list_to_chevron_list(data, key_name, last_key=None):
    """Transform a list of key-value pairs to a list of dicts with given key and value names.

    This is useful for passing dictionaries to chevron.
    If the last_key is given, the last item has an extra element with the last_key as key and True as value.

    >>> list_to_chevron_list(['foo', 'bar'], 'key')
    [{'key': 'foo'}, {'key': 'bar'}]
    >>> list_to_chevron_list(['foo', 'bar'], 'key', 'last')
    [{'key': 'foo'}, {'key': 'bar', 'last': True}]
    """
    chevron_list = [{key_name: value} for value in data]
    if last_key:
        chevron_list_mark_last(chevron_list, last_key)
    return chevron_list


def dict_to_chevron_list(data, key_name, value_name, last_key=None):
    """Transform a list of key-value pairs to a list of dicts with given key and value names.

    This is useful for passing dictionaries to chevron.
    If the last_key is given, the last item has an extra element with the last_key as key and True as value.

    >>> dict_to_chevron_list({'foo': 'bar'}, 'key', 'value')
    [{'key': 'foo', 'value': 'bar'}]
    >>> dict_to_chevron_list({'foo': 'bar', 'bar': 'baz'}, 'key', 'value', 'last')
    [{'key': 'foo', 'value': 'bar'}, {'key': 'bar', 'value': 'baz', 'last': True}]
    """
    chevron_list = [{key_name: key, value_name: value} for key, value in data.items()]
    if last_key:
        chevron_list_mark_last(chevron_list, last_key)
    return chevron_list


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
