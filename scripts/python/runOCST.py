import sys
import subprocess
import os
import configOCST
import time
from Queue import Queue
from threading import Thread
from configOCST import InstanceManager as manager
from configOCST import DataSets


def parallelRun(i, q):
    #Worker thread function.
    #These daemon threads go into an infinite loop, and only exit when the main thread ends.
    while True:
        (inputPath, outputPath, formulation) = q.get()
        start = time.time()
        print "Thread ", i, " new task: \t", inputPath, outputPath, formulation
        f=open(inputPath, "r")
        args = configOCST.InstanceManager.getArgList(formulation, outputPath) 
        proc = subprocess.Popen(args, stdin=f)
        proc.wait()
        end = time.time()
        print "Thread ", i," ends after:",int(end - start), "seconds"
        f.close()
        q.task_done()


def createOutput(Instance):
    instance = manager(Instance)
    if not os.path.exists( instance.instanceName() ):
        print("Directory " + instance.instanceName() +  " does not exist.")
        sys.exit();

    num_fetch_threads = manager.nthreads
    queue_tasks = Queue()

    for i in range(num_fetch_threads):
        worker = Thread(target=parallelRun, args=(i, queue_tasks,))
        worker.setDaemon(True)
        worker.start()

    for formulation in manager.formulations:
        for ni in manager.n:
            for pi in manager.p:
                    for i in range(manager.instances):
                        queue_tasks.put( (instance.fullPathInput(ni,pi,i), instance.fullPathOutput(ni,pi,formulation,i),
                                formulation ))
                        #w=open(instance.fullPathOutput(ni,pi,formulation,i),"w")
                        #w.close()
    print ("main thread waiting...")
    queue_tasks.join()
    print ("done")

def createOutputDataSets():
    for name in DataSets.dataSets:
        print name
        createOutput(name)


if __name__ == "__main__":

    if len(sys.argv) == 2 :
        createOutput(sys.argv[1])

    if len(sys.argv) == 1 :
        createOutputDataSets()

    sys.exit()
