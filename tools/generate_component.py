import argparse
import sys
import os
import shutil

from tools.generator_common import change_file
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

        # Create component skeleton
        new_folders.append(os.path.join(project_config['settings']['components_folder'], component_name))

        # create component configuration json
        new_files[config_json_path] = rt.dump_component_config(component_name)

        # replace sources list with new one and set for file modification
        modified_files['project.json'] = rt.dump_project_config()
    else:
        try:
            rt.load_component_config(component_name)
        except FileNotFoundError:
            print("Component {} does not exists. Did you mean to --create?".format(component_name))
            sys.exit(2)

    files = rt.update_component(component_name)


    def update_file(file_path, contents):
        if not os.path.isfile(file_path):
            new_files[file_path] = contents
        else:
            modified_files[file_path] = contents


    if args.update_header:
        update_file(component_file(component_name + '.h'), files[component_name + '.h'])

    if args.update_source:
        update_file(component_file(component_name + '.c'), files[component_name + '.c'])

    try:
        for folder in new_folders:
            print('NF: {}'.format(folder))
            os.makedirs(folder, exist_ok=True)

        for file_name, contents in new_files.items():
            print('N: {}'.format(file_name))
            with open(file_name, 'w+') as file:
                file.write(contents)


        def modify_file(fn, modified_contents):
            shutil.copy(fn, fn + ".bak")
            change_file(fn, modified_contents)

            if args.cleanup:
                os.remove(fn + ".bak")


        for file_name, contents in modified_files.items():
            print('C: {}'.format(file_name))
            modify_file(file_name, contents)

    except Exception:
        def delete(path):
            try:
                os.remove(path)
            except FileNotFoundError:
                pass


        for file_name in new_files:
            delete(file_name)

        for file_name in modified_files:
            delete(file_name)
            shutil.move(file_name + ".bak", file_name)

        for folder in new_folders:
            shutil.rmtree(folder)

        raise
