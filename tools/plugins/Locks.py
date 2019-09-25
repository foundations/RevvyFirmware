from tools.runtime import RuntimePlugin, Runtime, SignalConnection

can_be_async = [
    'WriteData',
    'WriteIndexedData',
    'WriteQueuedData',
    'ReadValue',
    'ReadIndexedValue',
    'ReadQueuedValue'
]


def add_async_flag(owner: Runtime):
    """Add the 'async' optional flag to supported port types"""
    for port_type, known_port_type in owner.port_types.items():
        if port_type in can_be_async:
            known_port_type['def_attributes']['optional']['async'] = False


def is_signal_async(owner: Runtime, connection: SignalConnection):
    """Determine whether connection requires locks to be generated

    If either the provider port or any of the consumers are async, generate locks.
    Exception: when the provider is not known, assume it's constant and skip locks."""
    port = owner.get_port(connection.provider)
    if port['port_type'] not in can_be_async:
        return False
    if port['async']:
        return True

    for consumer in connection.consumers:
        consumer_port = owner.get_port(consumer)
        if consumer_port['port_type'] in can_be_async:
            if consumer_port['async']:
                return True

    return False


def surround_with_lock(signal_impl):
    lock_impl = '/* TODO lock */'
    unlock_impl = '/* TODO unlock */'

    if type(signal_impl) is list:
        signal_impl.insert(0, lock_impl)
        signal_impl.append(unlock_impl)
    else:
        signal_impl = '{}\n{}\n{}'.format(lock_impl, signal_impl, unlock_impl)

    return signal_impl


def on_signal_generated(owner: Runtime, connection: SignalConnection, function_mod_list: list):
    if not is_signal_async(owner, connection):
        return

    for function_mods in function_mod_list:
        for mod in function_mods.values():
            if 'body' in mod:
                mod['body'] = surround_with_lock(mod['body'])


def locks():
    return RuntimePlugin("Locks", {
        'init':             add_async_flag,
        'signal_generated': on_signal_generated
    })
