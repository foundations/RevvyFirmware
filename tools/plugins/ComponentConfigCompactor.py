from tools.runtime import RuntimePlugin


def process_runnable_def(component, runnable_name, runnable):
    short_name = '{}/{}'.format(component, runnable_name)

    try:
        if type(runnable['arguments']) is list:
            print('Warning: converting argument list to dict in {}'.format(short_name))
            args = {}
            for arg_type in runnable['arguments']:
                args['arg_{}'.format(len(args))] = arg_type
        else:
            args = runnable['arguments']
    except KeyError:
        args = {}

    return {
        'short_name':  short_name,
        'arguments':   args,
        'return_type': runnable.get('return_type', 'void')
    }


def process_runnable_defs(component, runnable_config):
    return {runnable: process_runnable_def(component, runnable, runnable_config[runnable]) for runnable in
            runnable_config}


def expand_component_config(owner, component_name, component_config):
    """Expand shorthand forms in component configuration"""
    component_config['runnables'] = process_runnable_defs(component_name, component_config.get('runnables', {}))


def component_config_compactor():
    """Plugin that expands component configuration on load and compacts it on save"""
    handlers = {
        'load_component_config': expand_component_config
    }
    return RuntimePlugin("ComponentConfigCompactor", handlers)
