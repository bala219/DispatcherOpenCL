//
// Created by gurumurt on 3/22/18.
//

#ifndef OPENCLTEST_KERNEL_API_H
#define OPENCLTEST_KERNEL_API_H

#endif //OPENCLTEST_KERNEL_API_H

#pragma once

#include <cstring>
#include "headers.h"
#include "globals.h"

void delete_kernel(string kernel_name, cl_device_id device){

    pair<string,cl_device_id > in_pair = make_pair(kernel_name,device);
    cl_kernel kernel = kernel_dictionary.find(in_pair)->second;
    clReleaseKernel(kernel);

    for(multimap<pair<string, cl_device_id>, cl_kernel>::iterator it = kernel_dictionary.begin(); it != kernel_dictionary.end(); it++)
    {
        if((it->first) == make_pair(kernel_name,device))
        {
            kernel_dictionary.erase(it);
            break;
        }
    }
}

cl_kernel build_kernel(const char* source, const char* kernel_name,cl_device_id device,string _command_arg){

    cl_context context = queue[device].context;
    cl_int er[2];
    cl_program program = clCreateProgramWithSource(context, 1, &source, NULL, er);
    cl_int err = clBuildProgram(program, 1, &device, _command_arg.c_str(), NULL, NULL);

    if (err != CL_SUCCESS) {
        size_t LogSize;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                              0, NULL, &LogSize);
        char log[LogSize];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                              LogSize, log, NULL);
        std::cerr << ": Error building kernel "
                  << kernel_name << "." <<"( "<< err << " )" << std::endl;
        std::cerr << log << std::endl;
        return NULL;
    }

    cl_kernel kernel = clCreateKernel(program, kernel_name, &err);

    return kernel;
}

void add_kernel(string kernel_name, cl_device_id device, string kernel_src, string _command_arg=""){

    if(kernel_dictionary.count(make_pair(kernel_name,device))){
//        return;

        for(multimap<pair<string, cl_device_id>, cl_kernel>::iterator it = kernel_dictionary.begin(); it != kernel_dictionary.end(); it++)
        {
            clReleaseKernel(it->second);
            if((it->first) == make_pair(kernel_name,device))
            {
                kernel_dictionary.erase(it);
                break;
            }
        }
    }

    //Set kernel information

    char *src = new char[kernel_src.length() + 1];
    strcpy(src, kernel_src.c_str());

    char *name = new char[kernel_name.length() + 1];
    strcpy(name, kernel_name.c_str());

    cl_kernel kernel = build_kernel(src, name, device,_command_arg);

    kernel_dictionary.insert(make_pair(make_pair(kernel_name,device),kernel));
}

void delete_all_kernel(){

    //Remove from data dictionary
    for(multimap<pair<string, cl_device_id>, cl_kernel>::iterator it = kernel_dictionary.begin(); it != kernel_dictionary.end(); it++)
    {
        clReleaseKernel(it->second);
        kernel_dictionary.erase(it);
    }
}