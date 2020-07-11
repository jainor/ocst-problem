#################################################################
# This script generate the targets to:
# Generate the scripts to run an Instance of tests
#   -   cmake . -D InstanceOCSTP=InstanceName       generate MakeFile with options Instance Name
#   -   make generate-instance                      Generate new random instances
#   -   make run-instance                           Run instance with available formulations in Directory Instances
#   -                                               Execute cmake to source code to src/lib
#   -                                               Execute cmake to run tests inside Instances/InstanceName
#################################################################

cmake_minimum_required(VERSION 3.13) #version check
project(ocstp) 

#################################################################
#   Python scripts
#################################################################

#find_package (Python3 REQUIRED)
find_package (Python3 COMPONENTS Interpreter)
message("PYTHON_LIBRARY :${PYTHON_LIBRARY}")
message("PYTHON_INCLUDE_DIR :${PYTHON_INCLUDE_DIR}")

set(scriptsDir "${PROJECT_SOURCE_DIR}/scripts/python")
message("Python scripts at: ${scriptsDir}")
set(generateFilePython "${scriptsDir}/generateInput.py")
set(runInstancePython "${scriptsDir}/runOCST.py")
set(readInstancePython "${scriptsDir}/readOutput.py")

add_custom_target( generate-instance COMMAND python3  ${generateFilePython} ${InstanceOCSTP} )
add_custom_target( run-instance COMMAND python  ${runInstancePython} ${InstanceOCSTP} )
add_custom_target( read-instance COMMAND python3  ${readInstancePython} ${InstanceOCSTP} )

