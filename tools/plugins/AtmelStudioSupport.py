from xml.etree import ElementTree
import os

from tools.generator_common import list_files_recursive
from tools.runtime import RuntimePlugin, Runtime

bom = '\xef\xbb\xbf'


def read_cproject(path):
    with open(path, 'r') as xml:
        xml_in = xml.read().replace(bom, '')  # remove Byte order mark
        # preprocessing
        xml_in = xml_in.replace('<Project DefaultTargets="Build" '
                                'xmlns="http://schemas.microsoft.com/developer/msbuild/2003" '
                                'ToolsVersion="14.0">',
                                '<Project DefaultTargets="Build" ToolsVersion="14.0">')
    ElementTree.register_namespace('', 'http://schemas.microsoft.com/developer/msbuild/2003')

    return ElementTree.fromstring(xml_in)


def add_component_to_cproject(cproject, new_files, new_folders):
    # update Atmel Studio project xml
    tree = read_cproject(cproject)
    itemgroups = tree.findall('./ItemGroup')

    # add new files to Compile itemgroup
    compile_itemgroup = itemgroups[0]

    known_files = []
    for item in compile_itemgroup:
        known_files.append(item.attrib['Include'])

    new_files = set(new_files).difference(known_files)

    for filename in new_files:
        new_compile_item = ElementTree.SubElement(compile_itemgroup, 'Compile')
        new_compile_item.attrib = {'Include': filename.replace('/', '\\')}
        ElementTree.SubElement(new_compile_item, 'SubType').text = 'compile'

    # add new folder to folders itemgroup
    folders_itemgroup = itemgroups[1]

    known_folders = []
    for item in folders_itemgroup:
        known_folders.append(item.attrib['Include'])

    new_folders = set(new_folders).difference(known_folders)

    for folder in new_folders:
        new_folder = ElementTree.SubElement(folders_itemgroup, 'Folder')
        new_folder.attrib = {'Include': folder.replace('/', '\\')}

    # generate xml string
    xml = ElementTree.tostring(tree, encoding='utf8')

    # postprocess to better match atmel's
    return bom + xml.decode('utf8') \
        .replace("<?xml version='1.0' encoding='utf8'?>", '<?xml version="1.0" encoding="utf-8"?>') \
        .replace('<Project DefaultTargets="Build" ToolsVersion="14.0">',
                 '<Project DefaultTargets="Build" '
                 'xmlns="http://schemas.microsoft.com/developer/msbuild/2003" '
                 'ToolsVersion="14.0">') \
        .replace('      <framework-data>', '      <framework-data xmlns="">') \
        .replace('      <AcmeProjectConfig>', '      <AcmeProjectConfig xmlns="">')


def add_to_cproject(context, cproject_file):
    component_files = list_files_recursive(context['component_folder'])

    files = set()
    folders = set()

    for file in component_files + list(context['files'].keys()):
        file = file.replace('/', os.path.sep)
        if file.endswith('.c') or file.endswith('.h') or file.endswith('.json'):
            files.add(file)

        folders.add(os.path.dirname(file))

    context['files'][cproject_file] = add_component_to_cproject(cproject_file, files, folders)


def atmel_studio_support(cproject_file):
    return RuntimePlugin("AtmelStudioSupport", {
        'generating_component':
            lambda owner, component_name, context: add_to_cproject(context, cproject_file)
    })
