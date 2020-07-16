import networkx as nx
import matplotlib.pyplot as plt
import sys
import subprocess
import random

from configOCST import InstanceManager as manager 

def generateWeightedGraph(n,p):
    G = nx.erdos_renyi_graph(n,p)
    for (u, v) in G.edges():
        G.edges[u,v]['weight'] = random.randint(0,manager.maxW)
    return G

def generateInputOCSTP(n,p,fileName):
    print(fileName)
    outputFile = open(fileName, 'w') 
    while True:
        G = generateWeightedGraph(n,p)
        if nx.is_connected(G):
            break
    print (G.number_of_nodes(), G.size(), p,file=outputFile)
    for (u, v, d) in G.edges(data=True):
        print (u,v,d['weight'],file=outputFile)
    # R is the graph induced by requirements pairs
    R = generateWeightedGraph(n,manager.probR)
    print (R.size(),file=outputFile)
    for (u, v, d) in R.edges(data=True):
        print (u,v,d['weight'],file=outputFile)
    outputFile.close()


if __name__ == "__main__":
    sys.exit()
    
