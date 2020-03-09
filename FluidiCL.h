#include "include/headers.h"
#include "include/Environment.h"
#include "include/data_api.h"
#include "include/kernel_api.h"
#include "include/runtime_api.h"
#include "include/Importkernel.h"


//generate data

unsigned int* selectionDataGenerate(size_t size){


    add_data("sequential",(unsigned int*)calloc((unsigned int)size,sizeof(unsigned int)),device[0][0],size);
    vector<string> arg;
    vector<int> param;
    arg.push_back("sequential");
    string kern = readKernelFile("kernels/SeqGen.cl");
    add_kernel("SeqGen",device[0][0],kern);
    execute(device[0][0],"SeqGen",arg,param,size,256);

    return get_data("sequential",device[0][0],size);


}


void test(){

    double Gtransfer=0, Ctransfer=0;
    setup_environment();

    string selectionKernel = readKernelFile("kernels/selection_block.cl");
    string selectionKernelInterleaved = readKernelFile("kernels/selection_interleaved.cl");

    add_kernel("selection_interleaved",device[0][0],selectionKernelInterleaved);
    add_kernel("selection_block",device[1][0],selectionKernel);

    size_t size = pow(2,19);

    unsigned int* input = selectionDataGenerate(size);

    size_t deviceSize = size/2;

    cout<<size<<" - size : "<<deviceSize<<" - DeviceSize"<<endl;

    add_data("data1",input,device[0][0],deviceSize);
    Gtransfer +=copyTime;
    add_data("data2",&input[deviceSize],device[1][0],deviceSize);
    Ctransfer +=copyTime;
    add_data("output1",
             (unsigned int*)calloc((unsigned int)deviceSize/32,sizeof(unsigned int)),
             device[0][0],deviceSize);
    Gtransfer +=copyTime;
    add_data("output2",
             (unsigned int*)calloc((unsigned int)deviceSize/32,sizeof(unsigned int)),
             device[1][0],deviceSize);
    Ctransfer +=copyTime;

    vector<string> args1;
    vector<string> args2;

    vector<int> param;
    param.push_back(40);

    args1.push_back("data1");
    args1.push_back("output1");

    args2.push_back("data2");
    args2.push_back("output2");

    cout<<"Combined : "<<endl;
    execute(device[0][0],"selection_interleaved",args1,param,deviceSize,(deviceSize>1024)?1024:deviceSize);
    cout<<"GPU :"<<nanoSeconds <<"\t"<< Gtransfer<<endl;
    execute(device[1][0],"selection_block",args2,param,deviceSize/32,(deviceSize/32>8192)?8192:deviceSize/32);
    cout<<"CPU :"<<nanoSeconds  <<"\t"<<  Ctransfer<<endl;


    unsigned int *GPURes = get_data("output1",device[0][0],deviceSize/32,true);
    unsigned int *CPURes = get_data("output2",device[1][0],deviceSize/32,true);


    unsigned int *Total = (unsigned int*)calloc((unsigned int)size/32,sizeof(unsigned int));

    memcpy(Total,GPURes,sizeof(GPURes));
    memcpy(Total + sizeof(GPURes),CPURes,sizeof(CPURes));

    cout<<"Single"<<endl;
    //Testing single device
    add_data("gpu_data",input, device[0][0],size);
    Gtransfer +=copyTime;

    add_data("GPU_output",
             (unsigned int*)calloc((unsigned int)size/32,sizeof(unsigned int)),
             device[0][0],size/32);
    Gtransfer +=copyTime;

    vector<string> GPUargs;
    vector<int> paramCPU;

    GPUargs.push_back("gpu_data");
    GPUargs.push_back("GPU_output");
    paramCPU.push_back(40);


    execute(device[0][0],"selection_interleaved",GPUargs,paramCPU,size,(size>1024)?1024:size);
    cout<<"GPU : "<<nanoSeconds  <<"\t"<<  Gtransfer<<endl;
    unsigned int *GPUr = get_data("GPU_output",device[0][0],size/32);

    add_data("cpu_data",input, device[1][0],size);

    Ctransfer +=copyTime;

    add_data("CPU_output",
             (unsigned int*)calloc((unsigned int)size/32,sizeof(unsigned int)),
             device[1][0],size/32);
    Ctransfer +=copyTime;

    vector<string> CPUargs;




    CPUargs.push_back("cpu_data");
    CPUargs.push_back("CPU_output");


    execute(device[1][0],"selection_block",CPUargs,paramCPU,size/32,(size/32>8192)?8192:size/32);
    cout<<"CPU : "<<nanoSeconds  <<"\t"<<  Ctransfer<<endl;
    unsigned int *CPUr = get_data("CPU_output",device[1][0],size/32);

    //Testing equality of results

    int res1 = memcmp(Total,GPUr,size/32);
    int res2 = memcmp(Total,CPUr,size/32);
    int res3 = memcmp(CPUr,GPUr,size/32);

    cout<<res1<<"\t"<<res2<<"\t"<<res3<<endl;
}