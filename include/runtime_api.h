//
// Created by gurumurt on 3/22/18.
//

#ifndef OPENCLTEST_RUNTIME_API_H
#define OPENCLTEST_RUNTIME_API_H

#endif //OPENCLTEST_RUNTIME_API_H

#pragma once

#include "globals.h"

template<typename T>
void execute(cl_device_id device, string kernel_name, cl_event _event, vector<string> args, vector<T> param, size_t global_size = NULL,
             size_t local_size = NULL) {

    //Set the kernel arguments
    cl_kernel kernel = kernel_dictionary.find(make_pair(kernel_name, device))->second;
    short args_size = (short) args.size();
    for (int i = 0; i < args_size; i++) {

        cl_mem argument_buffer = data_dictionary.find(make_pair(args[i], device))->second;
        err |= clSetKernelArg(kernel, i, sizeof(cl_mem), &argument_buffer);
        if (err != CL_SUCCESS) {
            std::cerr << ": Error setting kernel arguments (" << err
                      << ")." << std::endl;
            return;
        }
    }
//    cout<<args_size<<endl;
    //Add parameters
    short param_size = (short) param.size();
    for (int i = 0; i < param_size; i++) {
//        cout<<param[i];
        err |= clSetKernelArg(kernel, i + args_size, sizeof(T), &param[i]);
        if (err != CL_SUCCESS) {
            std::cerr << ": Error setting kernel parameter arguments (" << err
                      << ")." << std::endl;
            return;
        }
    }

    //Execute kernel
    err = clEnqueueNDRangeKernel(queue[device].queue, kernel, 1, NULL, &global_size, &local_size,
                                 0, NULL, &event);

    clWaitForEvents(1, &event);

    clFinish(queue[device].queue);
}