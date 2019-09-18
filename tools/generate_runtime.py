"""
Generate a c/h file pair that contains runnable calls and port connections based on project.json
"""
import argparse
import sys
import chevron

from tools.generator_common import to_underscore, render_typedefs, change_file
from tools.plugins.BuiltinDataTypes import builtin_data_types
from tools.plugins.ComponentConfigCompactor import component_config_compactor
from tools.plugins.ProjectConfigCompactor import project_config_compactor
from tools.plugins.RuntimeEvents import runtime_events
from tools.runtime import Runtime

header_template = """#ifndef GENERATED_RUNTIME_H_
#define GENERATED_RUNTIME_H_

{{# type_includes }}
#include {{{ header }}}
{{/ type_includes }}

{{# types }}
{{{ . }}}
{{/ types }}

{{# components }}
#define COMPONENT_TYPES_{{ guard_def }}_H_
{{/ components }}

{{# components }}
#include "components/{{ name }}/{{ name }}.h"
{{/ components }}

{{# runtime_events }}
{{{ . }}}
{{/ runtime_events }}
#endif /* GENERATED_RUNTIME_H */
"""

source_template = """#include "{{ output_filename }}.h"
#include "utils.h"

{{# variables }}
{{{ . }}}
{{/ variables }}

{{# port_functions }}
{{{ . }}}
{{/ port_functions }}
"""


def validate_runnables(project_config, component_data):
    """Check runnables in the runtime configuration.

    Validate runnables against their definitions in their respective component configurations"""

    runnables_valid = True
    runtime_config = project_config['runtime']
    for event_name, runnables in runtime_config['runnables'].items():
        for runnable in runnables:
            provider_component_name = runnable['component']
            runnable_name = runnable['runnable']
            try:
                component_config = component_data[provider_component_name]
                try:
                    if component_config['runnables'][runnable_name]['arguments']:
                        print('{}_Run_{} must not have arguments'.format(provider_component_name, runnable_name))
                        runnables_valid = False
                except KeyError:
                    print('Component {} does not have a runnable called {}'
                          .format(provider_component_name, runnable_name))
                    runnables_valid = False
            except KeyError:
                print('Component {} does not exist'.format(provider_component_name))
                runnables_valid = False

    return runnables_valid


def create_runnable_groups(config):
    return list(config.keys())


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', help='Name of project config json file', default="project.json")
    parser.add_argument('--verbose', help='Print additional information', action="store_true")
    parser.add_argument('--validate-only', help='Skip generating code, only check the config json', action="store_true")
    parser.add_argument('--output', help='File name of generated files', default="rrrc/generated_runtime")

    args = parser.parse_args()

    if args.verbose:
        def log(x):
            print(x)
    else:
        def log(x):
            pass


    def get_runnable(runnable_ref, components):
        component = components[runnable_ref['component']]
        try:
            return component['runnables'][runnable_ref['port']]
        except KeyError:
            print('Invalid runnable reference: {}'.format(runnable_ref))
            print('Component runnables: {}'.format(list(component['runnables'].keys())))
            raise


    def get_port(port_ref, components):
        component = components[port_ref['component']]
        try:
            return component['ports'][port_ref['port']]
        except KeyError:
            print('Invalid port reference: {}'.format(port_ref))
            print('Component ports: {}'.format(list(component['ports'].keys())))
            raise


    log('Loading project configuration from {}'.format(args.config))
    rt = Runtime(args.config)
    rt.add_plugin(project_config_compactor())
    rt.add_plugin(builtin_data_types())
    rt.add_plugin(runtime_events())
    rt.add_plugin(component_config_compactor())

    rt.load(True)
    project_config = rt._project_config

    log('')
    log('Load components')
    component_data = rt._components
    valid = True

    log('')
    log('Validate runnables')
    valid = valid and validate_runnables(project_config, component_data)

    type_data = rt._types

    rt.generate_runtime('generated_runtime_')

    if not valid:
        print("Configuration invalid, exiting")
        sys.exit(1)
    elif args.validate_only:
        print("Runtime configuration is valid, exiting")
        sys.exit(0)

    ctx = rt.generate_runtime('generated_runtime')

    template_ctx = {
        'output_filename': args.output[args.output.rfind('/') + 1:],
        'includes':        ['components/{0}/{0}'.format(component) for component in project_config['components']],
        'components':      [{
            'name':      component,
            'guard_def': to_underscore(component).upper()
        } for component in project_config['components']],
        'types':           render_typedefs(type_data, type_data),
        'runnable_groups': create_runnable_groups(project_config['runtime']['runnables'])
    }

    change_file(args.output + '.h', chevron.render(template=header_template, data=template_ctx))
    change_file(args.output + '.c', chevron.render(source_template, template_ctx))
