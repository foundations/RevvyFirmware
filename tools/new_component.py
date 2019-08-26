import argparse
import re
import sys
import os
import datetime
import getpass
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
cproj_dir_pattern = 'rrrc\\components\\{}'
cproj_file_pattern = 'rrrc\\components\\{0}\\{0}.{1}'
include_pattern = '#include "components/{0}/{0}.h"'
init_fn_call_pattern = "{}_Run_OnInit();"

makefile_component_files_start_marker = '# Software Component Source Files\n'
makefile_component_files_end_marker = '# End of Software Component Source Files\n'


def to_underscore(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()


def create_files(new_component_name):
    component_dir = dir_pattern.format(new_component_name)
    h_file_path = file_pattern.format(new_component_name, new_component_name + '.h')
    c_file_path = file_pattern.format(new_component_name, new_component_name + '.c')

    guard_def = 'COMPONENT_{}_H_'.format(to_underscore(new_component_name).upper())

    os.makedirs(component_dir, exist_ok=True)
    with open(h_file_path, "w") as h_file:
        h_file.write(header_template
                     .replace('{{COMPONENT_NAME}}', new_component_name)
                     .replace('{{GUARD_DEF}}', guard_def)
                     .replace('{{DATE}}', datetime.datetime.now().strftime("%Y. %m. %d")))

    with open(c_file_path, "w") as c_file:
        c_file.write(source_template
                     .replace('{{COMPONENT_NAME}}', new_component_name)
                     .replace('{{GUARD_DEF}}', guard_def)
                     .replace('{{DATE}}', datetime.datetime.now().strftime("%Y. %m. %d")))


def add_include(new_component_name):
    worklogic_header_path = 'rrrc/rrrc_worklogic.h'
    with open(worklogic_header_path, "r") as worklogic:
        c = worklogic.read()

        def prepend_include(match):
            return match.group(1) + include_pattern.format(new_component_name) + "\n" + match.group(0)

        m = re.sub('([ \\t]*)/\\* end of component includes \\*/', prepend_include, c)

    with open(worklogic_header_path, "w") as worklogic:
        worklogic.write(m)


def add_initializer_call(new_component_name):
    worklogic_file_path = 'rrrc/rrrc_worklogic.c'
    with open(worklogic_file_path, "r") as worklogic:
        c = worklogic.read()

        def prepend_init_fn(match):
            return match.group(1) + init_fn_call_pattern.format(new_component_name) + "\n" + match.group(0)

        m = re.sub('([ \\t]*)/\\* end of component initializers \\*/', prepend_init_fn, c)

    with open(worklogic_file_path, "w") as worklogic:
        worklogic.write(m)


def insert_action_info(element, file):
    c_file = ET.SubElement(element, 'AcmeProjectActionInfo')
    c_file.attrib = {'Action': 'File', 'Hash': '', 'IsConfig': 'true', 'Source': file}


def add_compile_item(itemgroup, file):
    new_compile_item = ET.SubElement(itemgroup, 'Compile')
    new_compile_item.attrib = {'Include': file}

    ET.SubElement(new_compile_item, 'SubType').text = 'compile'


if __name__ == "__main__":
    # inquire name of new component
    parser = argparse.ArgumentParser()
    parser.add_argument('name', help='Component name')

    args = parser.parse_args()

    # gather software component names
    new_component_name = args.name
    with open('Makefile', 'r') as makefile:
        contents = makefile.readlines()

        start = contents.index(makefile_component_files_start_marker)
        end = contents.index(makefile_component_files_end_marker)

        component_sources = {}

        for file in contents[start + 2:end]:
            m = re.match('rrrc/components/(?P<component>[^/]+?)/(?P<file>[^ \n]+)', file)
            try:
                component_sources[m.group('component')].append(m.group('file'))
            except KeyError:
                component_sources[m.group('component')] = [m.group('file')]

        # stop if component exists
        if new_component_name in component_sources:
            print('Component already exists')
            sys.exit(1)

    # Create component skeleton
    create_files(new_component_name)
    add_include(new_component_name)
    add_initializer_call(new_component_name)

    component_sources[new_component_name] = ["{}.c".format(new_component_name)]

    contents_str = "".join(contents[0:start + 1]) + "".join(contents[end:])

    # assemble new sources list
    new_file_list_str = makefile_component_files_start_marker + "C_SRCS +="
    for component in sorted(component_sources.keys()):
        for file in component_sources[component]:
            new_file_list_str += " \\\n" + file_pattern.format(component, file)

    new_file_list_str += "\n" + makefile_component_files_end_marker

    # replace sources list with new one
    new_contents = contents_str.replace(makefile_component_files_start_marker + makefile_component_files_end_marker,
                                        new_file_list_str)

    # update Atmel Studio project xml
    with open('rrrc_samd51.cproj', 'r') as xml:
        xml_in = xml.read().replace('\xef\xbb\xbf', '')  # remove Byte order mark
        # preprocessing
        xml_in = xml_in.replace('<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003" ToolsVersion="14.0">', '<Project DefaultTargets="Build" ToolsVersion="14.0">')

    ET.register_namespace('', 'http://schemas.microsoft.com/developer/msbuild/2003')
    tree = ET.fromstring(xml_in)

    # add new files to AcmeActionInfos
    action_infos = tree.findall('./PropertyGroup/AcmeProjectConfig/AcmeProjectConfig/AcmeActionInfos')[0]

    insert_action_info(action_infos, cproj_file_pattern.format(new_component_name, 'h'))
    insert_action_info(action_infos, cproj_file_pattern.format(new_component_name, 'c'))

    itemgroups = tree.findall('./ItemGroup')

    # add new files to Compile itemgroup
    compile_itemgroup = itemgroups[0]

    add_compile_item(compile_itemgroup, cproj_file_pattern.format(new_component_name, 'h'))
    add_compile_item(compile_itemgroup, cproj_file_pattern.format(new_component_name, 'c'))

    # add new folder to folders itemgroup
    folders_itemgroup = itemgroups[1]

    new_folder = ET.SubElement(folders_itemgroup, 'Folder')
    new_folder.attrib = {'Include': cproj_dir_pattern.format(new_component_name)}

    # save new cproject file
    xml = ET.tostring(tree, encoding='utf8')

    # postprocess to better match atmel's
    xml = xml.decode('utf8') \
        .replace("<?xml version='1.0' encoding='utf8'?>", '<?xml version="1.0" encoding="utf-8"?>') \
        .replace('<Project DefaultTargets="Build" ToolsVersion="14.0">', '<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003" ToolsVersion="14.0">') \
        .replace('      <framework-data>', '      <framework-data xmlns="">') \
        .replace('      <AcmeProjectConfig>', '      <AcmeProjectConfig xmlns="">')

    with open('Makefile', 'w') as makefile:
        makefile.write(new_contents)

    with open('rrrc_samd51.cproj', 'w') as f:
        f.write(xml)
