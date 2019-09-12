import json
import re

component_folder_pattern = 'rrrc/components/{}'
component_file_pattern = 'rrrc/components/{}/{}'


def process_runnable_def(component, runnable_name, runnable):
    return {
        'short_name':  '{}/{}'.format(component, runnable_name),
        'arguments':   runnable.get('arguments', {}),
        'return_type': runnable.get('return_type', 'void')
    }


def process_port_def(component, port_name, port):
    short_name = '{}/{}'.format(component, port_name)
    if port['port_type'] == 'Event':
        return {
            'short_name':  short_name,
            'port_type':   port['port_type'],
            'return_type': 'void',
            'arguments':   port.get('arguments', {})
        }

    if port['port_type'] == 'ServerCall':
        return {
            'short_name':  short_name,
            'port_type':   port['port_type'],
            'return_type': port.get('return_type', 'void'),
            'arguments':   port.get('arguments', {})
        }

    elif port['port_type'] == 'ReadValue':
        return {
            'short_name':    short_name,
            'port_type':     port['port_type'],
            'data_type':     port['data_type'],
            'default_value': port.get('default_value', None)
        }

    else:
        port['short_name'] = short_name
        return port


def process_type_def(type_def):
    processed_type_def = {}

    if 'defined_in' in type_def:
        processed_type_def['type'] = 'external_type_def'
        processed_type_def['defined_in'] = type_def['defined_in']
        processed_type_def['default_value'] = type_def['default_value']
        processed_type_def['pass_semantic'] = type_def.get('pass_semantic', 'value')

    elif 'aliases' in type_def:
        processed_type_def['type'] = 'type_alias'
        processed_type_def['aliases'] = type_def['aliases']

    else:
        processed_type_def['type'] = type_def['type']
        if type_def['type'] == 'enum':
            processed_type_def['values'] = type_def['values']
            processed_type_def['default_value'] = type_def['default_value']
            processed_type_def['pass_semantic'] = type_def.get('pass_semantic', 'value')

        elif type_def['type'] == 'struct':
            processed_type_def['fields'] = type_def['fields']
            processed_type_def['default_values'] = type_def.get('default_values', {})
            processed_type_def['pass_semantic'] = type_def.get('pass_semantic', 'pointer')

        else:
            raise Exception('Unsupported type {}'.format(type_def['type']))

    return processed_type_def


def process_runnable_defs(component, runnable_config):
    return {runnable: process_runnable_def(component, runnable, runnable_config[runnable]) for runnable in
            runnable_config}


def process_port_defs(component, port_config):
    return {port: process_port_def(component, port, port_config[port]) for port in port_config}


def process_type_defs(type_defs):
    return {type_name: process_type_def(type_defs[type_name]) for type_name in type_defs}


def load_component_config(path):
    with open(path, 'r') as component_config_file:
        component_config = json.load(component_config_file)
        component = component_config['component_name']

        component_config['runnables'] = process_runnable_defs(component, component_config.get('runnables', {}))
        component_config['ports'] = process_port_defs(component, component_config.get('ports', {}))
        component_config['types'] = process_type_defs(component_config.get('types', {}))
    return component_config


def parse_runnable_reference(runnable):
    """Parse shorthand form of runnable reference into a dictionary"""
    if type(runnable) is str:
        parts = runnable.split('/')
        runnable = {
            'short_name': runnable,
            'component':  parts[0],
            'runnable':   parts[1]
        }
    else:
        runnable['short_name'] = "{}/{}".format(runnable['component'], runnable['port'])

    return runnable


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


def load_project_config(project_config_file):
    """Load configuration file and expand shorthand forms for the later stages"""
    with open(project_config_file, "r") as f:
        project_config = json.load(f)

        processed_runnables = {}
        for runnable_group in project_config['runtime'].get('runnables', {}):
            processed_runnables[runnable_group] = []
            for runnable in project_config['runtime']['runnables'][runnable_group]:
                processed_runnables[runnable_group].append(parse_runnable_reference(runnable))

        processed_port_connections = []
        for port_connection in project_config['runtime'].get('port_connections', []):
            if 'providers' not in port_connection:
                try:
                    providers = [port_connection['provider']]
                except KeyError:
                    providers = []
            else:
                providers = port_connection['providers']

            if 'consumers' not in port_connection:
                try:
                    consumers = [port_connection['consumer']]
                except KeyError:
                    consumers = []
            else:
                consumers = port_connection['consumers']

            processed_port_connections.append({
                'providers': [parse_port_reference(provider) for provider in providers],
                'consumers': [parse_port_reference(consumer) for consumer in consumers]
            })

        project_config['types'] = process_type_defs(project_config.get('types', {}))
        project_config['runtime']['runnables'] = processed_runnables
        project_config['runtime']['port_connections'] = processed_port_connections

    return project_config


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
