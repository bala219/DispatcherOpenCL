//
// Created by gurumurt on 3/22/18.
//

#ifndef OPENCLTEST_RUNTIME_API_H
#define OPENCLTEST_RUNTIME_API_H

#endif //OPENCLTEST_RUNTIME_API_H

#pragma once
#include "headers.h"
#include "globals.h"

cl_event e;


void extractExecutionTime(cl_event *ev = &e){

    cl_ulong time_start=0;
    cl_ulong time_end=0;

    clGetEventProfilingInfo(*ev, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
    clGetEventProfilingInfo(*ev, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
    nanoSeconds = time_end-time_start;
}

template <typename T>
void execute(cl_device_id device, string kernel_name, vector<string> argms, vector<T> param, size_t global_size,
             size_t local_size = 1, short add_local_buffer = 0,short add_local_buffer_factor =1,  cl_event *ev = &e) {

    //Set the kernel arguments
    pair<string,cl_device_id > in_pair = make_pair(kernel_name,device);
    cl_kernel kernel = kernel_dictionary.find(in_pair)->second;
    short args_size = (short) argms.size();
    size_t localBuffer = local_size*sizeof(unsigned int)*add_local_buffer_factor;

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
            nanoSeconds = 0;
            return;
        }
    }

    if(add_local_buffer){

        err|=clSetKernelArg(kernel,args_size,localBuffer,NULL);
        if(err !=CL_SUCCESS){
            std::cerr << ": Error setting local arguments (" << err
                      << ")-"
                      <<kernel_name
                      << std::endl;
            nanoSeconds = 0;
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

    if (err != CL_SUCCESS) {
        std::cerr << ": Error in execution of a kernel (" << err
                  << ")-"
                  <<kernel_name
                  << " - ( "
                  <<global_size
                  <<" : "
                  <<local_size
                  <<" ) "
                  << std::endl;
//        exit(0);
        return;
    }

    clWaitForEvents(1,ev);
    clFinish(queue[device].queue);
    clFlush(queue[device].queue);
    extractExecutionTime();
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
            return ev;
        }
    }

    //Execute kernel
    err = clEnqueueNDRangeKernel(queue[device].queue, kernel, 1, NULL, &global_size, &local_size,
                                 evetSize, evdep,  ev);
    return ev;
}

void executeGraph(cl_device_id device){
    clFinish(queue[device].queue);
}



//template <typename T>
//void execute_test(cl_device_id device, string kernel_name,vector<string> argms, vector<T> param,size_t global_size, size_t local_size= NULL,short add_local_buffer = 0, short num_of_iteration = 100, cl_event *ev = &e){
//
//
//    //Set the kernel arguments
//    pair<string,cl_device_id > in_pair = make_pair(kernel_name,device);
//    cl_kernel kernel = kernel_dictionary.find(in_pair)->second;
//    short args_size = (short) argms.size();
//    for(int i=0;i<args_size;i++){
//        pair<string,cl_device_id > buf_pair = make_pair(argms[i],device);
//        cl_mem argument_buffer = data_dictionary.find(buf_pair)->second;
//        err |= clSetKernelArg(kernel, i, sizeof(cl_mem), &argument_buffer);
//        if (err != CL_SUCCESS) {
//            std::cerr << ": Error setting kernel arguments (" << err
//                      << ")-"
//                      <<kernel_name
//                      <<" - arg "
//                      <<argms[i]
//                      << std::endl;
//            return;
//        }
//    }
//
//    //Add parameters
//    short param_size = (short) param.size();
//    for(int i=0;i<param_size;i++){
//        err |= clSetKernelArg(kernel, i+args_size, sizeof(T), &param[i]);
//        if (err != CL_SUCCESS) {
//            std::cerr << ": Error setting kernel parameter arguments (" << err
//                      << ")-"
//                      <<kernel_name
//                      << device
//                      << std::endl;
//            return;
//        }
//    }
//
//    nanoSeconds = 0;
//    //Execution must be done multiple times
//    for(int i =0;i<num_of_iteration;i++){
//        err = clEnqueueNDRangeKernel(queue[device].queue, kernel, 1, NULL, &global_size, &local_size,
//                                     0, NULL,  ev);
//        clWaitForEvents(1,&e);
//        clFinish(queue[device].queue);
//        cl_ulong time_start=0;
//        cl_ulong time_end=0;
//        clGetEventProfilingInfo(e, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
//        clGetEventProfilingInfo(e, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
//        nanoSeconds += time_end-time_start;
//        clFlush(queue[device].queue);
//        clFinish(queue[device].queue);
//    }
//    nanoSeconds/=num_of_iteration;
//
//}


void setEvents(){}