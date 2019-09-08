import argparse
import json
import re
import sys
import os
import datetime
import shutil
from xml.etree import ElementTree
import pystache

from tools.generator_common import type_includes, type_default_values

argument_template = '{{type}} {{name}}{{^last}}, {{/last}}'
argument_list_template = '{{#args}}' + argument_template + '{{/args}}{{^args}}void{{/args}}'
fn_header_template = '{{return_type}} {{ component_name }}_{{name}}(' + argument_list_template + ')'

header_template = '''#ifndef {{ guard_def }}
#define {{ guard_def }}

{{#includes}}
#include {{{.}}}
{{/includes}}

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
    {{#args}}
    (void) {{name}};
    {{/args}}
    {{#return_value}}
    return {{{.}}};
    {{/return_value}}
}
{{/functions}}
'''

dir_pattern = 'rrrc/components/{}'
file_pattern = 'rrrc/components/{}/{}'
include_pattern = '#include "components/{0}/{0}.h"'
init_fn_call_pattern = "{}_Run_OnInit();"

makefile_component_files_start_marker = '# Software Component Source Files\n'
makefile_component_files_end_marker = '# End of Software Component Source Files\n'

default_runnables = {
    'OnInit': {
        'return_type': 'void',
        'arguments':   {}
    }
}


def to_underscore(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()


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


def convert_functions(runnable_data, port_data):
    functions = []

    for runnable in runnable_data:
        runnable_arguments = runnable_data[runnable]['arguments']
        arguments = [{'name': arg, 'type': runnable_arguments[arg]} for arg in runnable_arguments]

        if arguments:
            arguments[len(arguments) - 1]['last'] = True

        functions.append({
            'name':        'Run_{}'.format(runnable),
            'return_type': runnable_data[runnable]['return_type'],
            'args':        arguments
        })

    for port in port_data:
        port_type = port_data[port]['port_type']
        data_type = port_data[port]['data_type']

        port_data_templates = {
            "WriteData": {
                "name":         "Write_{}",
                "return_type":  "void",
                "return_value": "",
                "arguments":    [{'name': 'value', 'type': data_type}]
            },
            "ReadValue": {
                "name":         "Read_{}",
                "return_type":  data_type,
                "return_value": port_data[port].get('default_value', type_default_values[data_type]),
                "arguments":    []
            }
        }

        port_function_data = {
            'name':         port_data_templates[port_type]['name'].format(port),
            'return_type':  port_data_templates[port_type]['return_type'],
            'return_value': port_data_templates[port_type]['return_value'],
            'args':         port_data_templates[port_type]['arguments'],
            'weak':         True
        }

        if port_data_templates[port_type]['arguments']:
            port_function_data['args'][len(port_function_data['args']) - 1]['last'] = True

        functions.append(port_function_data)

    return functions


def collect_includes(runnable_data, port_data):
    includes = set()

    for runnable in runnable_data:
        runnable_arguments = runnable_data[runnable]['arguments']
        for arg in runnable_arguments:
            try:
                includes.add(type_includes[runnable_arguments[arg]])
            except KeyError:
                pass

    for port in port_data:
        try:
            includes.add(type_includes[port_data[port]['data_type']])
        except KeyError:
            pass

    return list(includes)


def create_component_config(name, sources, runnables):
    json_contents = {
        'component_name': name,
        'source_files':   sources,
        'runnables':      runnables
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
    parser.add_argument('--dry-run', help='Don\'t execute changes', action='store_true')
    parser.add_argument('--cleanup', help='Remove backups', action='store_true')

    """Create generates json and empty c/h pair, adds include, adds to project json, cproject"""

    args = parser.parse_args()

    # gather software component names
    component_name = args.name


    def component_file(filename):
        return file_pattern.format(component_name, filename)


    with open('project.json', 'r') as project_config:
        config = json.load(project_config)

    new_folders = []
    new_files = {}
    modified_files = {}

    config_json_path = component_file('config.json')
    if args.create:
        # stop if component exists
        if component_name in config['components']:
            print('Component already exists')
            sys.exit(1)

        config['components'].append(component_name)
        config['components'] = sorted(config['components'])

        # Create component skeleton
        component_dir = dir_pattern.format(component_name)
        new_folders.append(component_dir)

        # create component configuration json
        runnables = default_runnables
        ports = {}
        new_files[config_json_path] = create_component_config(component_name, [component_name + '.c'], runnables)

        # replace sources list with new one and set for file modification
        modified_files['project.json'] = json.dumps(config, indent=4)
        modified_files['rrrc_samd51.cproj'] = add_component_to_cproject('rrrc_samd51.cproj', new_files, new_folders)
    else:
        with open(config_json_path, 'r') as component_config_file:
            component_config = json.load(component_config_file)

        runnables = component_config['runnables']
        ports = component_config['ports']

    template_ctx = {
        'component_name': component_name,
        'includes':       collect_includes(runnables, ports),
        'guard_def':      'COMPONENT_{}_H_'.format(to_underscore(component_name).upper()),
        'date':           datetime.datetime.now().strftime("%Y. %m. %d"),
        'functions':      convert_functions(runnables, ports)
    }


    def update_file(file_path, contents):
        if not os.path.isfile(file_path):
            new_files[file_path] = contents
        else:
            modified_files[file_path] = contents


    update_file(component_file(component_name + '.h'), pystache.render(header_template, template_ctx))
    update_file(component_file(component_name + '.c'), pystache.render(source_template, template_ctx))

    # noinspection PyBroadException
    try:

        if not args.dry_run:
            for folder in new_folders:
                os.makedirs(folder, exist_ok=True)

            for file_name in new_files:
                with open(file_name, 'w+') as file:
                    file.write(new_files[file_name])


            def modify_file(fn, modified_contents):
                shutil.copy(fn, fn + ".bak")

                with open(fn, "w") as f:
                    f.write(modified_contents)

                if args.cleanup:
                    os.remove(fn + ".bak")


            for file_name in modified_files:
                modify_file(file_name, modified_files[file_name])
        else:
            print('Dry run prevented the following changes:')
            for folder in new_folders:
                print('NF: {}'.format(folder))

            for file_name in new_files:
                print('N: {}'.format(file_name))

            for file_name in modified_files:
                print('C: {}'.format(file_name))

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
