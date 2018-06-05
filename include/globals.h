//
// Created by gurumurt on 3/19/18.
//

#ifndef OPENCLTEST_GLOBALS_H
#define OPENCLTEST_GLOBALS_H

#endif //OPENCLTEST_GLOBALS_H

#pragma once

#include "headers.h"
//#include "kernel_store.h"

static int count;
cl_event* evt;

cl_event event;

cl_int err;

const cl_command_queue_properties cqp = CL_QUEUE_PROFILING_ENABLE;
const cl_mem_flags mem_flag = CL_MEM_READ_WRITE;

const cl_profiling_info cpiS = CL_PROFILING_COMMAND_START;
const cl_profiling_info cpiE = CL_PROFILING_COMMAND_END;


cl_platform_id  * platform;
cl_uint platform_count;

cl_uint *device_local_count;
cl_device_id ** device;

cl_context * context;


struct device_queue{
    cl_context context;
    cl_command_queue queue;
};

enum Param_Type {
    INTEGER,
    FLOATER,
    DOUBLER,
    CHARER,
    STRINGER
};


map<cl_device_id, device_queue> queue;

multimap<pair<string, cl_device_id>, cl_mem> data_dictionary;

map<pair<string, cl_device_id>,cl_kernel> kernel_dictionary;