//
// Created by gurumurt on 3/22/18.
//

#ifndef OPENCLTEST_ENVIRONMENT_H
#define OPENCLTEST_ENVIRONMENT_H

#endif //OPENCLTEST_ENVIRONMENT_H

#pragma once
#include "headers.h"
#include "globals.h"

void pfn_notify(const char *errinfo, const void *private_info, size_t cb, void *user_data)
{
    cout<<"Here is error info : "<< errinfo<<endl;
    fprintf(stderr, "OpenCL Error (via pfn_notify): %s\n", errinfo);
}

void setup_environment(){

    //Get the platforms
    err = clGetPlatformIDs(0,NULL,&platform_count);
    if(err != CL_SUCCESS){

        cerr<<"Error in platform creation"<<endl;
        cout<<"Error in platform creation"<<endl;
        exit(0);
    }
    platform = (cl_platform_id *)malloc(sizeof(cl_platform_id) * platform_count);

    context = (cl_context*)malloc(sizeof(cl_context)*platform_count);

    //Allocate space for devices
    err = clGetPlatformIDs(platform_count,platform,NULL);
    device = new cl_device_id*[platform_count];
    device_local_count = new cl_uint[platform_count];
    //Get device data
    for(int i =0;i<platform_count;i++){

        //Creating context
        //Getting the number of devices in the current platform

        clGetDeviceIDs(platform[i],CL_DEVICE_TYPE_ALL,NULL,NULL,&device_local_count[i]);
        //Getting the devices
        device[i] = new cl_device_id[device_local_count[i]];
        err |= clGetDeviceIDs(platform[i],CL_DEVICE_TYPE_ALL,device_local_count[i],device[i],NULL);
        //Creating context for single platform
        err=0;

        cl_context_properties clp[]={
                CL_CONTEXT_PLATFORM,
                (cl_context_properties)platform[i],
                0
        };

        context[i] = clCreateContext(clp,device_local_count[i],device[i],NULL,NULL,&err);

        if(err != CL_SUCCESS){

            cerr<<"Context creation error of device: "<< i
                                                << "  - error : "<< err <<endl;

            cout<<"Context creation error of device: "<< i
                                                << "  - error : "<< err <<endl;
        }

        //Command queue for each devices
        for(int j=0;j<device_local_count[i];j++){

            device_queue dq;
            dq.context = context[i];
            dq.queue = clCreateCommandQueue(context[i],device[i][j],cqp,&err);

            //Combined queue
            queue.insert(make_pair(device[i][j],dq));
        }
    }

    evt = new cl_event[100];

}

void print_environment(){

    for(int i=0;i<platform_count;i++){
        char name[1024];
        clGetPlatformInfo(platform[i],CL_PLATFORM_NAME,1024,&name,NULL);
        cout<<name<<"\t-"<< i <<endl;
        cout<<"--------------------"<<endl;

        for(int j=0;j<device_local_count[i];j++){
            clGetDeviceInfo(device[i][j],CL_DEVICE_NAME,1024,&name,NULL);
            cout<<"\t\t\t\t"<<name<<"\t-"<< j <<endl;
        }
        cout<<"\n"<<endl;
    }
}