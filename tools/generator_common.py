import re


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
            },
            'void*': {
                'type': TypeCollection.BUILTIN,
                'pass_semantic': TypeCollection.PASS_BY_VALUE,
                'default_value': 'NULL'
            }
        }
        self._resolved_names = {'void': 'void'}
        self._renderers = {
            'void': None,
            'void*': None
        }

    def add(self, type_name, info, renderer):
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
            self._renderers[type_name] = renderer

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

    def export(self):
        def stripped(type_name):
            data = self._type_data[type_name].copy()
            if data['type'] not in [TypeCollection.ENUM, TypeCollection.STRUCT]:
                del data['type']

            return data

        return {name: stripped(name) for name in self._type_data if name not in ['void', 'void*']}

    def generate_typedef(self, type_name):
        renderer = self._renderers[type_name]
        return renderer(self, type_name) if renderer else None


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


def unique(values: list):
    """Removes duplicates from the input list

    >>> unique(['a', 'b', 'a', 'a', 'c', 'b', 'd'])
    ['a', 'b', 'c', 'd']"""
    unique_values = []
    for value in values:
        if value not in unique_values:
            unique_values.append(value)
    return unique_values
