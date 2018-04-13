//
// Created by gurumurt on 3/22/18.
//

#ifndef OPENCLTEST_ENVIRONMENT_H
#define OPENCLTEST_ENVIRONMENT_H

#endif //OPENCLTEST_ENVIRONMENT_H

#pragma once
#include "headers.h"
#include "globals.h"


void setup_environment(){

    //Get the platforms
    err = clGetPlatformIDs(0,NULL,&platform_count);
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
        clGetDeviceIDs(platform[i],CL_DEVICE_TYPE_ALL,device_local_count[i],device[i],NULL);

        //Creating context for single platform
        context[i] = clCreateContext(NULL,device_local_count[i],device[i],NULL,NULL,&err);

        //Command queue for each devices
        for(int j=0;j<device_local_count[i];j++){

            device_queue dq;
            dq.context = context[i];
            dq.queue = clCreateCommandQueue(context[i],device[i][j],cqp,&err);

            //Combined queue
            queue.insert(make_pair(device[i][j],dq));
        }
    }
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