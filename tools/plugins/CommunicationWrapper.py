import chevron

from tools.generator_common import dict_to_chevron_list
from tools.plugins.UserCodePlugin import add_sections_to_function
from tools.runtime import RuntimePlugin, Runtime, FunctionDescriptor

header_template = """#ifndef COMM_WRAPPERS_H_
#define COMM_WRAPPERS_H_

#include "components/MasterCommunication/CommunicationManager.h"

const Comm_CommandHandler_t communicationHandlers[{{ command_count }}];

#endif
"""

source_template = """#include "{{ runtime_header }}"

{{# includes }}
#include {{{ . }}}
{{/ includes }}

{{# command_functions }}
static {{{ . }}}
{{/ command_functions }}

/* Begin User Code Section: command handlers */

/* End User Code Section: command handlers */

const Comm_CommandHandler_t communicationHandlers[{{ command_count }}] = {
    {{# commands }}
    [{{ id }}] = {
        .Start = {{# start }}&{{ . }}{{/ start }}{{^ start }}NULL{{/ start }},
        .GetResult = {{# result }}&{{ . }}{{/ result }}{{^ result }}NULL{{/ result }},
        .Cancel = {{# cancel }}&{{ . }}{{/ cancel }}{{^ cancel }}NULL{{/ cancel }}
    },
    {{/ commands }}
    
    /* Begin User Code Section: commands */
    /* End User Code Section: commands */
};
"""


def create_command_function(types, function_name):
    fn = FunctionDescriptor(types, function_name, 'Comm_Status_t')

    fn.add_argument('commandPayload', 'ByteArray_t')
    fn.add_argument('response', 'ByteArray_t')
    fn.add_argument('responseCount', 'uint8_t*')

    fn.set_return_statement('Comm_Status_Ok')
    return fn


def wrap_constant_port(owner, wrapper_function: FunctionDescriptor, command, port, port_data,
                       port_function: FunctionDescriptor):
    wrapper_function.mark_argument_used('commandPayload')
    wrapper_function.mark_argument_used('responseCount')
    wrapper_function.mark_argument_used('response')

    wrapper_function.add_body('''if (commandPayload.count != 0u)
{
    return Comm_Status_Error_PayloadLengthError;
}''')

    if port_function.return_type == 'void':
        wrapper_function.add_body(
            port_function.function_call({'value': '({}*) &response.body'.format(port_data['data_type'])}) + ';')
    else:
        wrapper_function.includes.add('<string.h>')  # memcpy

        wrapper_function.add_body('{} temp = {};'.format(port_data['data_type'], port_function.function_call()))
        wrapper_function.add_body('memcpy(response.body, &temp, sizeof(temp);')
    wrapper_function.add_body('*responseCount = sizeof({});'.format(port_data['data_type']))


def validate_variable_length_input_slot(inputs, function_arguments):
    n_variable_args = 0
    for t in function_arguments.values():
        if t == 'ByteArray_t':
            n_variable_args += 1

    if n_variable_args == 0:
        return True

    elif n_variable_args > 1:
        return False

    else:
        return function_arguments[inputs[-1]['argument']] == 'ByteArray_t'


def get_slot_offsets(inputs, arguments):
    reserved_count = 0
    last_arg_name = None

    offsets = {}

    for input_spec in inputs:
        if 'argument' in input_spec:
            if input_spec['argument'] is None:
                reserved_count += input_spec['size']  # reserved place
            else:
                if last_arg_name is None:
                    start = reserved_count
                else:
                    start = 'offset_of_{} + sizeof({}) + {}'.format(
                        last_arg_name, arguments[last_arg_name], reserved_count)

                offsets[input_spec['argument']] = start

                last_arg_name = input_spec['argument']
                reserved_count = 0

    return offsets


def wrap_runnable_port(owner, wrapper_function: FunctionDescriptor, command, port, port_data,
                       port_function: FunctionDescriptor):

    # generate argument mapping to inputs, outputs
    inputs = command.get('inputs', [])
    outputs = command.get('outputs', [])

    input_arg_names = [i['argument'] for i in inputs if 'argument' in i and i['argument']]
    output_arg_names = [i['argument'] for i in outputs if 'argument' in i and i['argument']]

    if input_arg_names:
        wrapper_function.mark_argument_used('commandPayload')
    if output_arg_names:
        wrapper_function.mark_argument_used('response')
        wrapper_function.mark_argument_used('responseCount')

    missing_args = set(port_function.arguments.keys()) - set(input_arg_names)
    if missing_args:
        # TODO: implement output mapping
        # missing_args = missing_args - set(output_arg_names)

        if missing_args:
            raise Exception(
                'Arguments {} are missing from {}'.format(', '.join(missing_args), port_function.function_name))

    if not validate_variable_length_input_slot(inputs, port_function.arguments):
        raise Exception('Variable sized slot must be the last one for {}'.format(command['target']))

    input_offsets = get_slot_offsets(inputs, port_function.arguments)
    output_offsets = get_slot_offsets(outputs, port_function.arguments)

    # first, gather offsets
    for arg_name, offset in input_offsets.items():
        wrapper_function.add_body('const size_t offset_of_{} = {};'.format(arg_name, offset))

    for arg_name, offset in output_offsets.items():
        wrapper_function.add_body('const size_t offset_of_{} = {};'.format(arg_name, offset))

    # generate size check

    has_var_arg = False
    for arg_name in input_arg_names:
        if port_function.arguments[arg_name] == 'ByteArray_t':
            has_var_arg = True

    if not input_arg_names:
        wrapper_function.add_body('''
if (commandPayload.count != 0u)
{{
    return Comm_Status_Error_PayloadLengthError;
}}''')

    elif has_var_arg:
        wrapper_function.add_body('''
const size_t input_min_size = offset_of_{0} + 1u;
if (commandPayload.count < input_min_size || commandPayload.count < input_min_size + commandPayload.bytes[offset_of_{0}])
{{
    return Comm_Status_Error_PayloadLengthError;
}}'''.format(input_arg_names[-1]))
    else:
        wrapper_function.add_body('''
const size_t input_size = offset_of_{} + sizeof({});
if (commandPayload.count != input_size)
{{
    return Comm_Status_Error_PayloadLengthError;
}}'''.format(input_arg_names[-1], port_function.arguments[input_arg_names[-1]]))

    # generate arguments
    arguments = {}
    for arg_name in input_arg_names:
        if port_function.arguments[arg_name] == 'ByteArray_t':
            wrapper_function.add_body('''
ByteArray_t variable_argument = {{
    .bytes = &commandPayload.bytes[offset_of_{0} + 1],
    .count = commandPayload.bytes[offset_of_{0}]
}};'''.format(arg_name))
            arguments[arg_name] = 'variable_argument'

        else:
            if port_function.arguments[arg_name] == 'uint8_t':
                arguments[arg_name] = 'commandPayload.bytes[offset_of_{}]'.format(arg_name)
            else:
                wrapper_function.add_body('{0} value_of_{1} = *({0}*) &commandPayload.bytes[offset_of_{1}]'
                                          .format(port_function.arguments[arg_name], arg_name))
                arguments[arg_name] = 'value_of_{}'.format(arg_name)

    function_call = port_function.function_call(arguments)

    if port_function.return_type != 'void':

        # generate return value mapping
        wrapper_function.add_body(chevron.render(**{
            'template': '{{ return_type }} call_response = {{{ func_call }}};',
            'data':     {
                'return_type': port_function.return_type,
                'func_call':   function_call
            }}))

        if 'return_codes' not in command:
            print('Warning: ignoring return value of {}, command does not have return code mapping'.format(port))
        else:
            if not command['return_codes'] or type(command['return_codes']) is not dict:
                raise Exception('Invalid return value mapping specified for {}'.format(port))

            for value in command['return_codes'].values():
                if value not in owner.types.get('Comm_Status_t')['values']:
                    raise Exception('Invalid return value specified for {}: {}'.format(port, value))

            try:
                if command['default_response'] not in owner.types.get('Comm_Status_t')['values']:
                    raise Exception('Invalid default return value specified for {}'.format(port))
            except KeyError:
                command['default_response'] = 'Comm_Status_Ok'

            wrapper_function.add_body('Comm_status_t return_value;')
            wrapper_function.add_body(chevron.render(**{
                'template': '''switch (call_response)
{
    {{# return_value_map }}
    case {{ value }}:
        return_value = {{ status }};
        break;
    {{/ return_value_map }}
    
    default:
        return_value = {{ default_status }};
        break;
}''',
                'data':     {
                    'default_status':   command['default_response'],
                    'return_value_map': dict_to_chevron_list(command['return_codes'], 'value', 'status')
                }
            }))

            wrapper_function.set_return_statement('return_value')

    else:
        # assume call is no-fail
        wrapper_function.add_body(function_call + ';')


port_wrapper_implementations = {
    'Constant': wrap_constant_port,
    'Runnable': wrap_runnable_port
}


def create_port_wrapper(owner, context, port, command):
    port_data = owner.get_port(port)
    target_port_type = port_data['port_type']
    function = create_command_function(owner.types, 'CommWrapper_{}'.format(port.replace('/', '_')))
    port_function = context['functions'][port]

    try:
        wrapper_generator = port_wrapper_implementations[target_port_type]
    except KeyError:
        raise Exception("Can't generate wrapper for {}: CommWrapper is incompatible with {} ports."
                        .format(port, target_port_type))

    wrapper_generator(owner, function, command, port, port_data, port_function)

    return function


def generate_command_implementation(owner: Runtime, context, command_id, command):
    if command == {}:
        ping_function = create_command_function(owner.types, 'CommWrapper_PingCommand_{}'.format(command_id))

        return {'start': ping_function}

    wrapped_port = command['target']

    handler = {}

    handler['start'] = create_port_wrapper(owner, context, wrapped_port, command)

    return handler


def generate_comm_wrappers(owner: Runtime, context):
    config = owner._project_config.get('communication', {})

    command_id = 0
    commands = config.get('commands', [])
    command_implementations = {}
    for command in commands:
        if command is None:
            # unimplemented (or user implemented) command
            pass

        else:
            if type(command) == str:
                # shorthand for simple port wrappers
                command = {"target": command}

            if type(command) is not dict:
                raise Exception('Command {} is not a string or dictionary'.format(command_id))

            command_implementations[command_id] = generate_command_implementation(owner, context, command_id, command)

        command_id += 1

    def get_fn_names(commands):
        names_list = []
        for id, command in commands.items():
            names = {'id': id}
            for phase, handler in command.items():
                if handler:
                    names[phase] = handler.function_name
                else:
                    names[phase] = None
            names_list.append(names)
        return names_list

    def get_fns(commands):
        function_list = []
        for command in commands:
            for phase, handler in command.items():
                if handler:
                    add_sections_to_function(handler, handler.function_name)
                    function_list.append(handler)
        return function_list

    headers = [k for k in context['files'].keys() if k.endswith('.h')]
    command_functions = get_fns(command_implementations.values())

    includes = set()
    for f in command_functions:
        includes.update(f.includes)

    context['files']['rrrc/generated_comm_wrappers.c'] = chevron.render(source_template, {
        'runtime_header':    headers[0],
        'command_count':     len(commands),
        'commands':          get_fn_names(command_implementations),
        'command_functions': [fn.get_function() for fn in command_functions],
        'includes':          sorted(includes)
    })
    context['files']['rrrc/generated_comm_wrappers.h'] = chevron.render(header_template, {
        'command_count': len(commands)
    })

    context['runtime_includes'].add('"generated_comm_wrappers.h"')
    # TODO: create constant port to provide commands


def communication_wrapper():
    return RuntimePlugin(
        "CommunicationWrapper",
        {
            'before_generating_runtime': generate_comm_wrappers
        },
        requires=['BuiltinDataTypes', 'RuntimeEvents'])
