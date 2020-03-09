//
// Created by gurumurt on 11/27/18.
//



#include "include/headers.h"
#include "include/Environment.h"
#include "include/data_api.h"
#include "include/kernel_api.h"
#include "include/runtime_api.h"
#include "include/Importkernel.h"
#include <math.h>
#include <chrono>

using namespace std::chrono;


unsigned int *inputData;
size_t ps_size,inputsize;

double CPUtime, GPUtime;

//min + ( std::rand() % ( max - min + 1 ) )
void prepare_data(size_t size = 1024){

    inputsize=size;
    inputData = new unsigned int[size];
    for(size_t i =0;i<size;i++)
        inputData[i] = (unsigned int)( std::rand() % ( size + 1 ) );
}

string selection_block =        "__kernel void selection_CPU"
                                "(\n"
                                "       __global unsigned int* in,\n"
                                "       __global unsigned int* out,\n"
                                "       unsigned int cmp){\n"
                                "\n"
                                "    size_t pos = 32 * get_global_id(0);\n"
                                "    unsigned int bm = 0;\n"
                                "    for (unsigned int i=0; i<32; ++i)\n"
                                "    {\n"
                                "        out[pos] = (in[pos] < cmp);\n"
                                "        pos++;\n"
                                "    }\n"
                                "}";

string selection_coaleased =    "__kernel void selection_GPU"
                                "(\n"
                                "        __global unsigned int* in,\n"
                                "        __global unsigned int* out,\n"
                                "        unsigned int cmp) {\n"
                                "\n"
                                "    size_t ip_pos = get_global_id(0);\n"
                                "    out[ip_pos] = (in[ip_pos] < cmp);\n"
                                "}";

size_t indent = 32;

void testThroughput(){

    setup_environment();
//    print_environment();

    //Print max_work_group_size
//    size_t maxWorkGroup=0;
//    clGetDeviceInfo(device[0][0],CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(size_t),&maxWorkGroup,NULL);
//    cout<<maxWorkGroup<<endl;
//    clGetDeviceInfo(device[1][0],CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(size_t),&maxWorkGroup,NULL);
//    cout<<maxWorkGroup<<endl;

    prepare_data(pow(2,15));

    add_kernel("selection_CPU",device[1][0],selection_block);
    add_kernel("selection_GPU",device[0][0],selection_coaleased);

    add_data("inputGPU",inputData,device[0][0],inputsize);
    add_data("outputGPU",(unsigned int*)calloc((unsigned int)inputsize,sizeof(unsigned int)),device[0][0],inputsize);
    cout<<"GPU transfer time: "<<copyTime<<endl;

    add_data("inputCPU",inputData,device[1][0],inputsize);
    add_data("outputCPU",(unsigned int*)calloc((unsigned int)inputsize,sizeof(unsigned int)),device[1][0],inputsize);

    cout<<"CPU transfer time: "<<copyTime<<endl;

    int selection_value = 56;

    vector<string> argumentsGPU,argumentsCPU;
    vector<int> param;

    argumentsGPU.push_back("inputGPU");
    argumentsGPU.push_back("outputGPU");

    argumentsCPU.push_back("inputCPU");
    argumentsCPU.push_back("outputCPU");

    param.push_back(selection_value);


    //Testing throughput with increasing local size - GPU
    cout<<"LocalSize\tExecutionTime\tThroughput\tcompare"<<endl;
    for(int i =0;i<=15;i++){
        int compare = 1;
        double baseline=0;
        GPUtime=0;
        execute(device[0][0],"selection_GPU",argumentsGPU,param,inputsize,1);
        baseline = nanoSeconds;
        unsigned int *compareRes = get_data("outputGPU",device[0][0],inputsize);
        for(int j=0;j<10000;j++){

            execute(device[0][0],"selection_GPU",argumentsGPU,param,inputsize,pow(2,i));
            unsigned int *Res = get_data("outputGPU",device[0][0],inputsize);
            compare |= memcmp(Res,compareRes,inputsize);
            GPUtime +=nanoSeconds;
        }
//        GPUtime/=100000000000;
        GPUtime/=10000;
        cout<<i
//            <<"\t"<<baseline
            <<"\t"<<GPUtime
            <<"\t"<<inputsize/GPUtime
            <<"\t"<<compare<<endl;
    }
    cout<<"\n\n\n";
    //Testing throughput with increasing local size - CPU
    cout<<"LocalSize\tExecutionTime\tThroughput\tcompare\texecuteLS"<<endl;
    for(int i =0;i<=15;i++){
        int compare = 1;
        double baseline=0;
        CPUtime=0;
        execute(device[1][0],"selection_CPU",argumentsCPU,param,inputsize/32,1);

//      clGetKernelWorkGroupInfo(kernel)
        size_t workGroupSize[3];
        baseline = nanoSeconds;
        unsigned int *compareRes = get_data("outputCPU",device[1][0],inputsize);
        for(int j=0;j<10000;j++){

            execute(device[1][0],"selection_CPU",argumentsCPU,param,inputsize/32,pow(2,i));
            unsigned int *Res = get_data("outputCPU",device[1][0],inputsize);
            compare |= memcmp(Res,compareRes,inputsize);
            CPUtime +=nanoSeconds;
        }
//        CPUtime/=100000000000;
        CPUtime/=10000;
        cout<<i
//            <<"\t"<<baseline
            <<"\t"<<CPUtime
            <<"\t"<<inputsize/CPUtime
            <<"\t"<<compare<<endl;
    }
}