import argparse
import re
import sys
import os
import datetime
import shutil
import xml.etree.ElementTree as ET

header_template = '''#ifndef {{GUARD_DEF}}
#define {{GUARD_DEF}}

void {{COMPONENT_NAME}}_Run_OnInit(void);

#endif /* {{GUARD_DEF}} */
'''

source_template = '''#include "{{COMPONENT_NAME}}.h"

void {{COMPONENT_NAME}}_Run_OnInit(void)
{

}
'''

dir_pattern = 'rrrc/components/{}'
file_pattern = 'rrrc/components/{}/{}'
include_pattern = '#include "components/{0}/{0}.h"'
init_fn_call_pattern = "{}_Run_OnInit();"

makefile_component_files_start_marker = '# Software Component Source Files\n'
makefile_component_files_end_marker = '# End of Software Component Source Files\n'

worklogic_file_path = 'rrrc/rrrc_worklogic.c'
worklogic_header_path = 'rrrc/rrrc_worklogic.h'


def to_underscore(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()


def create_files(component_name):
    h_file_path = file_pattern.format(component_name, component_name + '.h')
    c_file_path = file_pattern.format(component_name, component_name + '.c')

    guard_def = 'COMPONENT_{}_H_'.format(to_underscore(component_name).upper())

    def format_template(template):
        return template\
            .replace('{{COMPONENT_NAME}}', component_name)\
            .replace('{{GUARD_DEF}}', guard_def)\
            .replace('{{DATE}}', datetime.datetime.now().strftime("%Y. %m. %d"))

    return {
        h_file_path: format_template(header_template),
        c_file_path: format_template(source_template)
    }


def add_include(component_name, file):
    with open(file, "r") as worklogic:
        c = worklogic.read()

    def prepend_include(match):
        return match.group(1) + include_pattern.format(component_name) + "\n" + match.group(0)

    return re.sub('([ \\t]*)/\\* end of component includes \\*/', prepend_include, c)


def add_initializer_call(component_name, file):
    with open(file, "r") as worklogic:
        c = worklogic.read()

    def prepend_init_fn(match):
        return match.group(1) + init_fn_call_pattern.format(component_name) + "\n" + match.group(0)

    return re.sub('([ \\t]*)/\\* end of component initializers \\*/', prepend_init_fn, c)


def read_cproject(path):
    with open(path, 'r') as xml:
        xml_in = xml.read().replace('\xef\xbb\xbf', '')  # remove Byte order mark
        # preprocessing
        xml_in = xml_in.replace('<Project DefaultTargets="Build" '
                                'xmlns="http://schemas.microsoft.com/developer/msbuild/2003" '
                                'ToolsVersion="14.0">',
                                '<Project DefaultTargets="Build" ToolsVersion="14.0">')
    ET.register_namespace('', 'http://schemas.microsoft.com/developer/msbuild/2003')

    return ET.fromstring(xml_in)


def create_component(component_name, dry_run=False):

    with open('Makefile', 'r') as makefile:
        contents = makefile.readlines()

        start = contents.index(makefile_component_files_start_marker)
        end = contents.index(makefile_component_files_end_marker)

        component_sources = {}

        for mk_file in contents[start + 2:end]:
            mk_match = re.match('rrrc/components/(?P<component>[^/]+?)/(?P<file>[^ \n]+)', mk_file)
            try:
                component_sources[mk_match.group('component')].append(mk_match.group('file'))
            except KeyError:
                component_sources[mk_match.group('component')] = [mk_match.group('file')]

    # stop if component exists
    if component_name in component_sources:
        print('Component already exists')
        sys.exit(1)

    # list of new folders' paths
    new_folders = []

    # map of original file path -> new contents
    modified_files = {}

    # Create component skeleton
    component_dir = dir_pattern.format(component_name)
    new_folders.append(component_dir)

    # map of file path -> contents
    new_files = create_files(component_name)

    # noinspection PyBroadException
    try:
        modified_files[worklogic_header_path] = add_include(component_name, worklogic_header_path)
        modified_files[worklogic_file_path] = add_initializer_call(component_name, worklogic_file_path)

        component_sources[component_name] = ["{}.c".format(component_name)]

        contents_str = "".join(contents[0:start + 1]) + "".join(contents[end:])

        # assemble new sources list
        new_file_list_str = makefile_component_files_start_marker + "C_SRCS +="
        for component in sorted(component_sources.keys()):
            for component_file in component_sources[component]:
                new_file_list_str += " \\\n" + file_pattern.format(component, component_file)

        new_file_list_str += "\n" + makefile_component_files_end_marker

        # replace sources list with new one and set for file modification
        modified_files['Makefile'] = contents_str.replace(
            makefile_component_files_start_marker + makefile_component_files_end_marker,
            new_file_list_str)

        # update Atmel Studio project xml
        tree = read_cproject('rrrc_samd51.cproj')

        itemgroups = tree.findall('./ItemGroup')

        # add new files to Compile itemgroup
        compile_itemgroup = itemgroups[0]

        for file_name in new_files:
            new_compile_item = ET.SubElement(compile_itemgroup, 'Compile')
            new_compile_item.attrib = {'Include': file_name.replace('/', '\\')}
            ET.SubElement(new_compile_item, 'SubType').text = 'compile'

        # add new folder to folders itemgroup
        folders_itemgroup = itemgroups[1]

        for folder in new_folders:
            new_folder = ET.SubElement(folders_itemgroup, 'Folder')
            new_folder.attrib = {'Include': folder.replace('/', '\\')}

        # generate xml string
        xml = ET.tostring(tree, encoding='utf8')

        # postprocess to better match atmel's
        xml = xml.decode('utf8') \
            .replace("<?xml version='1.0' encoding='utf8'?>", '<?xml version="1.0" encoding="utf-8"?>') \
            .replace('<Project DefaultTargets="Build" ToolsVersion="14.0">',
                     '<Project DefaultTargets="Build" '
                     'xmlns="http://schemas.microsoft.com/developer/msbuild/2003" '
                     'ToolsVersion="14.0">') \
            .replace('      <framework-data>', '      <framework-data xmlns="">') \
            .replace('      <AcmeProjectConfig>', '      <AcmeProjectConfig xmlns="">')

        modified_files['rrrc_samd51.cproj'] = xml

        if not dry_run:
            for folder in new_folders:
                os.makedirs(folder, exist_ok=True)

            for file_name in new_files:
                with open(file_name, 'w+') as file:
                    file.write(new_files[file_name])

            # apply changed
            def modify_file(fn, modified_contents):
                shutil.copy(fn, fn + ".bak")

                with open(fn, "w") as f:
                    f.write(modified_contents)

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
        def delete(file):
            try:
                os.remove(file)
            except FileNotFoundError:
                pass

        for file_name in new_files:
            delete(file_name)

        for file_name in modified_files:
            delete(file_name)
            shutil.move(file_name + ".bak", file_name)

        for folder in new_folders:
            shutil.rmtree(folder)


if __name__ == "__main__":
    # inquire name of new component
    parser = argparse.ArgumentParser()
    parser.add_argument('name', help='Component name')
    parser.add_argument('--dry-run', help='Don\'t execute changes', action='store_true')

    args = parser.parse_args()

    # gather software component names
    create_component(args.name, dry_run=args.dry_run)
