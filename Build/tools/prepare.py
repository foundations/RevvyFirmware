#!/usr/bin/python3
import os
import shutil
import json
import argparse
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
firmware_filename = "rrrc_samd51.bin"

if __name__ == "__main__":
    # prepare
    parser = argparse.ArgumentParser()
    parser.add_argument('--out', default='output', help='Directory where the output shall be placed')
    parser.add_argument('--build-dir', default='Debug', help='Directory where the output shall be placed')
    
    args = parser.parse_args()
    
    with open('Config/fw_version.h') as fw_version:
        contents = fw_version.readlines()
        version_patch = contents[4].replace('#define FW_VERSION_NUMBER ', '').strip()

    os.makedirs(args.out, exist_ok=True)

    version_str = "{}.{}.{}".format(version_major, version_minor, version_patch)
    firmware_file = "{}/{}".format(args.build_dir, firmware_filename)
    catalog_file = os.path.join(args.out, "catalog.json")

    # noinspection PyBroadException
    try:
        with open(catalog_file, "r") as cf:
            catalog = json.load(cf)
    except Exception:
        catalog = {}

    # copy firmware file
    filename = "revvy_firmware-{}.bin".format(version_str)
    destination_file = os.path.join(args.out, filename)
    shutil.copy(firmware_file, destination_file)

    print("Changes made to catalog file:")
    for hwv in hardware_versions:
        try:
            # delete old file
            old = catalog[hwv]
            if old['filename'] != filename:
                os.unlink(os.path.join(args.out, old['filename']))
                print('\tDeleted old firmware: {}'.format(old['filename']))
        except KeyError:
            pass
        except FileNotFoundError:
            pass

        # update catalog file
        size = os.stat(destination_file).st_size
        checksum = file_hash(destination_file)
        catalog[hwv] = {
            "version":  version_str,
            "filename": filename,
            "length":   size,
            "md5":      checksum
        }

        print("\tAdded: {} - HW: {}, FW: {}, size: {}, md5: {}".format(filename, hwv, version_str, size, checksum))

    with open(catalog_file, "w") as cf:
        json.dump(catalog, cf, indent=4)
