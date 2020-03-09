//
// Created by gurumurt on 3/22/18.
//

#ifndef OPENCLTEST_DATA_API_H
#define OPENCLTEST_DATA_API_H

#endif //OPENCLTEST_DATA_API_H

#pragma once
#include "headers.h"
#include "globals.h"
#include "runtime_api.h"


double copyTime;

void delete_data(string name, cl_device_id source);


template <typename T>
void add_data(string alias, T *data, cl_device_id device, size_t size,size_t m = 1){

    if(data_dictionary.count(make_pair(alias,device)))
        return;

    cl_context context = queue[device].context;
    cl_command_queue device_queue = queue[device].queue;

    //Create data buffer on device
    data_buffer = clCreateBuffer(context, mem_flag,(size+1)*m* sizeof(T),NULL,&err);

    if(err !=CL_SUCCESS){
        cerr<<"Error on creating buffer- "<< alias <<" ( "<<err<<" )"<<endl;
        exit(0);
    }


    cl_event writeEvent;
    //Add data into the device
    err = clEnqueueWriteBuffer(device_queue,data_buffer,CL_TRUE,0,size*sizeof(T),
                               data,0,NULL,&writeEvent);

    clWaitForEvents(1,&writeEvent);
    clFinish(queue[device].queue);

    cl_ulong time_start=0;
    cl_ulong time_end=0;

    clGetEventProfilingInfo(writeEvent, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
    clGetEventProfilingInfo(writeEvent, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
    copyTime = time_end-time_start;

    if(err !=CL_SUCCESS){
        cerr<<"Error on Write buffers- "<<alias <<" - size ( "<< size<<" ) - ( "<<err<<" )"<<endl;
    }

    //    cout<<"err: "<<err<<endl;
    data_dictionary.insert(make_pair(make_pair(alias,device),data_buffer));

    clFinish(queue[device].queue);
    clFlush(queue[device].queue);
}

template <typename T>
void add_new_data(string alias, cl_device_id device, size_t size,size_t m = 1){

    T* temp = (T*)calloc(size, sizeof(unsigned int));
    add_data(alias,temp,device,size);
    delete temp;
}


void lookup_data_buffer(){

    for(auto it = data_dictionary.cbegin(); it != data_dictionary.cend(); ++it)    {
        char name[100];
        clGetDeviceInfo(it->first.second,CL_DEVICE_NAME,100,&name,NULL);
        cout<< it->first.first<<"-->"<<name<<endl;
    }

}

unsigned int* result_data;

cl_event read_event;

unsigned int* get_data(string name, cl_device_id device, size_t global_size,bool flag_bits=false){

    err = 0;
    cl_mem argument_buffer = data_dictionary.find(make_pair(name,device))->second;

    if(!result_data)
            delete result_data;

    result_data = (unsigned int *) calloc((unsigned int)global_size, sizeof(unsigned int ));
//            new unsigned int[global_size];
    err = clEnqueueReadBuffer(queue[device].queue,argument_buffer,CL_TRUE,0,global_size*sizeof(unsigned int), result_data , 0, NULL, &read_event);

    clFinish(queue[device].queue);
    clFlush(queue[device].queue);
    clWaitForEvents(1,&read_event);
    return result_data;
}


unsigned int get_first_data(string name, cl_device_id device, size_t global_size,bool flag_bits=false){

    err = 0;
    cl_mem argument_buffer = data_dictionary.find(make_pair(name,device))->second;

    int result_data;
    err = clEnqueueReadBuffer(queue[device].queue,argument_buffer,CL_TRUE,0,sizeof(unsigned int), &result_data , 0, NULL, &read_event);

    clFinish(queue[device].queue);
    clFlush(queue[device].queue);
    clWaitForEvents(1,&read_event);
    return result_data;

}

unsigned int get_last_data(string name, cl_device_id device, size_t global_size,bool flag_bits=false){

    err = 0;
    cl_mem argument_buffer = data_dictionary.find(make_pair(name,device))->second;

    int result_data;
    err = clEnqueueReadBuffer(queue[device].queue,argument_buffer,CL_TRUE,((int)global_size - 1)*sizeof(unsigned int),sizeof(unsigned int), &result_data , 0, NULL, &read_event);

    clFinish(queue[device].queue);
    clFlush(queue[device].queue);
    clWaitForEvents(1,&read_event);
    return result_data;
}

void print_data(string name, cl_device_id device, size_t global_size,bool flag_bits=false){

    err = 0;
    cl_mem argument_buffer = data_dictionary.find(make_pair(name,device))->second;

    cl_event read_event;

    unsigned int* result = new unsigned int[global_size];
    err = clEnqueueReadBuffer(queue[device].queue,argument_buffer,CL_TRUE,0,global_size*sizeof(unsigned int), result, 0, NULL, &read_event);

    clFinish(queue[device].queue);
    clFlush(queue[device].queue);

//    cout<<"err: "<<err<<endl;
    if(flag_bits){
        for(int i=0;i<2;i++){
            std::bitset<32> x(result[i]);
            std::cout << x<<endl;
        }
    }

    else {
        for (int i = 0; i < global_size; i++) {
            cout << result[i] << endl;
        }
    }

}

void clear_data(){

     data_dictionary.clear();
    kernel_dictionary.clear();

}

void clear_data(vector<string> args,cl_device_id device){

    //clReleaseMemObject
//    for(int i=0;i<args.size();i++){
//
//        cl_mem argument_buffer = data_dictionary.find(make_pair(args[i],device))->second;
//        err |= clReleaseMemObject(argument_buffer);
//        if (err != CL_SUCCESS) {
//            std::cerr << ": Error releasing buffer arguments (" << err
//                      << ")." << std::endl;
//            return;
//        }
//    }
    data_dictionary.clear();
    kernel_dictionary.clear();

}


void transfer_data(string name,cl_device_id source, cl_device_id target,size_t size){

    //Get data from source
    cl_mem argument_buffer = data_dictionary.find(make_pair(name,source))->second;

    unsigned int* result = new unsigned int[size];
    cl_int err = clEnqueueReadBuffer(queue[source].queue,argument_buffer,CL_TRUE,0,size*sizeof(unsigned int), result, 0, NULL, NULL);

    //Copy data to target
    add_data(name,result,target,size);
    delete_data(name,source);
}

void delete_data(string name, cl_device_id source){

//    cout<<"the data to be deleted is "<<data_dictionary.find(make_pair(name,source))->second<<endl;

    if(!data_dictionary.count(make_pair(name,source)))
        return;

    clReleaseMemObject(data_dictionary.find(make_pair(name,source))->second);

    //Remove from data dictionary
    for(multimap<pair<string, cl_device_id>, cl_mem>::iterator it = data_dictionary.begin(); it != data_dictionary.end(); it++)
    {
        if((it->first) == make_pair(name,source))
        {
            data_dictionary.erase(it);
            break;
        }
    }
}

/**
 * Removes all data from device memory
 */
void delete_all_data(){

    //Remove from data dictionary
    for(multimap<pair<string, cl_device_id>, cl_mem>::iterator it = data_dictionary.begin(); it != data_dictionary.end(); it++)
    {
        clReleaseMemObject(it->second);
        data_dictionary.erase(it);
    }
}