import argparse

from tools.generator_common import change_file
from tools.plugins.BuiltinDataTypes import builtin_data_types
from tools.plugins.ProjectConfigCompactor import project_config_compactor
from tools.plugins.RuntimeEvents import runtime_events
from tools.runtime import Runtime


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', help='Name of project config json file', default="project.json")
    parser.add_argument('--output', help='File name of generated files', default="rrrc/generated_runtime")

    args = parser.parse_args()

    rt = Runtime(args.config)
    rt.add_plugin(project_config_compactor())
    rt.add_plugin(builtin_data_types())
    rt.add_plugin(runtime_events())

    rt.load(True)

    files = rt.generate_runtime(args.output)
    for file_name, contents in files.items():
        change_file(file_name, contents)
