"""
Generate a c/h file pair that contains runnable calls and port connections based on project.json
"""
import argparse
import json
import os
import sys

import pystache

header_template = """#ifndef GENERATED_RUNTIME_H_
#define GENERATED_RUNTIME_H_

{{#includes}}
#include "{{.}}.h"
{{/includes}}

{{#runnable_groups}}
void RunnableGroup_{{ group_name }}(void);
{{/runnable_groups}}

#endif /* GENERATED_RUNTIME_H */
"""

source_template = """#include "{{ output_filename }}.h"

{{#runnable_groups}}
void RunnableGroup_{{ group_name }}(void)
{
    {{ #runnables }}
    {{ component }}_Run_{{ runnable }}();
    {{ /runnables }}
}
{{/runnable_groups}}
"""

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', help='Name of project config json file', default="project.json")
    parser.add_argument('--validate-only', help='Skip generating code, only check the config json', action="store_true")
    parser.add_argument('--output', help='File name of generated files', default="rrrc/generated_runtime")

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

    template_ctx = {
        'output_filename': args.output[args.output.rfind('/')+1:],
        'includes':        ['components/{0}/{0}'.format(component) for component in config['components']],
        'runnable_groups': []
    }

    for runnable_group in config['runtime']['runnables']:
        group = {'group_name': runnable_group, 'runnables': []}

        for runnable in config['runtime']['runnables'][runnable_group]:
            group['runnables'].append(runnable)

        template_ctx['runnable_groups'].append(group)

    with open(args.output + ".h", "w+") as header:
        header.write(pystache.render(header_template, template_ctx))

    with open(args.output + ".c", "w+") as source:
        source.write(pystache.render(source_template, template_ctx))
