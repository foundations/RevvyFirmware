#!/usr/bin/python3
import os
import shutil
import json
from hashlib import md5


def file_hash(file):
    hash_fn = md5()
    with open(file, "rb") as f:
        hash_fn.update(f.read())
    return hash_fn.hexdigest()


# parameters
version_major = 0
version_minor = 1
hardware_versions = ["1.0.0", "1.0.1"]
build_config = "Debug"
firmware_filename = "rrrc_samd51.bin"
output_dir = "../output"

# prepare
os.makedirs(output_dir, exist_ok=True)

commit_count = os.popen('git rev-list --count HEAD').read()
version_patch = commit_count.strip()

version_str = "{}.{}.{}".format(version_major, version_minor, version_patch)
firmware_file = "../{}/{}".format(build_config, firmware_filename)
catalog_file = os.path.join(output_dir, "catalog.json")

# noinspection PyBroadException
try:
    with open(catalog_file, "r") as cf:
        catalog = json.load(cf)
except Exception:
    catalog = {}

# copy firmware file
filename = "revvy_firmware-{}.bin".format(version_str)
destination_file = os.path.join(output_dir, filename)
shutil.copy(firmware_file, destination_file)

print("Firmware file added to catalog:")
for hwv in hardware_versions:

    # update catalog file
    size = os.stat(destination_file).st_size
    checksum = file_hash(destination_file)
    catalog[hwv] = {
        "version":  version_str,
        "filename": filename,
        "length":   size,
        "md5":      checksum
    }

    print("\t{} - HW: {}, FW: {}, size: {}, md5: {}".format(filename, hwv, version_str, size, checksum))

with open(catalog_file, "w") as cf:
    json.dump(catalog, cf, indent=4)
