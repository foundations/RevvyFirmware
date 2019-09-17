import argparse
import json
import sys
import os
import datetime
import shutil
import chevron

from tools.generator_common import compact_project_config, to_underscore, render_typedefs, TypeCollection, \
    change_file, create_empty_component_data
from tools.plugins.AtmelStudioSupport import atmel_studio_support
from tools.plugins.BuiltinDataTypes import builtin_data_types
from tools.plugins.ComponentConfigCompactor import component_config_compactor, process_runnable_defs
from tools.plugins.ProjectConfigCompactor import project_config_compactor
from tools.plugins.RuntimeEvents import runtime_events
from tools.runtime import Runtime

header_template = '''#ifndef {{ guard_def }}
#define {{ guard_def }}

#ifndef {{ type_guard_def }}
#define {{ type_guard_def }}

{{# type_includes }}
#include {{{ . }}}
{{/ type_includes }}

{{# types }}
{{{ . }}}
{{/ types }}

#endif /* {{ type_guard_def }} */

{{# function_headers }}
{{{ . }}};
{{/ function_headers }}

#endif /* {{ guard_def }} */
'''

source_template = '''#include "{{ component_name }}.h"

{{# functions }}
{{{ . }}}
{{/ functions }}'''

default_runnables = {
    'OnInit': {}
}


def collect_used_types(functions, component_types, type_data: TypeCollection):
    types = set()

    def add_type(type_name):
        if type(type_name) is str:
            sanitized_name = type_name.replace('const', '').replace('*', '').replace(' ', '')
            resolved_name = type_data.resolve(sanitized_name)
            resolved_type = type_data[sanitized_name]

            if type_data.get(sanitized_name)['type'] == TypeCollection.ALIAS:
                types.add(sanitized_name)

            types.add(resolved_name)

            if resolved_type['type'] == TypeCollection.STRUCT:
                add_type(resolved_type['fields'].values())
        else:
            for t in type_name:
                add_type(t)

    for fun in functions:
        add_type(fun.referenced_types())

    add_type(component_types.keys())

    return sorted(types)


def collect_includes(used_types, type_data: TypeCollection):
    includes = set()

    if type(used_types) is str:
        sanitized_name = used_types.replace('const', '').replace('*', '').replace(' ', '')
        resolved_type = type_data[sanitized_name]

        if resolved_type['type'] == TypeCollection.EXTERNAL_DEF:
            if resolved_type['defined_in'] is not None:
                includes.add(resolved_type['defined_in'])

        elif resolved_type['type'] == TypeCollection.STRUCT:
            includes.update(collect_includes(resolved_type['fields'].values(), type_data))

    else:
        for t in used_types:
            includes.update(collect_includes(t, type_data))

    return sorted(includes)


def create_component_config(name, sources, runnables):
    contents = create_empty_component_data(name)
    contents['source_files'] = sources
    contents['runnables'] = process_runnable_defs(name, runnables)

    return json.dumps(contents, indent=4)


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
    rt.add_plugin(component_config_compactor())
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

    type_data = rt.types

    config_json_path = component_file('config.json')
    if args.create:
        # stop if component exists
        if component_name in project_config['components']:
            print('Component already exists')
            sys.exit(1)

        project_config['components'].append(component_name)
        project_config['components'] = sorted(project_config['components'])

        # Create component skeleton
        component_dir = "{}/{}".format(project_config['settings']['components_folder'], component_name)
        new_folders.append(component_dir)

        # create component configuration json
        runnables = process_runnable_defs(component_name, default_runnables)
        ports = {}
        component_types = {}
        new_files[config_json_path] = create_component_config(component_name, [component_name + '.c'], runnables)

        # replace sources list with new one and set for file modification
        modified_files['project.json'] = json.dumps(compact_project_config(project_config), indent=4)
    else:
        try:
            rt.load_component_config(component_name)
            component_config = rt._components[component_name]
            ports = component_config['ports']
            component_types = component_config['types']
        except FileNotFoundError:
            print("Component {} does not exists. Did you mean to --create?".format(component_name))
            sys.exit(2)

    rt.update_component(component_name, args.update_header, args.update_source)

    funcs = rt.functions.values()

    function_headers = [fn.get_header() for fn in funcs]
    functions = [fn.get_function() for fn in funcs]

    used_types = collect_used_types(rt.functions.values(), component_types, type_data)
    template_ctx = {
        'component_name':   component_name,
        'type_includes':    collect_includes(used_types, type_data),
        'guard_def':        'COMPONENT_{}_H_'.format(to_underscore(component_name).upper()),
        'type_guard_def':   'COMPONENT_TYPES_{}_H_'.format(to_underscore(component_name).upper()),
        'date':             datetime.datetime.now().strftime("%Y. %m. %d"),
        'functions':        functions,
        'function_headers': function_headers,
        'types':            render_typedefs(used_types, type_data)
    }


    def update_file(file_path, contents):
        if not os.path.isfile(file_path):
            new_files[file_path] = contents
        else:
            modified_files[file_path] = contents


    if args.update_header:
        update_file(component_file(component_name + '.h'), chevron.render(header_template, template_ctx))

    if args.update_source:
        update_file(component_file(component_name + '.c'), chevron.render(source_template, template_ctx))

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
