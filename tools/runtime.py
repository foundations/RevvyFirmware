import json
import os

import chevron

from tools.generator_common import TypeCollection, copy, dict_to_chevron_list, to_underscore, list_to_chevron_list, \
    unique

runtime_header_template = """#ifndef GENERATED_RUNTIME_H_
#define GENERATED_RUNTIME_H_

{{# type_includes }}
#include {{{ header }}}
{{/ type_includes }}

{{# types }}
{{{ . }}}
{{/ types }}

{{# components }}
#define COMPONENT_TYPES_{{ guard_def }}_H_
{{/ components }}

{{# components }}
#include "components/{{ name }}/{{ name }}.h"
{{/ components }}

{{# function_declarations }}
{{{ . }}};
{{/ function_declarations }}

#endif /* GENERATED_RUNTIME_H */
"""

component_header_template = '''#ifndef COMPONENT_{{ guard_def }}_H_
#define COMPONENT_{{ guard_def }}_H_

#ifndef COMPONENT_TYPES_{{ guard_def }}_H_
#define COMPONENT_TYPES_{{ guard_def }}_H_

{{# type_includes }}
#include {{{ header }}}
{{/ type_includes }}

{{# types }}
{{{ . }}}
{{/ types }}

#endif /* COMPONENT_TYPES_{{ guard_def }}_H_ */

{{# function_headers }}
{{{ . }}};
{{/ function_headers }}

#endif /* COMPONENT_{{ guard_def }}_H_ */
'''

source_template = '''{{# includes }}
#include {{{ header }}}
{{/ includes }}

{{# variables }}
{{{ . }}}
{{/ variables }}
{{# functions }}
\n{{{ . }}}{{/ functions }}'''


class RuntimePlugin:
    def __init__(self, name, handlers: dict, requires: list = None):
        self.name = name
        self._event_handlers = handlers
        self._owner = None
        self._requires = requires or []

    def bind(self, owner):
        for plugin in self._requires:
            if plugin not in owner._plugins:
                raise Exception('{} requires unloaded plugin {}'.format(self.name, plugin))
        self._owner = owner

    def handle(self, event_name, args):
        try:
            handler = self._event_handlers[event_name]
        except KeyError:
            return

        print('Running {}::{}'.format(self.name, event_name))
        handler(self._owner, *args)


class FunctionDescriptor:

    @staticmethod
    def create(name, data: dict = None):
        if not data:
            data = {}

        fd = FunctionDescriptor(name, data.get('return_type', 'void'))

        for name, data_type in data.get('arguments', {}).items():
            fd.add_argument(name, data_type)

        return fd

    def __init__(self, func_name, return_type='void'):
        self._name = func_name
        self._return_type = return_type
        self._arguments = {}
        self._used_arguments = set()
        self._asserts = set()
        self._return_statement = None
        self._body = []
        self._attributes = set()

    def mark_argument_used(self, arg):
        self._used_arguments.add(arg)

    def add_attribute(self, attribute):
        self._attributes.add(attribute)

    def add_argument(self, name, data_type):
        self._arguments[name] = data_type

    def add_input_assert(self, statements):
        if type(statements) is str:
            self._asserts.add('ASSERT({});'.format(statements))
        else:
            for statement in statements:
                self._asserts.add('ASSERT({});'.format(statement))

    def add_body(self, body):
        if type(body) is str:
            self._body.append(body)
        else:
            self._body += body

    def prepend_body(self, body):
        self._body.insert(0, body)

    def set_return_statement(self, statement):
        if self._return_statement:
            raise Exception('Return statement already set for {}'.format(self._name))

        if statement:
            if self._return_type == 'void':
                raise Exception('Function {} is void'.format(self._name))

            self._return_statement = statement

    def get_header(self):
        ctx = {
            'template': '{{ return_type }} {{ function_name }}({{^ arguments }}void{{/ arguments }}{{# arguments }}{{ type }} {{ name }}{{^ last}}, {{/ last }}{{/ arguments }})',
            'data':     {
                'return_type':   self._return_type,
                'function_name': self._name,
                'arguments':     dict_to_chevron_list(self._arguments, 'name', 'type', 'last')
            }
        }
        return chevron.render(**ctx)

    def get_function(self):
        body = list(self._asserts)
        body += [chunk.replace('\n', '\n    ') for chunk in self._body]
        if self._return_statement:
            body.append('return {};'.format(self._return_statement))

        def remove_trailing_spaces(l):
            return '\n'.join([line.rstrip(' ') for line in l.split('\n')])

        unused_arguments = self._arguments.keys() - self._used_arguments

        for arg in unused_arguments:
            body.insert(0, '(void) {};'.format(arg))

        ctx = {
            'template': "{{# attributes }}__attribute__(({{ . }}))\n{{/ attributes }}"
                        "{{ header }}\n"
                        "{\n"
                        "{{# body }}\n"
                        "    {{{ . }}}\n"
                        "{{/ body }}\n"
                        "}\n",

            'data':     {
                'header':     self.get_header(),
                'attributes': list(self._attributes),
                'body':       [remove_trailing_spaces(line) for line in body]
            }
        }
        return chevron.render(**ctx)

    @property
    def return_type(self):
        return self._return_type

    @property
    def arguments(self):
        return self._arguments

    def referenced_types(self):
        return list(self._arguments.values()) + [self._return_type]


class SignalConnection:
    def __init__(self, context, name, signal, provider_name, attributes):
        self.name = name
        self.signal = signal
        self.provider = provider_name
        self.attributes = attributes
        self.consumers = []
        self.context = context

        signal.create(context, self)

    def add_consumer(self, consumer_name):
        self.consumers.append(consumer_name)

    def generate(self):
        # collect implementations in a list
        function_mods_list = []
        function_mods = self.signal.generate_provider(self.context, self, self.provider)
        if function_mods:
            function_mods_list.append(function_mods)

        for consumer in self.consumers:
            function_mods = self.signal.generate_consumer(self.context, self, consumer)
            if function_mods:
                function_mods_list.append(function_mods)

        self.context['runtime'].raise_event('signal_generated', self, function_mods_list)

        # apply modifications
        for function_mods in function_mods_list:
            for function_name, mods in function_mods.items():
                function = self.context['functions'][function_name]
                if 'body' in mods:
                    function.add_body(mods['body'])

                if 'return_statement' in mods:
                    function.set_return_statement(mods['return_statement'])

                if 'used_arguments' in mods:
                    for argument in mods['used_arguments']:
                        function.mark_argument_used(argument)


class SignalType:
    def __init__(self, consumers='multiple', required_attributes=None):
        if required_attributes is None:
            required_attributes = []

        self._consumers = consumers
        self.required_attributes = frozenset(required_attributes)

    @property
    def consumers(self):
        return self._consumers

    def create(self, context, connection: SignalConnection):
        pass

    def generate_provider(self, context, connection: SignalConnection, provider_name):
        pass

    def generate_consumer(self, context, connection: SignalConnection, consumer_name):
        pass

    def create_connection(self, context, name, provider, attributes):
        missing_attributes = self.required_attributes.difference(attributes.keys())
        if missing_attributes:
            raise Exception('{} attributes are missing from connection provided by {}'
                            .format(", ".join(missing_attributes), provider))
        return SignalConnection(context, name, self, provider, attributes)


class Runtime:
    def __init__(self, project_config_file):
        self._project_config_file = project_config_file
        self._plugins = {}
        self._defined_types = {}
        self._project_config = {}
        self._components = {}
        self._types = TypeCollection()
        self._port_types = {}
        self._port_impl_lookup = {}
        self._signal_types = {}
        self._functions = {}

        self._ports = {}

        self._print_warnings = ['unconnected_signals']

    def add_plugin(self, plugin: RuntimePlugin):
        self._plugins[plugin.name] = plugin
        plugin.bind(self)

    def load(self, load_components=True):
        self.raise_event('init')

        with open(self._project_config_file, "r") as file:
            project_config = json.load(file)

        self.raise_event('load_project_config', project_config)

        if 'settings' not in project_config:
            project_config['settings'] = {
                'name':              'Project Name',
                'components_folder': 'components',
                'required_plugins':  []
            }

        print('Loaded configuration for {}'.format(project_config['settings']['name']))

        self._project_config = project_config

        for plugin_name in self.settings['required_plugins']:
            if plugin_name not in self._plugins:
                raise Exception('Project requires {} plugin, which is not loaded'.format(plugin_name))

        if load_components:
            for component_name in project_config['components']:
                self.load_component_config(component_name)

        self.raise_event('project_config_loaded', project_config)

    def add_port_type(self, port_type_name, data, lookup):
        self._port_types[port_type_name] = data
        self._port_impl_lookup[port_type_name] = lookup

    def process_port_def(self, component_name, port_name, port):
        port_type = port['port_type']

        try:
            attributes = self._port_types[port_type]['def_attributes']
            del port['port_type']
            return {
                'port_type': port_type,
                **attributes['static'],
                **copy(port, attributes['required'], attributes['optional'])
            }

        except KeyError:
            return port

        except Exception as e:
            raise Exception('Port {}/{} ({}) has unexpected attribute set: {}'
                            .format(component_name, port_name, port_type, e))

    def load_component_config(self, component_name):
        if not self._project_config:
            self.load(False)

        component_config_file = '{}/{}/config.json'.format(self.settings['components_folder'], component_name)
        with open(component_config_file, "r") as file:
            component_config = json.load(file)
            self.add_component(component_name, component_config)

    def add_component(self, component_name, component_config):
        self.raise_event('load_component_config', component_name, component_config)
        self._components[component_name] = component_config

        if not component_config['ports']:
            print('Warning: {} has no ports'.format(component_name))

        for port_name, port_data in component_config['ports'].items():
            processed_port = self.process_port_def(component_name, port_name, port_data)
            component_config['ports'][port_name] = processed_port

            short_name = '{}/{}'.format(component_name, port_name)
            self._ports[short_name] = processed_port

    def _process_type(self, type_name):
        defs = []
        includes = set()
        if type(type_name) is str:
            try:
                self._types.get(type_name)
            except KeyError:
                type_name = type_name.replace('const ', '').replace('*', '').replace(' ', '')

            resolved_type_data = self._types[type_name]
            if resolved_type_data['type'] == TypeCollection.EXTERNAL_DEF:
                includes.add(resolved_type_data['defined_in'])

            if resolved_type_data['type'] == TypeCollection.STRUCT:
                d, i = self._process_type(resolved_type_data['fields'].values())
                defs += d
                includes.update(i)

            typedef = self._types.generate_typedef(type_name)
            if typedef:
                defs.append(typedef)
        else:
            for tt in type_name:
                d, i = self._process_type(tt)
                defs += d
                includes.update(i)

        return defs, includes

    def update_component(self, component_name):

        component_folder = os.path.join(self.settings['components_folder'], component_name)
        source_file = os.path.join(component_folder, component_name + '.c')
        header_file = os.path.join(component_folder, component_name + '.h')
        config_file = os.path.join(component_folder, 'config.json')

        context = {
            'runtime':          self,
            'component_folder': component_folder,
            'functions':        {},
            'declarations':     [],
            'files':            {
                config_file: '',
                source_file: '',
                header_file: ''
            },
            'folders':          [component_name]
        }
        self.raise_event('create_component_ports', component_name, self._components[component_name], context)

        self.raise_event('before_generating_component', component_name, context)

        funcs = context['functions'].values()
        function_headers = [fn.get_header() for fn in funcs]
        functions = [fn.get_function() for fn in funcs]

        includes = [
            '"{}.h"'.format(component_name),
            '"utils.h"'
        ]
        for f in funcs:
            if f._asserts:
                includes.append('"utils_assert.h"')
                break

        types, type_includes = self._process_type(self._components[component_name].get('types', []))
        for f in funcs:
            t, i = self._process_type(f.referenced_types())
            types += t
            type_includes.update(i)

        ctx = {
            'includes':         list_to_chevron_list(includes, 'header'),
            'component_name':   component_name,
            'guard_def':        to_underscore(component_name).upper(),
            'variables':        context['declarations'],
            'types':            unique(types),
            'type_includes':    list_to_chevron_list(sorted(type_includes), 'header'),
            'functions':        functions,
            'function_headers': function_headers
        }

        context['files'][config_file] = self.dump_component_config(component_name)
        context['files'][source_file] = chevron.render(source_template, ctx)
        context['files'][header_file] = chevron.render(component_header_template, ctx)

        self.raise_event('generating_component', component_name, context)

        return context['files']

    def add_signal_type(self, name, signal_type: SignalType):
        self._signal_types[name] = signal_type

    def create_function_for_port(self, component_name, port_name, function_data=None):
        if not function_data:
            short_name = "{}/{}".format(component_name, port_name)
            function_data = self._get_function_data(short_name)

        fn_name = function_data['func_name_pattern'].format(component_name, port_name)
        function = FunctionDescriptor.create(fn_name, function_data)
        function.add_input_assert(function_data.get('asserts', []))

        return function

    def get_port(self, short_name):
        return self._ports[short_name]

    def _get_function_data(self, short_name):
        port_data = self.get_port(short_name)
        port_type = port_data['port_type']

        return self._port_impl_lookup[port_type](self._types, port_data)

    def get_port_type_data(self, short_name):
        port_data = self.get_port(short_name)
        port_type = port_data['port_type']

        return self._port_types[port_type]

    def generate_runtime(self, filename):
        source_file_name = filename + '.c'
        header_file_name = filename + '.h'

        context = {
            'runtime':                        self,
            'files':                          {source_file_name: '', header_file_name: ''},
            'functions':                      {},
            'declarations':                   [],
            'exported_function_declarations': [],
            'signals':                        {}
        }

        for connection in self._project_config['runtime']['port_connections']:
            provider_ref = connection['provider']

            provider_short_name = provider_ref['short_name']

            provider_port_type_data = self.get_port_type_data(provider_short_name)
            provided_signal_types = provider_port_type_data['provides']

            def create_if_weak(port_ref):
                """Generate function for the given port - but only if the default implementation is weak"""
                function_data = self._get_function_data(port_ref['short_name'])
                if 'weak' in function_data.get('attributes', []):
                    return self.create_function_for_port(port_ref['component'], port_ref['port'], function_data)
                else:
                    return None

            def create_signal_connection(attributes, signal_name, signal_type):
                signal = signal_type.create_connection(context, signal_name, provider_short_name, attributes)
                signal.add_consumer(consumer_short_name)
                return signal

            if provider_short_name not in context['functions']:
                context['functions'][provider_short_name] = create_if_weak(provider_ref)

            attributes = {key: connection[key] for key in connection if key not in ['provider', 'consumers']}

            # create a dict to store providers signals
            if provider_short_name not in context['signals']:
                context['signals'][provider_short_name] = {}
            provider_signals = context['signals'][provider_short_name]

            for consumer_ref in connection['consumers']:
                consumer_short_name = consumer_ref['short_name']

                consumer_port_type_data = self.get_port_type_data(consumer_short_name)
                consumed_signal_types = consumer_port_type_data['consumes']
                inferred_signal_type = provided_signal_types.intersection(consumed_signal_types)

                if len(inferred_signal_type) == 0:
                    raise Exception('Incompatible ports: {} and {}'.format(provider_short_name, consumer_short_name))
                elif len(inferred_signal_type) > 1:
                    raise Exception('Connection type can not be inferred for {} and {}'
                                    .format(provider_short_name, consumer_short_name))

                signal_type_name = inferred_signal_type.pop()
                signal_type = self._signal_types[signal_type_name]

                if consumer_short_name not in context['functions']:
                    context['functions'][consumer_short_name] = create_if_weak(consumer_ref)
                else:
                    # this port already is the consumer of some signal
                    # some ports can consume multiple signals, this is set in the port data
                    # (e.g. a runnable can be called by multiple events or calls)
                    if consumer_port_type_data['consumes'][signal_type_name] == 'single':
                        raise Exception('{} cannot consume multiple signals'.format(consumer_short_name))

                signal_name = '{}_{}_{}' \
                    .format(provider_short_name, consumer_short_name, signal_type_name) \
                    .replace('/', '_')

                try:
                    signals_of_current_type = provider_signals[signal_type_name]
                    if type(signals_of_current_type) is list:
                        if signal_type.consumers == 'multiple_signals':
                            # create new signal in all cases
                            signal_name += str(len(signals_of_current_type))

                            new_signal = create_signal_connection(attributes, signal_name, signal_type)

                            signals_of_current_type.append(new_signal)
                        else:
                            signals_of_current_type.add_consumer(consumer_short_name)
                    elif signal_type.consumers == 'multiple':
                        signals_of_current_type.add_consumer(consumer_short_name)
                    else:
                        raise Exception('Multiple consumers not allowed for {} signal (provided by {})'
                                        .format(signal_type_name, provider_short_name))
                except KeyError:
                    new_signal = create_signal_connection(attributes, signal_name, signal_type)

                    if signal_type.consumers == 'multiple_signals':
                        provider_signals[signal_type_name] = [new_signal]
                    else:
                        provider_signals[signal_type_name] = new_signal

        if 'unconnected_signals' in self._print_warnings:
            all_unconnected = set(self._ports.keys()).difference(context['functions'])
            for unconnected in all_unconnected:
                print('Warning: {} port is not connected'.format(unconnected))

        for signals in context['signals'].values():
            for signal in signals.values():
                if type(signal) is list:
                    for s in signal:
                        s.generate()
                else:
                    signal.generate()

        self.raise_event('before_generating_runtime', context)

        type_names = []
        for c in self._components.values():
            type_names += c.get('types', {}).keys()

        for f in context['functions'].values():
            if f:
                type_names += f.referenced_types()

        types = []
        includes = set()

        for t in type_names:
            ty, i = self._process_type(t)
            types += ty
            includes.update(i)

        output_filename = filename[filename.rfind('/') + 1:]
        template_data = {
            'output_filename':       output_filename,
            'includes':              [
                {'header': '"{}.h"'.format(output_filename)},
                {'header': '"utils.h"'}
            ],
            'components':            [
                {
                    'name':      name,
                    'guard_def': to_underscore(name).upper()
                } for name in self._components if name != 'Runtime'],  # TODO
            'types':                 unique(types),
            'type_includes':         list_to_chevron_list(sorted(includes), 'header'),
            'function_declarations': [context['functions'][func_name].get_header() for func_name in
                                      context['exported_function_declarations']],
            'functions':             [func.get_function() for func in context['functions'].values() if func],
            'variables':             context['declarations']
        }

        context['files'][source_file_name] = chevron.render(source_template, template_data)
        context['files'][header_file_name] = chevron.render(runtime_header_template, template_data)

        self.raise_event('after_generating_runtime', context)

        return context['files']

    def raise_event(self, event_name, *args):
        for plugin in self._plugins:
            try:
                self._plugins[plugin].handle(event_name, args)
            except Exception:
                print('Error while processing {}/{}'.format(plugin, event_name))
                raise

    @property
    def functions(self):
        return self._functions

    @property
    def types(self):
        return self._types

    @property
    def port_types(self):
        return self._port_types

    @property
    def settings(self):
        return self._project_config['settings']

    def dump_component_config(self, component_name):
        config = self._components[component_name].copy()
        self.raise_event('save_component_config', config)
        return json.dumps(config, indent=4)

    def dump_project_config(self):
        config = self._project_config.copy()
        self.raise_event('save_project_config', config)
        return json.dumps(config, indent=4)
