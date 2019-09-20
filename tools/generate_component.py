import argparse
import sys
import os

from tools.generator_common import FileTransaction
from tools.plugins.AtmelStudioSupport import atmel_studio_support
from tools.plugins.BuiltinDataTypes import builtin_data_types
from tools.plugins.ProjectConfigCompactor import project_config_compactor
from tools.plugins.RuntimeEvents import runtime_events
from tools.runtime import Runtime

if __name__ == "__main__":
    # inquire name of new component
    parser = argparse.ArgumentParser()
    parser.add_argument('name', help='Component name')
    parser.add_argument('--create', help='Create component', action='store_true')
    parser.add_argument('--update-header', help='Generate header file', action='store_true')
    parser.add_argument('--update-source', help='Generate source file', action='store_true')
    parser.add_argument('--cleanup', help='Remove backups', action='store_true')

    """Create generates json and empty c/h pair, adds include, adds to project json, cproject"""

    args = parser.parse_args()

    # gather software component names
    component_name = args.name

    rt = Runtime("project.json")
    rt.add_plugin(project_config_compactor())
    rt.add_plugin(builtin_data_types())
    rt.add_plugin(runtime_events())
    rt.add_plugin(atmel_studio_support())

    rt.load(False)
    project_config = rt._project_config


    def component_file(filename):
        return "{}/{}/{}".format(project_config['settings']['components_folder'], component_name, filename)


    new_folders = []
    new_files = {}
    modified_files = {}

    ft = FileTransaction(os.path.realpath('.'))

    config_json_path = component_file('config.json')
    if args.create:

        # stop if component exists
        if component_name in project_config['components']:
            print('Component already exists')
            sys.exit(1)

        component_config = {
            'name':         component_name,
            'source_files': [component_name + '.c'],
            'runnables':    {},
            'ports':        {},
            'types':        {}
        }

        rt.add_component(component_name, component_config)

        project_config['components'].append(component_name)
        project_config['components'] = sorted(project_config['components'])

        ft.create_folder(os.path.join(project_config['settings']['components_folder'], component_name))

        # create component configuration json
        ft.update_file(config_json_path, rt.dump_component_config(component_name))

        # add component to project json
        ft.update_file('project.json', rt.dump_project_config())
    else:
        try:
            rt.load_component_config(component_name)
        except FileNotFoundError:
            print("Component {} does not exists. Did you mean to --create?".format(component_name))
            sys.exit(2)

    files = rt.update_component(component_name)

    for filename, contents in files.items():
        ft.update_file(filename, contents)

    ft.apply()
