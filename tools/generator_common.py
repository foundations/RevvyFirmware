import re


def parse_port_reference(port):
    """Parse shorthand form of port reference into a dictionary"""
    if type(port) is str:
        parts = port.split('/')
        port = {
            'short_name': port,
            'component':  parts[0],
            'port':       parts[1]
        }
    else:
        port['short_name'] = "{}/{}".format(port['component'], port['port'])

    return port


def empty_component(name):
    return {
        'component_name': name,
        'runnables': {},
        'ports': {},
        'types': {}
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
        compacted_port_connection = {}

        providers = []
        for provider in port_connection['providers']:
            providers.append(compact_port_ref(provider))

        if len(providers) == 1:
            compacted_port_connection['provider'] = providers[0]
        else:
            compacted_port_connection['providers'] = providers

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
    def __init__(self):
        self._type_data = {}
        self._resolved_names = {}

    def add(self, type_name, info):
        if type_name in self._type_data:
            # type already exists, check if they are the same
            resolved_known = self.resolve(type_name)

            if info['type'] == 'type_alias':
                resolved_new = self.resolve(info['aliases'])
                if resolved_known != resolved_new:
                    raise Exception('Type {} is already defined'.format(type_name))

            elif info['type'] == 'external_type_def':
                if info['defined_in'] != self._type_data[resolved_known]['defined_in']:
                    raise Exception('Type {} can\'t override a type from a different source'.format(type_name))

            elif info['type'] == 'enum':
                if info['values'] != self._type_data[resolved_known]['values']:
                    raise Exception('Enum {} is incompatible with previous definition'.format(type_name))

            elif info['type'] == 'struct':
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

        if self._type_data[type_name]['type'] in ['type_alias', 'enum', 'struct']:

            if self._type_data[type_name]['type'] == 'type_alias':
                past.append(type_name)
                resolved = self._resolve(self._type_data[type_name]['aliases'], past)
            else:
                resolved = type_name

            self._resolved_names[type_name] = resolved

            return resolved

        if self._type_data[type_name]['type'] == 'external_type_def':
            self._resolved_names[type_name] = type_name
            return type_name

    def default_value(self, type_name):
        resolved = self[type_name]
        if resolved['type'] == 'struct':
            return {field: self.default_value(resolved['fields'][field]) for field in resolved['fields']}

        else:
            return resolved['default_value']

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

        if type_type == 'type_alias':
            aliases.append({
                'type':      type_type,
                'type_name': type_name,
                'aliased':   resolved_type
            })

        elif type_type == 'external_type_def':
            aliases.append({
                'type':       type_type,
                'type_name':  type_name,
                'defined_in': type_data[resolved_type]['defined_in']
            })

        elif type_type == 'enum':
            enum_values = [{'value': value} for value in type_data[resolved_type]['values']]
            enum_values[len(enum_values) - 1]['last'] = True
            aliases.append({
                'type':      type_type,
                'is_enum':   True,
                'type_name': type_name,
                'values':    enum_values
            })

        elif type_type == 'struct':
            struct_fields = [{'name': name, 'type': type_data[resolved_type]['fields'][name]} for name in
                             type_data[resolved_type]['fields']]
            aliases.append({
                'type':      type_type,
                'is_struct': True,
                'type_name': type_name,
                'fields':    struct_fields
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
