//
// Created by gurumurt on 11/6/18.
//

#ifndef EXEC_ENGINE_COMPARISON_RUNTIME_API_H
#define EXEC_ENGINE_COMPARISON_RUNTIME_API_H

#pragma once
#include "headers.h"
#include "globals.h"

namespace dispatcher{

    cl_event e;

    void extractExecutionTime(cl_event *ev = &e){

        cl_ulong time_start=0;
        cl_ulong time_end=0;

        clGetEventProfilingInfo(*ev, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
        clGetEventProfilingInfo(*ev, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
        nanoSeconds = time_end-time_start;
    }

    double getDuration(cl_event e){

    cl_ulong time_start=0;
    cl_ulong time_end=0;

    clGetEventProfilingInfo(e, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
    clGetEventProfilingInfo(e, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
    nanoSeconds = time_end-time_start;

        return nanoSeconds;
    }


    template <typename T>
    void execute(cl_device_id device, string kernel_name, vector<string> argms, vector<T> param, size_t global_size,
                 size_t local_size = NULL, cl_event *ev = &e, short add_local_buffer = 0) {

        //Set the kernel arguments
        pair<string,cl_device_id > in_pair = make_pair(kernel_name,device);
        cl_kernel kernel = kernel_dictionary.find(in_pair)->second;
        short args_size = (short) argms.size();
        for(int i=0;i<args_size;i++){

            pair<string,cl_device_id > buf_pair = make_pair(argms[i],device);
            cl_mem argument_buffer = data_dictionary.find(buf_pair)->second;
            err |= clSetKernelArg(kernel, i, sizeof(cl_mem), &argument_buffer);
            if (err != CL_SUCCESS) {
                std::cerr << ": Error setting kernel arguments (" << err
                          << ")-"
                          <<kernel_name
                          <<" - arg "
                          <<argms[i]
                          << std::endl;
                return;
            }
        }

        //Add parameters
        short param_size = (short) param.size();
        for(int i=0;i<param_size;i++){
            err |= clSetKernelArg(kernel, i+args_size, sizeof(T), &param[i]);
            if (err != CL_SUCCESS) {
                std::cerr << ": Error setting kernel parameter arguments (" << err
                          << ")-"
                          <<kernel_name
                          << device
                          << std::endl;
                return;
            }
        }

        //Execute kernel

        err = clEnqueueNDRangeKernel(queue[device].queue, kernel, 1, NULL, &global_size, &local_size,
                                     0, NULL,  ev);
        clWaitForEvents(1,&e);
        clFinish(queue[device].queue);
//        cl_ulong time_start=0;
//        cl_ulong time_end=0;
//
//    clGetEventProfilingInfo(e, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
//    clGetEventProfilingInfo(e, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
//    nanoSeconds = time_end-time_start;
//
//    clFlush(queue[device].queue);
//    clFinish(queue[device].queue);
//    clWaitForEvents(1,&e);

        extractExecutionTime();
        //Remove buffers
    }

    template <typename T>
    cl_event* queueExecution(cl_device_id device, string kernel_name, vector<string> argms, vector<T> param, size_t global_size=100,
                             cl_event *evdep = nullptr,size_t evetSize = 0, size_t local_size = NULL) {

        cl_event *ev = &evt[num_of_available_events++];
        //Set the kernel arguments
        pair<string,cl_device_id > in_pair = make_pair(kernel_name,device);
        cl_kernel kernel = kernel_dictionary.find(in_pair)->second;
        short args_size = (short) argms.size();
        for(int i=0;i<args_size;i++){

            pair<string,cl_device_id > buf_pair = make_pair(argms[i],device);
            cl_mem argument_buffer = data_dictionary.find(buf_pair)->second;
            err |= clSetKernelArg(kernel, i, sizeof(cl_mem), &argument_buffer);
            if (err != CL_SUCCESS) {
                std::cerr << ": Error setting kernel arguments (" << err
                          << ")-"
                          <<kernel_name
                          <<" - arg "
                          <<argms[i]
                          << std::endl;
                err=0;
                return ev;
            }
        }

        //Add parameters
        short param_size = (short) param.size();
        for(int i=0;i<param_size;i++){
            err |= clSetKernelArg(kernel, i+args_size, sizeof(T), &param[i]);
            if (err != CL_SUCCESS) {
                std::cerr << ": Error setting kernel parameter arguments (" << err
                          << ")-"
                          <<kernel_name
                          << device
                          << std::endl;
                err=0;
                return ev;
            }
        }

        //Execute kernel

        err = clEnqueueNDRangeKernel(queue[device].queue, kernel, 1, NULL, &global_size, &local_size,
                                     evetSize, evdep,  ev);

        if (err != CL_SUCCESS) {
            std::cerr << ": Error executing kernel (" << err
                      << ")-"
                      <<kernel_name
                      <<" in device "
                      << device
                      << std::endl;
            err=0;
            return ev;
        }

        return ev;
    }

    void executeGraph(cl_device_id device){
        clFinish(queue[device].queue);
    }

    void setEvents(){}

}

#endif //EXEC_ENGINE_COMPARISON_RUNTIME_API_H
