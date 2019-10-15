import xml
import os
from pprint import pprint
from pdb import set_trace
from NetworkModelBuilder import NetworkModelBuilder
# from Ns3Simulator import Ns3Simulator

import networkx as nx
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages


HOMEPATH = os.path.dirname(os.path.abspath(__file__))

# region [Case: IEEE39]
# CASE = 'IEEE39.dat'
# endregion

# region [Case: WECC179]
CASE = '179.dat'
SEPERATORS = {
    'BUS DATA': [0,4,17,20,23,26,33,40,49,59,67,75,83,90,98,106,114,122,127],         
    'BRANCH DATA': [0,4,9,12,15,17,19,29,39,49,55,61,67,72,74,82,90,97,104,111,119,126]
    }
# endregion

# region [Case: IEEE14CDF]
CASE = 'ieee14cdf.txt'
SEPERATORS = {
    'BUS DATA': [0,4,17,20,23,26,33,40,49,59,67,75,83,90,98,106,114,122,127],         
    'BRANCH DATA': None
    }
# endregion

def visualize(filepath, links):
    g = nx.Graph()
    g.add_edges_from(links)
    pos = nx.kamada_kawai_layout(g)
    node_labels = get_node_labels(g.nodes)

    with PdfPages(filepath) as pdf:
        plt.figure(figsize=(18,18))
        nx.draw_networkx_nodes(g, pos, node_size=5, alpha=0.2)
        nx.draw_networkx_edges(g, pos, edgelist=[e for e in g.edges], alpha=0.2)
        nx.draw_networkx_labels(g, pos, labels=node_labels, font_size=4)
        plt.axis('off')
        pdf.savefig()
        plt.close()

def get_node_labels(nodes):
    labels = dict()
    for n in nodes:
        altname = n.DeviceName
        if n.__class__.__name__ != 'Relay':
            if len(n.DeviceName) >= 7:
                altname = '.'.join([i[0] for i in n.DeviceName.split()])
            if n.__class__.__name__ == 'Router':
                altname = '%s(R)' % n.Location
        labels.update({n: altname})
    return labels

if __name__ == '__main__':    
    b =  NetworkModelBuilder({
        'CasePath': '%s/dat/%s' % (HOMEPATH, CASE), 
        'CaseFormat': 'ieeecdf', 
        'IeeeCdfSeperators': SEPERATORS,
        })
        
    b.build_comm()
    b.xml_serialize('%s/etc/%s/networkbuilder.xml' % (HOMEPATH, CASE.split('.')[0]))
    # visualize('%s/etc/%s/networkbuilder.pdf' % (HOMEPATH, CASE.split('.')[0]), b.CommLinks)

    # sim = Ns3Simulator()
    # sim.xml_import('%s/etc/%s/networkbuilder.xml' % (HOMEPATH, CASE.split('.')[0]))
    # sim.run_tapcsma()

    set_trace()