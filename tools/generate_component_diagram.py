import argparse

import chevron
from graphviz import Digraph

from tools.plugins.BuiltinDataTypes import builtin_data_types
from tools.plugins.Locks import locks
from tools.plugins.ProjectConfigCompactor import project_config_compactor
from tools.plugins.RuntimeEvents import runtime_events
from tools.plugins.UserCodePlugin import user_code_plugin
from tools.runtime import Runtime

component_template = '''<<TABLE BORDER="1" CELLBORDER="0" CELLSPACING="0">
    <TR>
        <TD COLSPAN="2" BGCOLOR="grey"><FONT COLOR="white"><B>{{ component_name }}</B></FONT></TD>
    </TR>
    <TR>
        <TD>
            {{# has_consumer_ports }}
            <TABLE FIXEDSIZE="TRUE" WIDTH="0" HEIGHT="0" ALIGN="LEFT" BORDER="0" CELLBORDER="1" CELLSPACING="0">
                {{# consumer_ports }}
                <TR><TD PORT="{{ component_name }}/{{ . }}" BGCOLOR="lightgrey">&gt;</TD><TD ALIGN="LEFT">{{ . }}</TD></TR>
                {{/ consumer_ports }}
            </TABLE>
            {{/ has_consumer_ports }}
        </TD>
        <TD VALIGN="TOP">
            {{# has_provider_ports }}
            <TABLE FIXEDSIZE="TRUE" WIDTH="0" HEIGHT="0" ALIGN="RIGHT" BORDER="0" CELLBORDER="1" CELLSPACING="0">
                {{# provider_ports }}
                <TR><TD ALIGN="RIGHT">{{ . }}</TD><TD PORT="{{ component_name }}/{{ . }}" BGCOLOR="lightgrey">&gt;</TD></TR>
                {{/ provider_ports }}
            </TABLE>
            {{/ has_provider_ports }}
        </TD>
    </TR>
</TABLE>>
'''

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument('--config', help='Name of project config json file', default="project.json")
    parser.add_argument('--format', help='Output file format', choices=['pdf', 'svg'], default="pdf")
    parser.add_argument('--output', help='Output file name', default="diagram")
    parser.add_argument('--component', help='Only render connections for this component', default=None)

    args = parser.parse_args()

    rt = Runtime(args.config)
    rt.add_plugin(project_config_compactor())
    rt.add_plugin(builtin_data_types())
    rt.add_plugin(runtime_events())
    rt.add_plugin(locks())
    rt.add_plugin(user_code_plugin())

    rt.load(True)

    context = {}
    rt.generate_runtime('', context)

    def connections(signals):
        """List provider-consumer pairs from the signal list"""
        for provider_name, signals in signals.items():
            for consumer_connections in signals.values():
                if type(consumer_connections) is list:
                    consumer_connections_list = consumer_connections
                else:
                    consumer_connections_list = [consumer_connections]

                for signal in consumer_connections_list:
                    for consumer in signal.consumers:
                        yield provider_name, consumer[0]

    # collect info
    if args.component is not None:
        components_to_draw = set()
        edges_to_draw = []
        for provider_name, consumer_name in connections(context['signals']):
            pc = provider_name.split('/')[0]
            cc = consumer_name.split('/')[0]
            if pc == args.component or cc == args.component:
                components_to_draw.add(pc)
                components_to_draw.add(cc)
                edges_to_draw.append((provider_name, consumer_name))
    else:
        components_to_draw = rt._components.keys()
        edges_to_draw = connections(context['signals'])

    # render
    def add_component(g: Digraph, component_name, component_data):
        """Display a component and its ports"""
        # classify ports
        consumer_ports = []
        provider_ports = []
        for port_name, port_data in component_data['ports'].items():
            port_type_data = rt.port_types[port_data['port_type']]
            if 'consumes' in port_type_data:
                consumer_ports.append(port_name)
            else:
                provider_ports.append(port_name)

        consumer_ports = sorted(consumer_ports)
        provider_ports = sorted(provider_ports)

        # render HTML template and add node
        template_ctx = {
            'component_name': component_name,
            'consumer_ports': consumer_ports,
            'has_consumer_ports': len(consumer_ports) > 0,
            'provider_ports': provider_ports,
            'has_provider_ports': len(provider_ports) > 0
        }

        g.node(component_name, label=chevron.render(component_template, data=template_ctx))

    filename = args.output
    if args.component:
        filename += '-' + args.component

    g = Digraph(engine='dot', filename=filename + '.gv', format=args.format)
    g.attr('node', shape='plaintext')
    g.attr('graph', rankdir='LR')
    g.attr('graph', ranksep='3')
    g.attr('graph', splines='polyline')
    g.attr('graph', concentrate='true')

    for name in components_to_draw:
        add_component(g, name, rt._components[name])

    for provider_name, consumer_name in edges_to_draw:
        pc = provider_name.split('/')[0]
        cc = consumer_name.split('/')[0]
        g.edge('{}:{}:e'.format(pc, provider_name), '{}:{}:w'.format(cc, consumer_name))

    g.view()
