//
// Created by gurumurt on 6/29/18.
//

#ifndef OPENCLDISPATCHER_TEST_LOCAL_H
#define OPENCLDISPATCHER_TEST_LOCAL_H

#endif //OPENCLDISPATCHER_TEST_LOCAL_H


#include "include/headers.h"
#include "include/Environment.h"
#include "include/data_api.h"
#include "include/kernel_api.h"
#include "include/runtime_api.h"

void test_local(){

    setup_environment();
    print_environment();

    cl_device_id CPU, GPU;

    int a[1024];
    for(int i=0;i<1024;i++){
        a[i] = i+1;
    }

    GPU = device[0][0];
    CPU = device[1][0];

    //Add data to the respective devices
    add_data("a",a,CPU, 1024);
//    add_data("b", nullptr,CPU, 1024);


    add_data("a",a,GPU,1024);
//    add_data("b",NULL,GPU,100);

    //Add kernels to the respective devices
    string kernel_source = "void kernel branched_aggregate_local(\n"
            "        __global const int* source,\n"
            "        __local int* dest\n"
            ") {\n"
            "size_t local_offset = get_local_id(0)*32;\n"
            "size_t global_offset = get_global_id(0)*32;\n"
            "event_t event;\n"
            "async_work_group_copy(&dest[local_offset],&source[global_offset],32,event);\n"
            "\n"
            "wait_group_events(1,&event);\n"
            "\n"
            "size_t pos = 32 * get_global_id(0);\n"
            "for (unsigned int i = 0;i<32; ++i){\n"
            "\n"
            "    printf(\"Thread:%d--> %d , (%d,%d)\\n\",get_global_id(0),source[get_global_id(0)],source[i+ global_offset],dest[i + local_offset]);\n"
            "    }\n"
            "}";
    string kernel_name = "branched_aggregate_local";

    cout<<kernel_source<<endl;

    add_kernel(kernel_name, GPU, kernel_source);
    add_kernel(kernel_name, CPU, kernel_source);

    vector<string> arguments;
    arguments.push_back("a");
//    arguments.push_back("b");
//    arguments.push_back("c");

    vector<int> param;
//    execute(GPU, kernel_name,arguments,param,100);
    execute(CPU, kernel_name,arguments,param,32,4);
    cout<<"GPU kernel\n\n\n";
    execute(GPU, kernel_name,arguments,param,32,4);
}