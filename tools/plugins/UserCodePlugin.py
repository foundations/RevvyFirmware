import re

from tools.runtime import RuntimePlugin, Runtime, FunctionDescriptor


def parse_file(file_path):
    """Parse the given file for user sections"""
    file_sections = {}
    try:
        with open(file_path, 'r') as f:
            contents = f.read()

        # parse contents
        matches = re.findall(
            '(?P<indent>[ ]*)/\\* Begin User Code Section: (?P<secname>.*?) \\*/\n(?P<usercode>.*?)\n(?P=indent)/\\* End User Code Section: (?P=secname) \\*/',
            contents, flags=re.DOTALL)

        for indent, secname, usercode in matches:
            indent_level = len(indent)
            # remove indentation
            if indent_level > 0:
                if usercode.startswith(indent):
                    usercode = usercode[len(indent):]

            file_sections[secname] = usercode
    except FileNotFoundError:
        pass

    return file_sections


def create_section(name, contents):
    return '/* Begin User Code Section: {0} */\n{1}/* End User Code Section: {0} */'.format(name, contents)


def add_sections_to_function(function: FunctionDescriptor, name):
    secname = name + ' Start'
    function.prepend_body(create_section(secname, ''))

    secname = name + ' End'
    function.add_body(create_section(secname, ''))


def fill_sections(source, sections):
    def repl(matches):
        indent = matches[1]
        secname = matches[2]

        lines = sections.get(secname, '').split("\n")
        proclines = []
        for line in lines:
            if line != '':
                proclines.append(indent + line)
            else:
                proclines.append(line)
        proclines.append(indent)

        return indent + create_section(secname, "\n".join(proclines))

    return re.sub(
        '(?P<indent>[ ]*)/\\* Begin User Code Section: (?P<secname>.*?) \\*/(?P<usercode>.*?)/\\* End User Code Section: (?P=secname) \\*/',
        repl,
        source,
        flags=re.DOTALL)


def add_sections_to_component(owner: Runtime, component_name, context: dict):
    context['declarations'].insert(0, create_section('Declarations', ''))

    for name, function in context['functions'].items():
        name = name[name.rfind('/') + 1:]  # don't need to have the component name
        add_sections_to_function(function, name)


def add_sections_to_runtime(owner: Runtime, context: dict):
    context['declarations'].insert(0, create_section('Declarations', ''))

    for name, function in context['functions'].items():
        if function:
            add_sections_to_function(function, name)


def replace_sections_in_files(context: dict):
    for file, source in context['files'].items():
        sections = parse_file(file)

        context['files'][file] = fill_sections(source, sections)


def user_code_plugin():
    return RuntimePlugin("UserCodePlugin", handlers={
        'before_generating_component': add_sections_to_component,
        'generating_component':        lambda owner, component_name, context: replace_sections_in_files(context),
        'before_generating_runtime':   add_sections_to_runtime,
        'after_generating_runtime':    lambda owner, context: replace_sections_in_files(context)
    })
