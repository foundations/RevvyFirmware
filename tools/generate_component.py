import argparse
import json
import sys
import os
import datetime
import shutil
import pystache

from tools.generator_common import compact_project_config, to_underscore, collect_type_aliases, TypeCollection, \
    change_file, create_empty_component_data, dict_to_pystache_list
from tools.plugins.AtmelStudioSupport import atmel_studio_support
from tools.plugins.BuiltinTypes import builtin_types
from tools.plugins.ComponentConfigCompactor import component_config_compactor, process_runnable_defs
from tools.plugins.ProjectConfigCompactor import project_config_compactor
from tools.runtime import Runtime

argument_template = '{{type}} {{name}}{{^last}}, {{/last}}'
argument_list_template = '{{#args}}' + argument_template + '{{/args}}{{^args}}void{{/args}}'
fn_header_template = '{{return_type}} {{name}}(' + argument_list_template + ')'

typedef_template = """{{ #aliased }}
typedef {{ aliased }} {{ type_name }};
{{ /aliased }}
{{ #is_enum }}
typedef enum {
    {{ #values }}
    {{ value }}{{ ^last }},{{ /last }}
    {{ /values }}
} {{ type_name }};
{{ /is_enum }}
{{ #is_struct }}
typedef struct {
    {{ #fields }}
    {{ type }} {{ name }};
    {{ /fields }}
} {{ type_name }};
{{ /is_struct }}"""

header_template = '''#ifndef {{ guard_def }}
#define {{ guard_def }}

#ifndef {{ type_guard_def }}
#define {{ type_guard_def }}

{{ #type_includes }}
#include {{{.}}}
{{ /type_includes }}

{{ #types }}
''' + typedef_template + '''
{{ /types }}

#endif /* {{ type_guard_def }} */

{{#functions}}
''' + fn_header_template + ''';
{{/functions}}

#endif /* {{ guard_def }} */
'''

source_template = '''#include "{{ component_name }}.h"
{{#functions}}{{#weak}}
__attribute__((weak)){{/weak}}
''' + fn_header_template + '''
{
    {{#unused_args}}
    (void) {{ . }};
    {{/unused_args}}
    {{#arg_values}}
    *{{name}} = {{{value}}};
    {{/arg_values}}
    {{#return_value}}
    return {{{.}}};
    {{/return_value}}
}
{{/functions}}
'''

default_runnables = {
    'OnInit': {}
}


def convert_functions(component_name, runnable_data, port_data, type_data: TypeCollection):
    functions = []

    def default_value(type_name, given_value):
        if given_value is None:
            default = type_data.default_value(type_name)
            if type_data[type_name]['type'] == TypeCollection.STRUCT:
                field_default_strs = ['.{} = {}'.format(field, default[field]) for field in default]
                return '({}) {{ {} }}'.format(type_name, ", ".join(field_default_strs))

            else:
                return default

        return given_value

    for rn_name, runnable in runnable_data.items():
        return_type = runnable['return_type']
        functions.append({
            'name':         '{}_Run_{}'.format(component_name, rn_name),
            'return_type':  return_type,
            'return_value': default_value(return_type, runnable.get('return_value', None)),
            'args':         dict_to_pystache_list(runnable['arguments'],
                                                  key_name='name',
                                                  value_name='type',
                                                  last_key='last')
        })

    for port_name, port in port_data.items():
        port_type = port['port_type']

        data_type = port.get('data_type', 'void')
        passed_by_value = type_data.passed_by(data_type) == TypeCollection.PASS_BY_VALUE
        port_data_templates = {
            "WriteData":
                lambda: {
                    "name":        "{}_Write_{}",
                    "return_type": "void",
                    "arguments":   {'value': data_type if passed_by_value else "const {}*".format(data_type)},
                    "weak":        True
                },
            "WriteIndexedData":
                lambda: {
                    "name":        "{}_Write_{}",
                    "return_type": "void",
                    "arguments":   {
                        'index': 'uint32_t',
                        'value': data_type if passed_by_value else "const {}*".format(data_type)
                    },
                    "weak":        True
                },
            "ReadValue":
                lambda: {
                    "name":         "{}_Read_{}",
                    "return_type":  data_type,
                    "return_value": default_value(data_type, port.get('default_value')),
                    "arguments":    {},
                    "weak":         True
                } if passed_by_value else {
                    "name":                "{}_Read_{}",
                    "return_type":         'void',
                    "arguments":           {'value': "{}*".format(data_type)},
                    "out_argument_values": {'value': default_value(data_type, port.get('default_value'))},
                    "weak":                True
                },
            "ReadQueuedValue":
                lambda: {
                    "name":                "{}_Read_{}",
                    "return_type":         'QueueStatus_t',
                    "return_value":        default_value('QueueStatus_t', None),
                    "arguments":           {'value': "{}*".format(data_type)},
                    "out_argument_values": {'value': default_value(data_type, port.get('default_value'))},
                    "weak":                True
                },
            "ReadIndexedValue":
                lambda: {
                    "name":         "{}_Read_{}",
                    "return_type":  data_type,
                    "return_value": default_value(data_type, port.get('default_value')),
                    "arguments":    {'index': 'uint32_t'},
                    "weak":         True
                } if passed_by_value else {
                    "name":                "{}_Read_{}",
                    "return_type":         'void',
                    "arguments":           {'index': 'uint32_t', 'value': "{}*".format(data_type)},
                    "out_argument_values": {'value': default_value(data_type, port.get('default_value'))},
                    "weak":                True
                },
            "Constant":
                lambda: {
                    "name":         "{}_Constant_{}",
                    "return_type":  data_type,
                    "return_value": port['value'],
                    "arguments":    {},
                    "weak":         False
                } if passed_by_value else {
                    "name":                "{}_Constant_{}",
                    "return_type":         'void',
                    "arguments":           {'value': "{}*".format(data_type)},
                    "out_argument_values": {'value': port['value']},
                    "weak":                False
                },
            "Event":
                lambda: {
                    "name":        "{}_Call_{}",
                    "return_type": "void",
                    "arguments":   port.get('arguments', {}),
                    "weak":        True
                },
            "ServerCall":
                lambda: {
                    "name":        "{}_Call_{}",
                    "return_type": port.get('return_type', 'void'),
                    "arguments":   port.get('arguments', {}),
                    "weak":        True
                }
        }

        data = port_data_templates[port_type]()
        out_args = data.get('out_argument_values', {})

        arguments = dict_to_pystache_list(data['arguments'], key_name='name', value_name='type', last_key='last')
        out_arguments = dict_to_pystache_list(out_args, key_name='name', value_name='value')

        argument_names = data['arguments'].keys()
        used_argument_names = out_args.keys()

        unused_arguments = argument_names - used_argument_names

        port_function_data = {
            'name':         data['name'].format(component_name, port_name),
            'return_type':  data['return_type'],
            'return_value': data.get('return_value', ''),
            'args':         arguments,
            'unused_args':  unused_arguments,
            'arg_values':   out_arguments,
            'weak':         data['weak']
        }
        functions.append(port_function_data)

    return functions


def collect_used_types(functions, component_types, type_data: TypeCollection):
    types = set()

    def add_type(type_name):
        sanitized_name = type_name.replace('const', '').replace('*', '').replace(' ', '')
        resolved_name = type_data.resolve(sanitized_name)
        resolved_type = type_data[sanitized_name]

        if type_data.get(sanitized_name)['type'] == TypeCollection.ALIAS:
            types.add(sanitized_name)

        types.add(resolved_name)
        if resolved_type['type'] == TypeCollection.STRUCT:
            for field in resolved_type['fields']:
                add_type(resolved_type['fields'][field])

    for fun in functions:
        add_type(fun['return_type'])
        for arg in fun['args']:
            add_type(arg['type'])

    for type_name in component_types:
        add_type(type_name)

    return sorted(types)


def collect_includes(used_types, type_data: TypeCollection):
    includes = set()

    def add_type(type_name):
        sanitized_name = type_name.replace('const', '').replace('*', '').replace(' ', '')
        resolved_type = type_data[sanitized_name]

        if resolved_type['type'] == TypeCollection.EXTERNAL_DEF:
            if resolved_type['defined_in'] is not None:
                includes.add(resolved_type['defined_in'])

        elif resolved_type['type'] == TypeCollection.STRUCT:
            for field in resolved_type['fields'].values():
                add_type(field)

    for used in used_types:
        add_type(used)

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
    rt.add_plugin(builtin_types())
    rt.add_plugin(atmel_studio_support())

    rt.load(False)
    project_config = rt._project_config


    def component_file(filename):
        return "{}/{}/{}".format(project_config['settings']['components_folder'], component_name, filename)


    new_folders = []
    new_files = {}
    modified_files = {}

    type_data = TypeCollection()

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
            runnables = component_config['runnables']
            ports = component_config['ports']
            component_types = component_config['types']
        except FileNotFoundError:
            print("Component {} does not exists. Did you mean to --create?".format(component_name))
            sys.exit(2)

    for builtin_type, type_info in project_config['types'].items():
        type_data.add(builtin_type, type_info)

    for new_type, type_info in component_types.items():
        type_data.add(new_type, type_info)

    functions = convert_functions(component_name, runnables, ports, type_data)
    used_types = collect_used_types(functions, component_types, type_data)
    template_ctx = {
        'component_name': component_name,
        'type_includes':  collect_includes(used_types, type_data),
        'guard_def':      'COMPONENT_{}_H_'.format(to_underscore(component_name).upper()),
        'type_guard_def': 'COMPONENT_TYPES_{}_H_'.format(to_underscore(component_name).upper()),
        'date':           datetime.datetime.now().strftime("%Y. %m. %d"),
        'functions':      functions,
        'types':          collect_type_aliases(used_types, type_data)
    }


    def update_file(file_path, contents):
        if not os.path.isfile(file_path):
            new_files[file_path] = contents
        else:
            modified_files[file_path] = contents


    if args.update_header:
        update_file(component_file(component_name + '.h'), pystache.render(header_template, template_ctx))

    if args.update_source:
        update_file(component_file(component_name + '.c'), pystache.render(source_template, template_ctx))

    # noinspection PyBroadException
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
