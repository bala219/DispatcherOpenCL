CMAKE_MINIMUM_REQUIRED(VERSION 3.5) #cmake_minimum_required(VERSION 3.10)
PROJECT(openCLDispatcher)

SET(CMAKE_CXX_STANDARD 11) #set(CMAKE_CXX_STANDARD 98)

#set(OpenCL_INCLUDE_DIR /opt/intel/OpenCL)

FIND_PACKAGE(OpenCL)

ADD_EXECUTABLE(openCLDispatcher

        Main.h
        Main.cpp
        Query6.h

        include/kernel_class/logical.h
        include/kernel_class/selection.h
        include/kernel_generator/selection.h
        include/kernel_generator/selection_generator.h

        include/headers.h
        include/globals.h
        include/Environment.h
        include/data_api.h
        include/kernel_api.h
        include/runtime_api.h
        include/base_kernel.h
        include/DAG_query.h
        include/ImportKernelSource.h


        include/primitives/header/BitonicSorting.h
        #include/primitives/source/BitonicSorting.cpp
        include/primitives/header/Merging.h
        #include/primitives/header/Merging.cpp
        include/primitives/header/Aggregating.h
        #include/primitives/source/Aggregation.cpp
        include/primitives/header/Aggregation.h

        include/distribution.h)

#INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${OpenCL_INCLUDE_DIR} )

#LINK_DIRECTORIES(/opt/intel/OpenCL)
TARGET_LINK_LIBRARIES( openCLDispatcher LINK_PUBLIC ${OpenCL_LIBRARIES})


#-----------------------------------------------------------------------------------------------------------

#cmake_minimum_required(VERSION 2.0)
#project(openCLDispatcher)

#set(CMAKE_CXX_STANDARD 11)

#add_executable(openCLDispatcher

#               main.cpp
#               Query6.h
#               include/headers.h
#               include/globals.h
#               include/Environment.h
#               include/data_api.h
#               include/kernel_api.h
#               include/runtime_api.h
#               include/DAG_query.h
#               include/base_kernel.h
#               include/kernel_generator/selection.h
#               include/kernel_class/selection.h
#               include/kernel_generator/selection_generator.h
#               include/kernel_class/logical.h)

#FIND_PACKAGE(Boost 1.58 COMPONENTS system chrono)
#FIND_PACKAGE(OpenCL)

#INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
#INCLUDE_DIRECTORIES( ${OpenCL_INCLUDE_DIR} )

#LINK_DIRECTORIES(/usr/local/cuda-8.0/include)
#TARGET_LINK_LIBRARIES( openCLDispatcher LINK_PUBLIC ${Boost_LIBRARIES} ${OpenCL_LIBRARIES})