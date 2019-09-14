from xml.etree import ElementTree

from tools.runtime import RuntimePlugin


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


def atmel_studio_support():
    return RuntimePlugin("AtmelStudioSupport", {

    })
