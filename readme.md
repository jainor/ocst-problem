#Run Instances

First, compile CMAKE. Then a MAKEFILE. will be generated
                                                                                
CMAKE will build a target rule for creating a new set of instances in Directory Instances/< InstanceName>
                                                                                
    cmake . -D InstanceOCSTP=<InstanceName>
    make generate-instance
                                                                                
Then, if you want to run the experiments in < InstanceName> you should execute the following:
                                                                                
    make run-instance
                                                                                
Observe that if you want to change the target directory, you must execute: 
                                                                                
    cmake . -D InstanceOCSTP=<OtherInstanceName> 
                                                                                
The format of the directory is specified in the **python scripts (scripts/python)**. In particular, the files configOCST.py
contains all the information and structure on running tests. 
                                                                                
                                                                                
##About Source Code 
                                                                               
Conversely, if you want to modify the **c++ source code (in src/)**, you should execute CMAKE. (in the directory src/) and rerun the CMake command in the root directory.

    cd src
    cmake .
    make

The binary generated is saved in the directory **src/bin**. Since we only make modifications in **src/** we can execute *make run-instance* with an updated version without additional changes. 
                                                                                
##Testing 

Furthermore, if you want to add a new test, you must add the input and output file to Directory **/src/test/TestInstances**.
Then you should modify the file **formulations\_test.cpp** and include the file names to input and output. Then run CMake . in directory
src to create and update binaries. 
                                                                                
To execute test cases, you should execute (after **cmake .**):

    ctest -VV 
                                                                                
Thank you. 
