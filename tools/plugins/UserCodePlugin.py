import re

from tools.runtime import RuntimePlugin, Runtime


def parse_file(file_path):
    """Parse the given file for user sections"""
    file_sections = {}
    try:
        with open(file_path, 'r') as f:
            contents = f.read()

        # parse contents
        matches = re.findall(
            '/\\* Begin User Code Section: (?P<secname>.*?) \\*/(?P<usercode>.*?)/\\* End User Code Section: (?P=secname) \\*/',
            contents, re.DOTALL)

        for secname, usercode in matches:
            file_sections[secname] = usercode
    except FileNotFoundError:
        pass

    return file_sections


def format_section(section_name, contents):
    template = '/* Begin User Code Section: {0} */\n{1}\n/* End User Code Section: {0} */'
    return template.format(section_name, contents.replace('\n    ', '\n').strip('\n'))


def add_sections_to_component(owner: Runtime, component_name, context: dict):
    source_files = [file for file in context['files'].keys() if file.endswith('.c')]

    for source in source_files:
        sections = parse_file(source)

        context['declarations'].insert(0, format_section('Declarations', sections.get('Declarations', '')))

        for name, function in context['functions'].items():
            name = name[name.rfind('/')+1:]  # don't need to have the component name
            secname = name + ' Start'
            function.prepend_body(format_section(secname, sections.get(secname, '')))

            secname = name + ' End'
            function.add_body(format_section(secname, sections.get(secname, '')))


def add_sections_to_runtime(owner: Runtime, context: dict):
    source_files = [file for file in context['files'].keys() if file.endswith('.c')]

    for source in source_files:
        sections = parse_file(source)

        context['declarations'].insert(0, format_section('Declarations', sections.get('Declarations', '')))

        for name, function in context['functions'].items():
            if function:
                secname = name + ' Start'
                function.prepend_body(format_section(secname, sections.get(secname, '')))

                secname = name + ' End'
                function.add_body(format_section(secname, sections.get(secname, '')))


def user_code_plugin():
    return RuntimePlugin("UserCodePlugin", handlers={
        'before_generating_component': add_sections_to_component,
        'before_generating_runtime': add_sections_to_runtime
    })
