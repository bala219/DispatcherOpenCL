//
// Created by gurumurt on 3/22/18.
//

#ifndef OPENCLTEST_DATA_API_H
#define OPENCLTEST_DATA_API_H

#endif //OPENCLTEST_DATA_API_H

#pragma once

#include "globals.h"

template <typename T>
void add_data(string alias, T *data, cl_device_id device, size_t size){

    cl_context context = queue[device].context;
    cl_command_queue device_queue = queue[device].queue;

    //Create data buffer on device
    cl_mem data_buffer = clCreateBuffer(context, mem_flag,size * sizeof(T),NULL,NULL);

    //Add data into the device
    err = clEnqueueWriteBuffer(device_queue,data_buffer,CL_TRUE,0,size*sizeof(T),data,0,NULL,NULL);
//    cerr<<": Error from preparing data (" << err
//                                          <<")." << endl;

    data_dictionary.insert(make_pair(make_pair(alias,device),data_buffer));
}

void lookup_data_buffer(){

    for(auto it = data_dictionary.cbegin(); it != data_dictionary.cend(); ++it)    {
        char name[100];
        clGetDeviceInfo(it->first.second,CL_DEVICE_NAME,100,&name,NULL);
        cout<< it->first.first<<"-->"<<name<<endl;
    }

}

void print_data(string name, cl_device_id device, size_t global_size,bool flag_bits=false){

    err = 0;
    cl_mem argument_buffer = data_dictionary.find(make_pair(name,device))->second;

    unsigned int* result = new unsigned int[global_size];
    err = clEnqueueReadBuffer(queue[device].queue,argument_buffer,CL_TRUE,0,global_size*sizeof(unsigned int), result, 0, NULL, NULL);

    if(flag_bits){
        for(int i=0;i<2;i++){
            std::bitset<32> x(result[i]);
            std::cout << x <<endl;
        }
    }

    else {
        for (int i = 0; i < global_size; i++) {
            cout << result[i] << endl;
        }
    }


}

void transfer_data(string name,cl_device_id source, cl_device_id target,size_t size){

    //Get data from source
    cl_mem argument_buffer = data_dictionary.find(make_pair(name,source))->second;

    unsigned int* result = new unsigned int[size];
    cl_int err = clEnqueueReadBuffer(queue[source].queue,argument_buffer,CL_TRUE,0,size*sizeof(unsigned int), result, 0, NULL, NULL);

    //Copy data to target
    add_data(name,result,target,size);
}

void delete_data(string name, cl_device_id source){
    clReleaseMemObject(data_dictionary.find(make_pair(name,source))->second);
}