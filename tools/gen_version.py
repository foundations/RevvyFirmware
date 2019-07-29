#!/usr/bin/python3
import os

file = "../config/fw_version.h"
template = """
/* This file is generated during the build process */
#ifndef FW_VERSION
#define FW_VERSION "{{FW_VERSION}}"
#define FW_VERSION_NUMBER {{FW_VERSION}}
#endif
"""

version = os.popen('git rev-list --count HEAD').read()
version = version.strip()
with open(file, 'w') as out:
    out.write(template.replace("{{FW_VERSION}}", version))
