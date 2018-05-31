CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(openCLDispatcher)

SET(CMAKE_CXX_STANDARD 11)

#set(OpenCL_INCLUDE_DIR /opt/intel/OpenCL)

FIND_PACKAGE(OpenCL)

ADD_EXECUTABLE(openCLDispatcher

        include/headers.h
        include/globals.h
        include/Environment.h
        include/BitonicSort.h

        main.cpp
        include/data_api.h
        include/kernel_api.h
        include/runtime_api.h
        Query6.h
        include/kernel_generator/selection.h
        include/base_kernel.h
        include/kernel_class/selection.h
        include/kernel_generator/selection_generator.h
        include/DAG_query.h
        include/kernel_class/logical.h include/ImportKernelSource.h)

#INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${OpenCL_INCLUDE_DIR} )

#LINK_DIRECTORIES(/opt/intel/OpenCL)
TARGET_LINK_LIBRARIES( openCLDispatcher LINK_PUBLIC ${OpenCL_LIBRARIES})


#-----------------------------------------------------------------------------------------------------------

#cmake_minimum_required(VERSION 2.0)
#project(openCLDispatcher)

#set(CMAKE_CXX_STANDARD 11)

#add_executable(openCLDispatcher

#include/headers.h
 #               include/globals.h
  #              include/Environment.h
#
 #               main.cpp include/data_api.h include/kernel_api.h include/runtime_api.h Query6.h include/kernel_generator/selection.h include/base_kernel.h include/kernel_class/selection.h include/kernel_generator/selection_generator.h include/DAG_query.h include/kernel_class/logical.h)

#FIND_PACKAGE(Boost 1.58 COMPONENTS system chrono)
#FIND_PACKAGE(OpenCL)

#INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
#INCLUDE_DIRECTORIES( ${OpenCL_INCLUDE_DIR} )

#LINK_DIRECTORIES(/usr/local/cuda-8.0/include)
#TARGET_LINK_LIBRARIES( openCLDispatcher LINK_PUBLIC ${Boost_LIBRARIES} ${OpenCL_LIBRARIES})