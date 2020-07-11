#this file depends on ansOptimal.h 
class ResultFieldsOCST:
    time = "time"
    gap =  "gap"
    lazy = "lazyConstraints"
    cuts = "cutsGenerated"
    solved = "solved"
    vertices = "vertices"
    probability = "prob"

class FramesConfig:
    extension = "csv" #current extension of file output
    outputSuffix = "_output"
    joinSuffix = "_joined"
    allName = "allFormulations"
    encoding = "utf-8-sig"
    generalResult = "general_results"

class FormulationsManager:
    header = "formulation"
    formulations = ["pathbased", "flowbased", "flowbasedrelaxed","rootedbased"]

class InstanceManager:

    gaps = [0.1,0.2,0.3,1]
    eps = 0.0001
    n = [20,40,50,60,70]
    p = [0.2,0.4,0.7]
    instances = 40
    ## NUMBER OF THREADS TO EXECUTE ALL EXPERIMENTS !!!
    nthreads = 8
    #n = [10,15]
    #p = [0.3,0.6]
    #instances = 4
    maxW = 10000000
    formulations = FormulationsManager.formulations
    defaultFormulation = 0
#change this
    command = "./src/bin/main"
    #command = "./lib/compareoptimal_c++"
    #time = 3600
    time = 3600
    heuristics = 0.05

    prefixInput =  "in"
    prefixOutput =  "out"
    defaultDirectory = "Instances"

    extensionInput = ""
    extensionOutput = ".csv"

    prefixn = "n"
    prefixp = "p"
    Instance = None

    def __init__(self,InstanceName):
        self.Instance = InstanceManager.addPath(InstanceManager.defaultDirectory, InstanceName)

    @staticmethod
    def addPath(x,y):
        return x + "/" + y;

    def getName(self,cn,cp):
        return  self.prefixn + str(cn) + self.prefixp + str(cp)
    
    @staticmethod
    def getArgList(formulation,fileOutput):
        return [InstanceManager.command,formulation,fileOutput, str(InstanceManager.time), str(InstanceManager.heuristics) ]

    def fullPath(self,cn,cp):
        return InstanceManager.addPath( self.Instance ,  self.getName(cn,cp) )

    def fullPathForm(self,cn,cp,formulation):
        dirnp = InstanceManager.addPath( self.Instance ,  self.getName(cn,cp) )
        return InstanceManager.addPath( dirnp ,  formulation )

    def fullPathInput(self,cn,cp,i):
        return InstanceManager.addPath( self.fullPath(cn,cp), InstanceManager.prefixInput +  str(i) + InstanceManager.extensionInput )

    def fullPathOutput(self,cn,cp,formulation,i):
        directory = InstanceManager.addPath( self.fullPath(cn,cp) , formulation)
        fileName = InstanceManager.addPath( directory, InstanceManager.prefixOutput +  str(i) + InstanceManager.extensionOutput )
        return fileName

    def instanceName(self):
        return self.Instance
