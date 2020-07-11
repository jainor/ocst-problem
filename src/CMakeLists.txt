#################################################################
# This script generate the targets to:
# Generate the scripts to run an Instance of tests
#   -   cmake . 							        generate MakeFile with options Instance Name
#   -   make    			                      	Add generated lib to /lib/	update version
#   -   make                                        Run instance with available formulations in Directory Instances
#   -                                               Execute Clang-format on all files
#################################################################

cmake_minimum_required(VERSION 3.13) #version check
project(ocstp-source) 
set(CMAKE_CXX_STANDARD 17)

#########################################################
#   Modify directories`
#########################################################

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#########################################################
#   Shared Library
#########################################################

file(GLOB project_SRC
    "${PROJECT_SOURCE_DIR}/c++/*.cc"
    "${PROJECT_SOURCE_DIR}/c++/headers/*.h"
)

file(GLOB project_SRC_SHARED
    "${PROJECT_SOURCE_DIR}/c++/ans_optimal.cc"
    "${PROJECT_SOURCE_DIR}/c++/formulations.cc"
    "${PROJECT_SOURCE_DIR}/c++/optimal_solver.cc"
    "${PROJECT_SOURCE_DIR}/c++/sec_integer.cc"
    "${PROJECT_SOURCE_DIR}/c++/union_find.cc"
    "${PROJECT_SOURCE_DIR}/c++/network.cc"
    "${PROJECT_SOURCE_DIR}/c++/optimal_struct.cc"
    "${PROJECT_SOURCE_DIR}/c++/solver.cc"
    "${PROJECT_SOURCE_DIR}/c++/util_optimal.cc"
    "${PROJECT_SOURCE_DIR}/c++/path_based_formulation.cc"
    "${PROJECT_SOURCE_DIR}/c++/flow_based_relaxed.cc"
    "${PROJECT_SOURCE_DIR}/c++/flow_based_formulation.cc"
    "${PROJECT_SOURCE_DIR}/c++/rooted_tree_formulation.cc"
    "${PROJECT_SOURCE_DIR}/c++/sec_directed_separation.cc"
    "${PROJECT_SOURCE_DIR}/c++/sec_separation.cc"
    "${PROJECT_SOURCE_DIR}/c++/util_solver.cc"
)

ADD_LIBRARY(project_LIB SHARED ${project_SRC_SHARED})
target_include_directories( project_LIB PUBLIC 
    "${PROJECT_SOURCE_DIR}/c++/headers/"
    "${PROJECT_SOURCE_DIR}/../include/gurobi902"
)
target_link_directories(project_LIB PUBLIC "${PROJECT_SOURCE_DIR}/../lib/gurobi902")
target_link_libraries(project_LIB -lgurobi_c++ -lgurobi90 -lm)

#########################################################
#   Compile Project
#########################################################
file(GLOB project_main_SRC
    "${PROJECT_SOURCE_DIR}/c++/main.cc"
    "${PROJECT_SOURCE_DIR}/c++/headers/*.h"
)
add_executable(main ${project_main_SRC}) 

target_include_directories( main PUBLIC 
    "${PROJECT_SOURCE_DIR}/c++/headers/"
    "${PROJECT_SOURCE_DIR}/../include/gurobi902"
)
target_link_directories(main PUBLIC "${PROJECT_SOURCE_DIR}/../lib/gurobi902")

set(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")
set_target_properties(main PROPERTIES COMPILE_FLAGS "-m64 -g -O3 -O2 -O1 -std=c++17")
target_link_libraries(main -m64 -g -O3 -O2 -O1 -std=c++17  -lm project_LIB)

#########################################################
#   Compile Test
#########################################################

enable_testing()
find_package(GTest  REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB project_SRC_TEST
    "${PROJECT_SOURCE_DIR}/test/*.cc"
)
add_executable(mainTest ${project_SRC_TEST}) 

target_include_directories( mainTest PUBLIC 
    "${PROJECT_SOURCE_DIR}/c++/headers/"
    "${PROJECT_SOURCE_DIR}/../include/gurobi902"
)
target_link_directories(mainTest PUBLIC 
    "${PROJECT_SOURCE_DIR}/../lib/gurobi902"
    "${PROJECT_SOURCE_DIR}/lib"
)
include_directories("${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")
set_target_properties(mainTest PROPERTIES COMPILE_FLAGS "-m64 -g -O3 -O2 -O1 -std=c++17")
target_link_libraries(mainTest gtest gtest_main -lpthread -m64 -g -O3 -O2 -O1 -std=c++17 -lm project_LIB)

add_test(mainTest bin/mainTest --gtest_color=yes)

#########################################################
#   CLANG-TIDY
#########################################################
find_program(CLANG_TIDY_EXE NAMES "clang-tidy" DOC "Path to clang-tidy executable") 
    if (NOT CLANG_TIDY_EXE) 
        message(STATUS "clang-tidy not found.") 
    else()  
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}") 
        set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-checks=-*,clang-analyzer-alpha.*,warnings-as-errors=*,readability-identifier-naming,google-*") 
    endif()
#set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-clang-analyzer-alpha.*, -checks=google-*, -warnings-as-errors=*")

set_target_properties( main PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON COMPILE_FLAGS "${WARNING_FLAGS}") 
    if (CLANG_TIDY_EXE)
        set_target_properties(project_LIB PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}") 
        set_target_properties(main PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}") 
    endif()


#########################################################
#   CLANG-FORMAT
#########################################################
find_program(CLANG_FORMAT_EXE NAMES "clang-format" DOC   "Path to clang-format executable") 
    if (NOT CLANG_FORMAT_EXE)
        message(STATUS "clang-format not found.") 
    else() 
        message(STATUS  "clang-format found: " "${CLANG_FORMAT_EXE}") 
        #set(DO_CLANG_FORMAT "${CLANG_FORMAT_EXE}"   "-i -style=google") 
        set_target_properties(main PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}") 
    endif()

if (CLANG_FORMAT_EXE)
    add_custom_target( clang-format-project-files COMMAND ${CLANG_FORMAT_EXE} -i -style=google ${project_SRC} ${project_SRC_TEST}) 
endif()

#########################################################
#########################################################

