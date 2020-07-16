import networkx as nx
#import matplotlib.pyplot as plt
import sys
import subprocess
import os
from configOCST import InstanceManager as manager
from configOCST import FramesConfig as frameManager 
from configOCST import ResultFieldsOCST as OCSTFields
from configOCST import FormulationsManager as OCSTFormulations
from configOCST import DataSets

import os
import glob
import pandas as pd
import numpy as np

def joinOutput(Instance):
    instance = manager(Instance)
    if not os.path.exists( instance.instanceName() ):
        print("Directory " + instance.instanceName() +  " does not exist.")
        sys.exit();

    formulationFrames = []
    for formulation in manager.formulations:
        allFiles = []
        for ni in manager.n:
            for pi in manager.p:
                for i in range(manager.instances):
                    allFiles.append( instance.fullPathOutput(ni,pi,formulation,i) )
                #combine all files in the list
        combined_csv = pd.concat([pd.read_csv(f, index_col=None) for f in allFiles ], axis=0, ignore_index=True)
        #export to csv
        name_combined = manager.addPath( instance.Instance, formulation + frameManager.joinSuffix + "." + frameManager.extension ) 
        print (combined_csv)
        combined_csv.to_csv( name_combined, index=False, encoding='utf-8')

        combined_csv[OCSTFormulations.header] = formulation
        formulationFrames.append(combined_csv)

        dictionary = {OCSTFields.time:[np.mean,np.std,np.max,np.min],
                      OCSTFields.gap:[np.mean,np.std],
                      OCSTFields.lazy:np.mean,
                      OCSTFields.cuts:np.mean,
                      OCSTFields.solved:np.sum}

        frame = combined_csv.groupby([OCSTFields.vertices,OCSTFields.probability],as_index=False).agg(dictionary)
        #print (newFile)
        frame.to_csv(  manager.addPath(instance.Instance, formulation + frameManager.outputSuffix + "." + frameManager.extension ), index=False, encoding=frameManager.encoding)
        frame.to_latex(  manager.addPath(instance.Instance, formulation + frameManager.outputSuffix + ".tex" ), multirow = True, escape = False, index=False, encoding=frameManager.encoding)
        
    #common metrics for all cases
    allFrames = pd.concat(formulationFrames, axis=0, ignore_index=True)
    name_allFrames = manager.addPath( instance.Instance, frameManager.allName + "." + frameManager.extension )
    
    for pi in manager.gaps:
        allFrames[str(pi)] = allFrames[ OCSTFields.gap ].apply(lambda x: 1 if x - manager.eps <= pi else 0)

    allFrames.to_csv( name_allFrames, index=False, encoding='utf-8')


    dictionaryAll = {OCSTFields.time:[np.mean],
                     OCSTFields.solved:[np.sum]}
    for pi in manager.gaps:
        dictionaryAll[str(pi)]=np.sum;
    
    frame = allFrames.groupby([OCSTFormulations.header],as_index=False).agg(dictionaryAll)
    frame.to_csv(  manager.addPath(instance.Instance, frameManager.generalResult + "." + frameManager.extension ), index=False, encoding=frameManager.encoding)

    return pd.concat(formulationFrames, axis=0, ignore_index=True)

def joinOutputDataSets():
    DataSetFrames = []
    newLabel = 'DataSet'
    csvFile = 'AllDataSets.csv'
    for nameInstance in DataSets.dataSets:
        dataSetFrame = joinOutput(nameInstance)
        dataSetFrame[newLabel] = nameInstance
        DataSetFrames.append(dataSetFrame)
    allFrames = pd.concat(DataSetFrames, axis=0, ignore_index=True)
    for nameInstance in DataSets.dataSets:
        allFrames[nameInstance] =  allFrames[ [newLabel, OCSTFields.solved] ].apply(lambda x: 1 if x[0] == nameInstance and x[1] == 1 else 0, axis=1)
    allFrames.to_csv( csvFile, index=False, encoding=frameManager.encoding)

if __name__ == "__main__":

    if len(sys.argv) == 1:
        joinOutputDataSets()

    if len(sys.argv) == 2:
        joinOutput(sys.argv[1])
    
    sys.exit()
