import argparse
import json
import sys
import os
import datetime
import shutil
from xml.etree import ElementTree
import pystache

from tools.generator_common import component_file_pattern, \
    component_folder_pattern, process_runnable_defs, load_component_config, load_project_config, compact_project_config, \
    to_underscore, collect_type_aliases, TypeCollection

argument_template = '{{type}} {{name}}{{^last}}, {{/last}}'
argument_list_template = '{{#args}}' + argument_template + '{{/args}}{{^args}}void{{/args}}'
fn_header_template = '{{return_type}} {{ component_name }}_{{name}}(' + argument_list_template + ')'

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
    (void) {{name}};
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

include_pattern = '#include "components/{0}/{0}.h"'
init_fn_call_pattern = "{}_Run_OnInit();"

makefile_component_files_start_marker = '# Software Component Source Files\n'
makefile_component_files_end_marker = '# End of Software Component Source Files\n'

default_runnables = {
    'OnInit': {}
}


def read_cproject(path):
    with open(path, 'r') as xml:
        xml_in = xml.read().replace('\xef\xbb\xbf', '')  # remove Byte order mark
        # preprocessing
        xml_in = xml_in.replace('<Project DefaultTargets="Build" '
                                'xmlns="http://schemas.microsoft.com/developer/msbuild/2003" '
                                'ToolsVersion="14.0">',
                                '<Project DefaultTargets="Build" ToolsVersion="14.0">')
    ElementTree.register_namespace('', 'http://schemas.microsoft.com/developer/msbuild/2003')

    return ElementTree.fromstring(xml_in)


def convert_functions(runnable_data, port_data, type_data: TypeCollection):
    functions = []

    def default_value(type_name, given_value):
        if given_value is None:
            resolved = type_data[type_name]
            if resolved['type'] == 'struct':
                field_defaults = {field: default_value(resolved['fields'][field], None) for field in resolved['fields']}
                field_default_strs = ['.{} = {}'.format(field, field_defaults[field]) for field in field_defaults]
                return '({}) {{ {} }}'.format(type_name, ", ".join(field_default_strs))

            else:
                return resolved['default_value']

        return given_value

    for runnable in runnable_data:
        runnable_arguments = runnable_data[runnable]['arguments']
        arguments = [{'name': arg, 'type': runnable_arguments[arg]} for arg in runnable_arguments]

        if arguments:
            arguments[len(arguments) - 1]['last'] = True

        return_type = runnable_data[runnable]['return_type']
        functions.append({
            'name':         'Run_{}'.format(runnable),
            'return_type':  return_type,
            'return_value': default_value(return_type, runnable_data[runnable].get('return_value', None)),
            'args':         arguments
        })

    for port in port_data:
        port_type = port_data[port]['port_type']

        data_type = port_data[port].get('data_type', 'void')
        port_data_templates = {
            "WriteData":
                lambda: {
                    "name":         "Write_{}",
                    "return_type":  "void",
                    "return_value": "",
                    "arguments":    [{'name': 'value', 'type': data_type}],
                    "weak":         True
                } if type_data[data_type]['pass_semantic'] == 'value' else {
                    "name":         "Write_{}",
                    "return_type":  'void',
                    "return_value": "",
                    "arguments":    [{'name': 'value', 'type': "const {}*".format(data_type)}],
                    "weak":         True
                },
            "WriteIndexedData":
                lambda: {
                    "name":         "Write_{}",
                    "return_type":  "void",
                    "return_value": "",
                    "arguments":    [
                        {'name': 'index', 'type': 'uint32_t'},
                        {'name': 'value', 'type': data_type}
                    ],
                    "weak":         True
                },
            "ReadValue":
                lambda: {
                    "name":         "Read_{}",
                    "return_type":  data_type,
                    "return_value": default_value(data_type, port_data[port]['default_value']),
                    "arguments":    [],
                    "weak":         True
                } if type_data[data_type]['pass_semantic'] == 'value' else {
                    "name":                "Read_{}",
                    "return_type":         'void',
                    "return_value":        "",
                    "arguments":           [{'name': 'value', 'type': "{}*".format(data_type)}],
                    "out_argument_values": [
                        {'name': 'value', 'value': default_value(data_type, port_data[port]['default_value'])}],
                    "weak":                True
                },
            "ReadIndexedValue":
                lambda: {
                    "name":         "Read_{}",
                    "return_type":  data_type,
                    "return_value": port_data[port]['default_value'],
                    "arguments":    [{'name': 'index', 'type': 'uint32_t'}],
                    "weak":         True
                },
            "Constant":
                lambda: {
                    "name":         "Constant_{}",
                    "return_type":  data_type,
                    "return_value": port_data[port]['value'],
                    "arguments":    [],
                    "weak":         False
                } if type_data[data_type]['pass_semantic'] == 'value' else {
                    "name":                "Constant_{}",
                    "return_type":         'void',
                    "return_value":        "",
                    "arguments":           [{'name': 'value', 'type': "{}*".format(data_type)}],
                    "out_argument_values": [{'name': 'value', 'value': port_data[port]['value']}],
                    "weak":                False
                },
            "Event":
                lambda: {
                    "name":         "Call_{}",
                    "return_type":  "void",
                    "return_value": "",
                    "arguments":    [],
                    "weak":         True
                }
        }

        data = port_data_templates[port_type]()

        unused_arguments = []
        out_arg_values = data.get('out_argument_values', [])
        for arg in data['arguments']:
            found = False
            for out in out_arg_values:
                # noinspection PyTypeChecker
                if arg['name'] == out['name']:
                    found = True
            if not found:
                unused_arguments.append(arg)

        port_function_data = {
            'name':         data['name'].format(port),
            'return_type':  data['return_type'],
            'return_value': data['return_value'],
            'args':         data['arguments'],
            'unused_args':  unused_arguments,
            'arg_values':   out_arg_values,
            'weak':         data['weak']
        }

        if port_function_data['args']:
            port_function_data['args'][len(port_function_data['args']) - 1]['last'] = True

        functions.append(port_function_data)

    return functions


def collect_includes(runnable_data, port_data, component_types, type_data: TypeCollection):
    includes = set()

    def add_type(type_name):
        sanitized_name = type_name.replace('const', '').replace('*', '').replace(' ', '')
        resolved_type = type_data[sanitized_name]

        if resolved_type['type'] == 'external_type_def':
            includes.add(resolved_type['defined_in'])
        elif resolved_type['type'] == 'struct':
            for field in resolved_type['fields']:
                add_type(resolved_type['fields'][field])

    for runnable in runnable_data:
        runnable_arguments = runnable_data[runnable]['arguments']
        for arg in runnable_arguments:
            add_type(runnable_arguments[arg])

    for port in port_data:
        try:
            add_type(port_data[port]['data_type'])
        except KeyError:
            pass

    for type_name in component_types:
        add_type(type_name)

    return sorted(includes)


def create_component_config(name, sources, runnables):
    json_contents = {
        'component_name': name,
        'source_files':   sources,
        'runnables':      process_runnable_defs(name, runnables)
    }
    return json.dumps(json_contents, indent=4)


def add_component_to_cproject(cproject, new_files, new_folders):
    # update Atmel Studio project xml
    tree = read_cproject(cproject)
    itemgroups = tree.findall('./ItemGroup')

    # add new files to Compile itemgroup
    compile_itemgroup = itemgroups[0]
    for filename in new_files:
        new_compile_item = ElementTree.SubElement(compile_itemgroup, 'Compile')
        new_compile_item.attrib = {'Include': filename.replace('/', '\\')}
        ElementTree.SubElement(new_compile_item, 'SubType').text = 'compile'

    # add new folder to folders itemgroup
    folders_itemgroup = itemgroups[1]
    for folder in new_folders:
        new_folder = ElementTree.SubElement(folders_itemgroup, 'Folder')
        new_folder.attrib = {'Include': folder.replace('/', '\\')}

    # generate xml string
    xml = ElementTree.tostring(tree, encoding='utf8')

    # postprocess to better match atmel's
    return xml.decode('utf8') \
        .replace("<?xml version='1.0' encoding='utf8'?>", '<?xml version="1.0" encoding="utf-8"?>') \
        .replace('<Project DefaultTargets="Build" ToolsVersion="14.0">',
                 '<Project DefaultTargets="Build" '
                 'xmlns="http://schemas.microsoft.com/developer/msbuild/2003" '
                 'ToolsVersion="14.0">') \
        .replace('      <framework-data>', '      <framework-data xmlns="">') \
        .replace('      <AcmeProjectConfig>', '      <AcmeProjectConfig xmlns="">')


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


    def component_file(filename):
        return component_file_pattern.format(component_name, filename)


    project_config = load_project_config('project.json')

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
        component_dir = component_folder_pattern.format(component_name)
        new_folders.append(component_dir)

        # create component configuration json
        runnables = process_runnable_defs(component_name, default_runnables)
        ports = {}
        component_types = {}
        new_files[config_json_path] = create_component_config(component_name, [component_name + '.c'], runnables)

        # replace sources list with new one and set for file modification
        modified_files['project.json'] = json.dumps(compact_project_config(project_config), indent=4)
        modified_files['rrrc_samd51.cproj'] = add_component_to_cproject('rrrc_samd51.cproj', new_files, new_folders)
    else:
        try:
            component_config = load_component_config(config_json_path)
            runnables = component_config['runnables']
            ports = component_config['ports']
            component_types = component_config['types']
        except FileNotFoundError:
            print("Component {} does not exists. Did you mean to --create?".format(component_name))
            sys.exit(2)

    for builtin_type in project_config['types']:
        type_data.add(builtin_type, project_config['types'][builtin_type])

    for new_type in component_types:
        type_data.add(new_type, component_types[new_type])

    template_ctx = {
        'component_name': component_name,
        'type_includes':  collect_includes(runnables, ports, component_types, type_data),
        'guard_def':      'COMPONENT_{}_H_'.format(to_underscore(component_name).upper()),
        'type_guard_def': 'COMPONENT_TYPES_{}_H_'.format(to_underscore(component_name).upper()),
        'date':           datetime.datetime.now().strftime("%Y. %m. %d"),
        'functions':      convert_functions(runnables, ports, type_data),
        'types':          collect_type_aliases(component_types, type_data)
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

        for file_name in new_files:
            print('N: {}'.format(file_name))
            with open(file_name, 'w+') as file:
                file.write(new_files[file_name])


        def modify_file(fn, modified_contents):
            shutil.copy(fn, fn + ".bak")

            with open(fn, "w") as f:
                f.write(modified_contents)

            if args.cleanup:
                os.remove(fn + ".bak")


        for file_name in modified_files:
            print('C: {}'.format(file_name))
            modify_file(file_name, modified_files[file_name])

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
