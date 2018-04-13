cmake_minimum_required(VERSION 3.9)
project(openCLDispatcher)

set(CMAKE_CXX_STANDARD 11)

add_executable(openCLDispatcher

                include/headers.h
                include/globals.h
                include/Environment.h

                main.cpp include/data_api.h include/kernel_api.h include/runtime_api.h Query6.h include/kernel_generator/selection.h include/base_kernel.h include/kernel_class/selection.h include/kernel_generator/selection_generator.h include/DAG_query.h include/kernel_class/logical.h)

FIND_PACKAGE(Boost 1.58 COMPONENTS system chrono)
FIND_PACKAGE(OpenCL)

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${OpenCL_INCLUDE_DIR} )

LINK_DIRECTORIES(/usr/local/cuda-8.0/include)
TARGET_LINK_LIBRARIES( openCLDispatcher LINK_PUBLIC ${Boost_LIBRARIES} ${OpenCL_LIBRARIES})