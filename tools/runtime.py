import json

import chevron

from tools.generator_common import TypeCollection, change_file, copy, dict_to_chevron_list


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
        self._asserts = set()
        self._return_statement = None
        self._body = []
        self._attributes = set()

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
            self._body += list(body)

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
        ctx = {
            'template': "{{# attributes }}__attribute__(({{ . }}))\n{{/ attributes }}"
                        "{{ header }}\n"
                        "{\n"
                        "{{# asserts }}\n"
                        "    {{{ . }}}{{/ asserts }}{{# body }}\n"
                        "    {{{ . }}}{{/ body }}{{# return_statement }}\n"
                        "    return {{{ return_statement }}};{{/ return_statement }}\n"
                        "}\n",

            'data':     {
                'header':           self.get_header(),
                'return_statement': self._return_statement,
                'attributes':       list(self._attributes),
                'asserts':          list(self._asserts),
                'body':             [chunk.replace('\n', '    \n') for chunk in self._body]
            }
        }
        return chevron.render(**ctx)

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
        signal.generate_provider(context, self, provider_name)

    def add_consumer(self, consumer_name):
        self.consumers.append(consumer_name)
        self.signal.generate_consumer(self.context, self, consumer_name)


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

    def add_plugin(self, plugin: RuntimePlugin):
        self._plugins[plugin.name] = plugin
        plugin.bind(self)

    def load(self, load_components=True):
        self._call_plugin_event('init')

        with open(self._project_config_file, "r") as file:
            project_config = json.load(file)

        self._call_plugin_event('load_project_config', project_config)

        if 'settings' not in project_config:
            project_config['settings'] = {
                'name':              'Project Name',
                'components_folder': 'components',
                'required_plugins':  []
            }

        print('Loaded configuration for {}'.format(project_config['settings']['name']))

        for plugin_name in project_config['settings']['required_plugins']:
            if plugin_name not in self._plugins:
                raise Exception('Project requires {} plugin, which is not loaded'.format(plugin_name))

        self._project_config = project_config

        # add defined types
        for type_name, type_info in project_config['types'].items():
            self._types.add(type_name, type_info)

        if load_components:
            for component_name in project_config['components']:
                self.load_component_config(component_name)

        self._call_plugin_event('project_config_loaded', project_config)

    def load_component_config(self, component_name):
        if not self._project_config:
            self.load(False)

        project_settings = self._project_config['settings']

        component_config_file = '{}/{}/config.json'.format(project_settings['components_folder'], component_name)
        with open(component_config_file, "r") as file:
            component_config = json.load(file)
            self.add_component(component_name, component_config)

    def add_component(self, component_name, component_config):
        self._call_plugin_event('load_component_config', component_name, component_config)
        self._components[component_name] = component_config
        # add defined types
        for data_type, type_info in component_config['types'].items():
            self._types.add(data_type, type_info)
        for port_name, port_data in component_config['ports'].items():
            component_config['ports'][port_name] = self.process_port_def(component_name, port_name, port_data)

    def create_component(self, component):
        pass

    def update_component(self, component_name, update_header=True, update_source=True):
        self._call_plugin_event('create_component_ports', component_name, self._components[component_name])

    def add_signal_type(self, name, signal_type: SignalType):
        self._signal_types[name] = signal_type

    def create_function_for_port(self, component_name, port_name, function_data=None):
        if not function_data:
            function_data = self._get_function_data(component_name, port_name)

        fn_name = function_data['func_name_pattern'].format(component_name, port_name)
        return FunctionDescriptor.create(fn_name, function_data)

    def _get_function_data(self, component_name, port_name):
        port_data = self.get_port_data(component_name, port_name)
        port_type = port_data['port_type']
        function_data = self._port_impl_lookup[port_type](self._types, port_data)
        return function_data

    def get_port(self, short_name):
        parts = short_name.split('/')
        return self._components[parts[0]]['ports'][parts[1]]

    def get_port_data(self, component_name, port_name):
        return self._components[component_name]['ports'][port_name]

    def generate_runtime(self, filename):
        context = {
            'runtime': self,
            'functions': {},
            'declarations': [],
            'exported_function_declarations': []
        }

        consumers = {}
        providers = {}

        signals = {}

        for connection in self._project_config['runtime']['port_connections']:
            component_name = connection['provider']['component']
            provider_port_name = connection['provider']['port']
            provider_port_config = self.get_port_data(component_name, provider_port_name)
            provider_port_data = self._port_types[provider_port_config['port_type']]

            provider_short_name = "{}/{}".format(component_name, provider_port_name)
            provided_signal_types = provider_port_data['provides']

            if provider_short_name not in providers:
                providers[provider_short_name] = self.create_function_for_port(component_name, provider_port_name)

            provider_func = providers[provider_short_name]
            context['functions'][provider_short_name] = provider_func

            attributes = {key: connection[key] for key in connection if key not in ['provider', 'consumers']}

            if provider_short_name not in signals:
                signals[provider_short_name] = {}

            for consumer_ref in connection['consumers']:
                consumer_component = consumer_ref['component']
                consumer_port = consumer_ref['port']
                consumer_short_name = "{}/{}".format(consumer_component, consumer_port)

                consumer_port_config = self.get_port_data(consumer_component, consumer_port)
                consumer_port_type = consumer_port_config['port_type']

                consumed_signal_types = self._port_types[consumer_port_type]['consumes'].keys()

                inferred_signal_type = provided_signal_types.intersection(consumed_signal_types)

                if len(inferred_signal_type) == 0:
                    raise Exception('Incompatible ports: {} and {}'.format(provider_short_name, consumer_short_name))
                elif len(inferred_signal_type) > 1:
                    raise Exception('Connection type can not be inferred for {} and {}'
                                    .format(provider_short_name, consumer_short_name))

                signal_type_name = inferred_signal_type.pop()
                signal_type = self._signal_types[signal_type_name]

                if consumer_short_name in consumers:
                    # this port already is the consumer of some signal
                    # some ports can consume multiple signals, this is set in the port data
                    # (e.g. a runnable can be called by multiple events or calls)
                    if self._port_types[consumer_port_type]['consumes'][signal_type_name] == 'single':
                        raise Exception('{} cannot consume multiple signals'.format(consumer_short_name))

                    consumer_func = consumers[consumer_short_name]
                else:
                    function_data = self._get_function_data(consumer_component, consumer_port)
                    # only weak functions need implementation generated
                    if 'weak' in function_data.get('attributes', []):
                        func = self.create_function_for_port(consumer_component, consumer_port, function_data)
                        consumers[consumer_short_name] = func
                        context['functions'][consumer_short_name] = func
                    else:
                        context['functions'][consumer_short_name] = None

                signal_name = '{}_{}_{}'\
                    .format(provider_short_name, consumer_short_name, signal_type_name)\
                    .replace('/', '_')

                def create_signal_connection(attributes, signal_name, signal_type):
                    signal = signal_type.create_connection(context, signal_name, provider_short_name, attributes)
                    signal.add_consumer(consumer_short_name)
                    return signal

                try:
                    signals_of_current_type = signals[provider_short_name][signal_type_name]
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
                        signals[provider_short_name][signal_type_name] = [new_signal]
                    else:
                        signals[provider_short_name][signal_type_name] = new_signal

        # change_file(filename + '.h', pystache.render(header_template, generator_context))
        # change_file(filename + '.c', pystache.render(source_template, generator_context))
        pass

    def _call_plugin_event(self, event_name, *args):
        for plugin in self._plugins:
            try:
                self._plugins[plugin].handle(event_name, args)
            except Exception:
                print('Error while processing {}/{}'.format(plugin, event_name))
                raise

    def add_port_type(self, port_type_name, data, lookup):
        self._port_types[port_type_name] = data
        self._port_impl_lookup[port_type_name] = lookup

    def process_port_def(self, component_name, port_name, port):
        short_name = '{}/{}'.format(component_name, port_name)
        port_type = port['port_type']

        try:
            attributes = self._port_types[port_type]['def_attributes']
            del port['port_type']
            return {
                'short_name': short_name,
                'port_type':  port_type,
                **attributes['static'],
                **copy(port, attributes['required'], attributes['optional'])
            }

        except KeyError:
            return port

        except Exception as e:
            raise Exception('Port {} ({}) has unexpected attribute set: {}'.format(short_name, port_type, e))

    def add_function(self, key, function: FunctionDescriptor):
        self._functions[key] = function

    @property
    def functions(self):
        return self._functions

    @property
    def types(self):
        return self._types

    @property
    def port_types(self):
        return self._port_types
