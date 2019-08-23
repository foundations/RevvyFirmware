import argparse
import re
import sys
import os
import datetime
import getpass

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


def to_underscore(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()


if __name__ == "__main__":
    # inquire name of new component
    parser = argparse.ArgumentParser()
    parser.add_argument('name', help='Component name')

    args = parser.parse_args()

    start_marker = '# Software Component Source Files\n'
    end_marker = '# End of Software Component Source Files\n'
    dir_pattern = 'rrrc/components/{}'
    file_pattern = dir_pattern + '/{}'

    guard_def = 'COMPONENT_{}_H_'.format(to_underscore(args.name).upper())

    # gather software component names
    new_component_name = args.name
    with open('Makefile', 'r') as makefile:
        contents = makefile.readlines()

        start = contents.index(start_marker)
        end = contents.index(end_marker)

        component_sources = {}

        for file in contents[start+2:end]:
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
    worklogic_file_path = 'rrrc/rrrc_worklogic.c'
    worklogic_header_path = 'rrrc/rrrc_worklogic.h'
    component_dir = dir_pattern.format(new_component_name)
    h_file_path = file_pattern.format(new_component_name, new_component_name + '.h')
    c_file_path = file_pattern.format(new_component_name, new_component_name + '.c')
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

    with open(worklogic_file_path, "r") as worklogic:
        c = worklogic.read()

        def prepend_init_fn(match):
            return match.group(1) + "{}_Run_OnInit();".format(new_component_name) + "\n" + match.group(0)

        m = re.sub('([ \\t]*)/\\* end of component initializers \\*/', prepend_init_fn, c)

    with open(worklogic_file_path, "w") as worklogic:
        worklogic.write(m)

    with open(worklogic_header_path, "r") as worklogic:
        c = worklogic.read()

        def prepend_include(match):
            return match.group(1) + "#include \"components/{0}/{0}.h\"".format(new_component_name) + "\n" + match.group(0)

        m = re.sub('([ \\t]*)/\\* end of component includes \\*/', prepend_include, c)

    with open(worklogic_header_path, "w") as worklogic:
        worklogic.write(m)

    component_sources[new_component_name] = ["{}.c".format(new_component_name)]

    contents_str = "".join(contents[0:start+1]) + "".join(contents[end:])

    # assemble new sources list
    new_file_list_str = start_marker + "C_SRCS +="
    for component in sorted(component_sources.keys()):
        for file in component_sources[component]:
            new_file_list_str += " \\\n" + file_pattern.format(component, file)

    new_file_list_str += "\n" + end_marker

    # replace sources list with new one
    new_contents = contents_str.replace(start_marker+end_marker, new_file_list_str)

    with open('Makefile', 'w') as makefile:
        makefile.write(new_contents)
