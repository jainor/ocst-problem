import networkx as nx
#import matplotlib.pyplot as plt
import sys
import subprocess
import os
import glob
import pandas as pd
import numpy as np

from configOCST import InstanceManager as manager
from configOCST import DataSets
from configOCST import FramesConfig as frameManager 
from configOCST import ResultFieldsOCST as OCSTFields
from configOCST import FormulationsManager as OCSTFormulations
import generateGraph as graphGen

def createInput(Instance):
    instance = manager(Instance)
    if not os.path.exists( instance.instanceName() ):
        os.makedirs(instance.instanceName(), exist_ok=True)
        print("Directory " + instance.instanceName() +  " created.")
    else:    
        print("Directory " + instance.instanceName() + " already exists.")
        sys.exit();

    for ni in manager.n:
        for pi in manager.p:
            os.mkdir(instance.fullPath(ni,pi))
            for formulation in manager.formulations:
                os.mkdir(instance.fullPathForm(ni,pi,formulation))
            for i in range(manager.instances):
                #open(instance.fullPathInput(ni,pi,i), 'a').close()
                graphGen.generateInputOCSTP(ni,pi,instance.fullPathInput(ni,pi,i))
                
                #os.mkdir(fullPath(ni,pi,i))

# create datasets defined in config.py
def createDataSet():
    for name, pR, pW in zip(DataSets.dataSets, DataSets.probRValues, DataSets.maxWValues) :
        manager.probR = pR
        manager.maxW = pW
        createInput(name)

if __name__ == "__main__":

    if len(sys.argv) == 2 :
        createInput(sys.argv[1])

    if len(sys.argv) == 1 :
        createDataSet()

    sys.exit()
