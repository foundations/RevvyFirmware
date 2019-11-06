import os
import re
import shutil


class TypeCollection:
    BUILTIN = 'builtin'
    ALIAS = 'type_alias'
    EXTERNAL_DEF = 'external_type_def'
    ENUM = 'enum'
    STRUCT = 'struct'
    UNION = 'union'

    PASS_BY_VALUE = 'value'
    PASS_BY_POINTER = 'pointer'

    def __init__(self):
        self._type_data = {
            'void':  {
                'type':          TypeCollection.BUILTIN,
                'pass_semantic': TypeCollection.PASS_BY_VALUE,
                'default_value': None
            },
            'void*': {
                'type':          TypeCollection.BUILTIN,
                'pass_semantic': TypeCollection.PASS_BY_VALUE,
                'default_value': 'NULL'
            }
        }
        self._resolved_names = {'void': 'void'}
        self._type_categories = {
            TypeCollection.BUILTIN: {
                'typedef_renderer': None,
                'attributes': {}
            }
        }

    def add_category(self, name, info):
        self._type_categories[name] = info

    def add(self, type_name, info):
        if type_name in self._type_data:
            # type already exists, check if they are the same
            resolved_known = self.resolve(type_name)
            resolved_type_data = self._type_data[resolved_known]

            if info['type'] == TypeCollection.ALIAS:
                resolved_new = self.resolve(info['aliases'])
                if resolved_known != resolved_new:
                    raise Exception('Type {} is already defined'.format(type_name))

            elif info['type'] == TypeCollection.EXTERNAL_DEF:
                if info['defined_in'] != resolved_type_data['defined_in']:
                    raise Exception('Type {} can\'t override a type from a different source'.format(type_name))

            elif info['type'] == TypeCollection.ENUM:
                if info['values'] != resolved_type_data['values']:
                    raise Exception('Enum {} is incompatible with previous definition'.format(type_name))

            elif info['type'] == TypeCollection.STRUCT:
                if info['fields'] != resolved_type_data['fields']:
                    raise Exception('Structure {} is incompatible with previous definition'.format(type_name))

            elif info['type'] == TypeCollection.UNION:
                if info['members'] != resolved_type_data['members']:
                    raise Exception('Structure {} is incompatible with previous definition'.format(type_name))

            else:
                raise Exception('Invalid type definition {}'.format(type_name))

        else:
            self._type_data[type_name] = info

    def resolve(self, type_name, past=None):
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
        type_data = self._type_data[type_name]

        if type_data['type'] == TypeCollection.ALIAS:
            past.append(type_name)
            resolved = self.resolve(type_data['aliases'], past)

        self._resolved_names[type_name] = resolved
        return resolved

    def _find_first_with_field(self, type_name, field):
        type_data = self.get(type_name)

        # find the concrete type or the first alias in the chain that has the requested field (e.g. default value)
        while not type_data.get(field):
            if type_data['type'] == TypeCollection.ALIAS:
                type_name = type_data['aliases']
                type_data = self.get(type_name)
            else:
                # we have something that is not an alias, stop here
                break

        return type_name, type_data

    def default_value(self, type_name):
        type_name, type_data = self._find_first_with_field(type_name, 'default_value')

        if type_data['type'] == TypeCollection.STRUCT:
            # if a struct member does not have default value, look for it recursively
            default = type_data.get('default_value', {})
            struct_fields = type_data['fields']

            # use 'or' so we only look up the default for the field if it is not given in the struct data
            return {name: default.get(name) or self.default_value(struct_fields[name]) for name in struct_fields}

        else:
            # concrete non-struct types must have default values
            return type_data['default_value']

    def render_value(self, type_name, value, context='assignment'):
        if value is None:
            value = self.default_value(type_name)

        try:
            resolved_type_data = self[type_name]
            type_category = resolved_type_data['type']
            value_formatter = self._type_categories[type_category]['value_formatter']
            return value_formatter(self, type_name, resolved_type_data, value, context)
        except (KeyError, TypeError):
            # by default treat the value as string
            return str(value)

    def passed_by(self, type_name):
        _, type_data = self._find_first_with_field(type_name, 'pass_semantic')

        try:
            return type_data['pass_semantic']
        except KeyError:
            raise Exception('Pass semantic not defined for {}'.format(type_name))

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
        type_data = self.get(type_name)
        type_category = type_data['type']
        renderer = self._type_categories[type_category]['typedef_renderer']

        try:
            return renderer(self, type_name)
        except TypeError:
            return None


def to_underscore(name):
    """
    >>> to_underscore('ADC')
    'adc'
    >>> to_underscore('FizzBuzz')
    'fizz_buzz'
    """
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()


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

    required_keys = set(required)
    present_keys = set(src.keys())
    optional_keys = set(optional.keys())

    missing_required = required_keys - present_keys
    not_required = present_keys - required_keys
    unexpected_keys = not_required - optional_keys

    if unexpected_keys:
        raise Exception('Unexpected keys: {}'.format(', '.join(unexpected_keys)))

    if missing_required:
        raise Exception('Missing keys: {}'.format(', '.join(missing_required)))

    return {**optional, **src}


def delete(path):
    try:
        os.remove(path)
    except FileNotFoundError:
        pass


def copy_file(src, dst):
    shutil.copy(src, dst)
    print('Copied: {} -> {}'.format(src, dst))


def change_file(filename, contents, delete_backup=False):
    """
    Update filename with contents

    If the file already exists, copies it to a unique backup file
    :param delete_backup: Delete backup file on success
    :param filename: the filename to write to
    :param contents: new file contents
    :return: True if the file is created, False if up to date, string if a backup was created before write
    """
    try:
        with open(filename, "r") as f:
            file_changed = contents != f.read()
        file_exists = True
    except FileNotFoundError:
        file_changed = True
        file_exists = False

    if file_exists:
        if not file_changed:
            return False

        i = 0
        backup = filename + ".bak"
        while os.path.isfile(backup):
            i += 1
            backup = "{}.bak{}".format(filename, i)

        copy_file(filename, backup)

        with open(filename, "w+") as f:
            f.write(contents)

        if delete_backup:
            delete(backup)
            backup = True

        return backup
    else:
        with open(filename, "w+") as f:
            f.write(contents)

        return True


class FileTransaction:
    def __init__(self, root):
        self._root = root
        self._new_folders = []
        self._files = {}

    def create_folder(self, folder):
        self._new_folders.append(folder.replace('/', os.path.sep))

    def update_file(self, file_name, contents):
        self._files[file_name.replace('/', os.path.sep)] = contents

    def apply(self, delete_backups=False):
        backups = {}
        new_files = []
        try:
            for folder in self._new_folders:
                try:
                    os.makedirs(os.path.join(self._root, folder))
                    print('New folder: {}'.format(folder))
                except OSError:
                    print('Skipped folder: {}'.format(folder))

            for file_name, contents in self._files.items():
                file_path = os.path.join(self._root, file_name)

                result = change_file(file_path, contents)
                if type(result) is str:
                    print('Modified: {}'.format(file_path))
                    backups[file_path] = result
                elif result:
                    print('Created: {}'.format(file_path))
                    new_files.append(file_path)
                else:
                    print('Up to date: {}'.format(file_path))

            if delete_backups:
                for file_name, backup in backups.items():
                    print('Deleted: {}'.format(backup))
                    delete(backup)

        except Exception:
            for file_name in new_files:
                delete(file_name)

            for file_name, backup in backups.items():
                delete(file_name)
                shutil.move(backup, file_name)

            for folder in self._new_folders:
                shutil.rmtree(folder)

            raise


def list_files_recursive(root):
    files = []

    try:
        for entry in os.listdir(root):
            path = os.path.join(root, entry)
            if os.path.isdir(path):
                files += list_files_recursive(path)
            else:
                files.append(path.replace(os.path.sep, '/'))
    except FileNotFoundError:
        pass

    return files
