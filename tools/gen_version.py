#!/usr/bin/python3
import os
import argparse

from tools.generator_common import change_file

file = "Config/fw_version.h"
template = """
/* This file is generated during the build process */
#ifndef FW_VERSION
#define FW_VERSION "{{FW_VERSION}}"
#define FW_VERSION_NUMBER {{FW_VERSION}}
#endif
"""

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--version', default='0', help='Patch version number to be generated')
    args = parser.parse_args()
    
    if args.version == '0':
        version = os.popen('git rev-list --count HEAD').read()
        version = version.strip()
    else:
        version = args.version

    if change_file(file, template.replace("{{FW_VERSION}}", version), delete_backup=True):
        print('Version file updated to {}'.format(version))
    else:
        print('Version file up to date, version {}'.format(version))
