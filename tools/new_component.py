import argparse
import re
import sys

if __name__ == "__main__":
    # inquire name of new component
    parser = argparse.ArgumentParser()
    parser.add_argument('name', help='Component name')
    
    args = parser.parse_args()

    start_marker = '# Software Component Source Files\n'
    end_marker = '# End of Software Component Source Files\n'
    file_pattern = 'rrrc/components/{}/{}'

    # gather software component names
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
        if args.name in component_sources:
            print('Component already exists')
            sys.exit(1)

    # TODO: create component files

    component_sources[args.name] = ["{}.c".format(args.name)]

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
