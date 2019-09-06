"""
Generate a c/h file pair that contains runnable calls and port connections based on project.json
"""
import argparse
import json
import os
import sys

import pystache


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', help='Name of project config json file', default="project.json")
    parser.add_argument('--validate-only', help='Skip generating code, only check the config json', action="store_true")

    args = parser.parse_args()

    with open(args.config, "r") as f:
        config = json.load(f)

    # validate components
    valid = True
    for component in config['components']:
        if not os.path.isdir('rrrc/components/{}'.format(component)):
            print('Component folder for {} does not exist'.format(component))
            valid = False
        else:
            required_files = ['config.json', component + '.c', component + '.h']
            for file in required_files:
                if not os.path.isfile('rrrc/components/{}/{}'.format(component, file)):
                    print('{} does not exist in component {}'.format(file, component))
                    valid = False

    # validate runnables
    for runnable_group in config['runtime']['runnables']:
        for runnable in config['runtime']['runnables'][runnable_group]:
            component_name = runnable['component']
            if component_name not in config['components']:
                print('Component {} does not exist'.format(component_name))
                valid = False
            else:
                component_config_file = 'rrrc/components/{}/config.json'.format(component_name)
                with open(component_config_file, "r") as f:
                    component_config = json.load(f)
                runnable_name = runnable['runnable']
                if runnable_name not in component_config.get('runnables', {}):
                    print('Component {} does not have a runnable called {}'.format(component_name, runnable_name))
                    valid = False

    if not valid:
        sys.exit(1)

    if args.validate_only:
        if valid:
            print("Runtime configuration is valid")
        sys.exit(0)
