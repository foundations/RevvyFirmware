#!/usr/bin/python3

# loosely based on https://gist.github.com/jeremy-prater/94de3abd1e4fbe1119f3ea3217653af5

import os
import re
import shutil

print("Converting makefile")

if not os.path.isdir("Build"):
    os.mkdir("Build")

with open("Debug/Makefile", "r") as src:
    with open("Build/Makefile", "w") as dst:
        text = src.read()

        text = re.sub('C:\\\\.*\\\\bin\\\\', '', text)
        text = text.replace('.exe', '')
        text = text.replace('SHELL := cmd', '')
        text = text.replace('C:\\Program Files (x86)\\Atmel\\Studio\\7.0\\Packs\\Atmel\\SAMD51_DFP\\1.1.96\\samd51a\\include', 'tmp/dfp/samd51a/include')
        text = text.replace('C:\\Program Files (x86)\\Atmel\\Studio\\7.0\\Packs\\arm\\cmsis\\5.0.1\\CMSIS\\Include', 'tmp/cmsis/CMSIS/Include')
        text = text.replace('@echo Building file: $<', '@echo Building file: $<\n\tmkdir -p $(@D)')
        text = text.replace('\\\\', '/')

        dst.write(text)

with open("Debug/makedep.mk", "r") as src:
    with open("Build/makedep.mk", "w") as dst:
        text = src.read()

        text = text.replace('\\', '/')

        dst.write(text)

print("Makefile placed in Build/")
