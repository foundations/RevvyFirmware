import json
import pystache

from tools.generator_common import TypeCollection, change_file, copy


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


class Runtime:
    def __init__(self, project_config_file):
        self._project_config_file = project_config_file
        self._plugins = {}
        self._defined_types = {}
        self._project_config = {}
        self._components = {}
        self._types = TypeCollection()
        self._port_types = {}

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
        pass

    def generate_runtime(self, filename):
        generator_context = {}

        self._call_plugin_event('generate_runtime', generator_context)

        # change_file(filename + '.h', pystache.render(header_template, generator_context))
        # change_file(filename + '.c', pystache.render(source_template, generator_context))

    def _call_plugin_event(self, event_name, *args):
        for plugin in self._plugins:
            try:
                self._plugins[plugin].handle(event_name, args)
            except Exception:
                print('Error while processing {}/{}'.format(plugin, event_name))
                raise

    def add_port_type(self, port_type_name, data):
        self._port_types[port_type_name] = data

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
