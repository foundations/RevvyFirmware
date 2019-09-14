import json


class RuntimePlugin:
    def __init__(self, name, handlers: dict):
        self.name = name
        self._event_handlers = handlers
        self._owner = None

    def bind(self, owner):
        self._owner = owner

    def handle(self, event_name, args):
        try:
            handler = self._event_handlers[event_name]
            print('Running {}::{}'.format(self.name, event_name))
            handler(self._owner, *args)
        except KeyError:
            pass


class Runtime:
    def __init__(self, project_config_file):
        self._project_config_file = project_config_file
        self._plugins = {}
        self._defined_types = {}
        self._project_config = {}
        self._components = {}

    def add_plugin(self, plugin: RuntimePlugin):
        self._plugins[plugin.name] = plugin
        plugin.bind(self)

    def load(self, load_components=True):
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

        if load_components:
            for component_name in project_config['components']:
                self.load_component_config(component_name)

        self._call_plugin_event('project_config_loaded')

    def load_component_config(self, component_name):
        if not self._project_config:
            self.load(False)

        project_settings = self._project_config['settings']

        component_config_file = '{}/{}/config.json'.format(project_settings['components_folder'], component_name)
        with open(component_config_file, "r") as file:
            component_config = json.load(file)
            self._call_plugin_event('load_component_config', component_name, component_config)
            self._components[component_name] = component_config

    def create_component(self, component):
        pass

    def update_component(self, update_header=True, update_source=True):
        pass

    def generate_runtime(self):
        pass

    def _call_plugin_event(self, event_name, *args):
        for plugin in self._plugins:
            self._plugins[plugin].handle(event_name, args)
