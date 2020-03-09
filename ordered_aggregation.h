//
// Created by gurumurt on 7/2/18.
//

#ifndef OPENCLDISPATCHER_ORDERED_AGGREGATION_H
#define OPENCLDISPATCHER_ORDERED_AGGREGATION_H

#endif //OPENCLDISPATCHER_ORDERED_AGGREGATION_H

#include "include/headers.h"
#include "include/Environment.h"
#include "include/data_api.h"
#include "include/kernel_api.h"
#include "include/runtime_api.h"
#include "include/Importkernel.h"
#include <math.h>
#include <chrono>

enum device_num {
    cpu = 1,
    gpu = 0
};

enum agg_variant{
    atomic = 0,
    branched_inc = 1,
    
    branched_dec = 2
};

double cpu_nanosecs, gpu_nanosecs;

struct {
    string name;
    string src;
    int local_size;
}av[3];

int ps_size;

kernel_info BitonicSortKernel, PartialPrefixSum,FinalPS;


void initialize_aggregates(){
    av[0].name = "atomic_aggregate";
    av[0].src = readKernelFile("kernels/ordered_aggregate_kernels/atomic_aggregate.cl");
    av[0].local_size = 1;

    av[1].name = "atomic_sequential_aggregate";
    av[1].src = readKernelFile("kernels/ordered_aggregate_kernels/atomic_sequential_aggregate.cl");
    av[1].local_size = 1;

    av[2].name = "branched_aggregate";
    av[2].src = readKernelFile("kernels/ordered_aggregate_kernels/branched_aggregate.cl");
    av[2].local_size = 8192;

//    av[3].name = "partial_sequential_aggregate";
//    av[3].src = readKernelFile("kernels/ordered_aggregate_kernels/branched_array_aggregate.cl");
//    av[3].local_size = 8192;
//
//    av[4].name = "partial_atomic_aggregate";
//    av[4].src = readKernelFile("kernels/ordered_aggregate_kernels/partial_atomic_aggregate.cl");
//    av[4].local_size = 1;

}

void initialize_kernels(){

    BitonicSortKernel.name = "_kernel_bitonic_sort";
    BitonicSortKernel.src = readKernelFile("kernels/sort_kernels/BitonicSort.cl");
    BitonicSortKernel.local_size = 2;

    PartialPrefixSum.name = "_kernel_index_value";
    PartialPrefixSum.src = readKernelFile("kernels/prefix_sum/IndexValue.cl");
    PartialPrefixSum.local_size = 2;

    FinalPS.name = "FinalPS";
    FinalPS.src = readKernelFile("kernels/prefix_sum/FinalPS.cl");
    FinalPS.local_size = 2;

    initialize_aggregates();
}

using namespace std::chrono;


unsigned int *bm, *ps;
unsigned int  *cpu_res, *gpu_res;

void unique_percent_generate(int size = 1024, int unique=2){

    //Creating percentage unique values
    for(int i=1;i<size-unique;i++){
        bm[i]=0;
    }
    for(int i=size-unique;i<size;i++){
        bm[i]=1;
    }

}

void unique_percent_generate_val(int size = 1024, int unique=2){

    //Creating percentage unique values

    for(int i=0;i<size-unique;i++){
        bm[i]=1;
    }
    int add_val = 2;
    for(int i=size-unique;i<size;i++){
        bm[i]=++add_val;
    }

}

void percent_generate_val(int size = 1024, int repeat= 2){

    int add_num = 1;

    for(int i = 0; i < size; i++){
        for(int j = 0; (j<repeat-1)&&(i<size-1);j++){
            bm[i] = add_num;
            i++;

        }

        bm[i] = add_num;
        add_num++;


    }

}

void percent_generate(int size = 1024, int repeat= 2){

    for(int i = 1; i < size; i++){
        for(int j = 0; (j<repeat-1)&&(i<size-1);j++){
            bm[i] = 0;
            i++;
        }
        bm[i] = 1;
    }

}

//generates dataset with repeat number of groups of random sizes
void random_generate(int size = 1024, int repeat= 2){

}

void prepare_data_val(int size = 1024, int repeat=2,short type=0){

    bm = (unsigned int*)calloc((unsigned int)size,sizeof(unsigned int));
    ps = (unsigned int*)calloc((unsigned int)size,sizeof(unsigned int));

    if(type)
        unique_percent_generate_val(size,repeat);
    else
        percent_generate_val(size,repeat);

//    cout<<"input values"<<endl;
//    for(int i = 0;i<size;i++)
//            cout<<bm[i]<<endl;
}



void prepare_data(int size = 1024, int repeat=2,short type=0){

    bm = (unsigned int*)calloc((unsigned int)size,sizeof(unsigned int));
    ps = (unsigned int*)calloc((unsigned int)size,sizeof(unsigned int));

    bm[0] = 0;

    if(type)
        unique_percent_generate(size,repeat);
    else
        percent_generate(size,repeat);
    //Derive prefix-sum from the results
    ps[0] = 0;
    for (int i = 1; i < size; i++) {
        ps[i] = ps[i - 1] + bm[i];
    }

//    cout<<"Prefix sum"<<endl;
//    for(int i = 0;i<size;i++)
//            cout<<ps[i]<<endl;

    ps_size=ps[size-1];
    cpu_res = (unsigned int*)calloc((unsigned int)ps[size- 1]+1,sizeof(unsigned int));
    gpu_res = (unsigned int*)calloc((unsigned int)ps[size - 1]+1,sizeof(unsigned int));

}

void add_arguments(int variant, int dev, int size, unsigned int* res){

    cl_device_id d = device[dev][0];
    add_data("PS", ps, d, size);
    if (variant != 1)
        add_data("BM", bm, d, size);
    add_data("res", res, d, ps[size - 1]+1);
}

string kernel_name;

void add_variant_kernel(int variant, int dev){

    cl_device_id d = device[dev][0];

    kernel_name = av[variant].name;
    string kernel_source = av[variant].src;

    add_kernel(kernel_name, d, kernel_source);
}

bool flags_variant[2][2] = {{true,true},{true,true}};

double testTime;

unsigned int* execute_variant(int variant=1, int dev=0, int size = 1024, int ls = 1,unsigned int* res = gpu_res){

    vector<string> arguments;
    vector<int> param;

    testTime = 0;
    cl_device_id d = device[dev][0];

    add_data("PS", ps, d, size);
//    if (variant == 2)
//        add_data("BM", bm, d, size);
    add_data("res", res, d, ps[size - 1] + 1);

    size_t ITERATOR = 8192;
    stringstream _sStream;
    _sStream << " -DITERATOR=" << ITERATOR;

    string kernel_name = av[variant].name;
    string kernel_source = av[variant].src;
    add_kernel(kernel_name, d, kernel_source,_sStream.str());

//    cout<<"kernel source \n"<<kernel_source<<endl;

    arguments.push_back("PS");
//    if (variant == 2)
//        arguments.push_back("BM");
    arguments.push_back("res");

    if(variant!=4)
        execute(d, kernel_name, arguments, param, (size_t) size/av[variant].local_size,(size_t) ls);
    else
        execute(d, kernel_name, arguments, param, (size_t) size/av[variant].local_size,(size_t) 32);

    testTime = nanoSeconds;

    unsigned int* r = get_data("res",d,ps[size - 1]+1);

//    cout<<r[0]<<"\t";
//
//    for(int i = 0;i<ps[size - 1]+1;i++){
//        cout<<r[i]<<"\n";
//    }

    clear_data(arguments,d);

    return r;


}

double cpu_time, gpu_time, total_time;
double PPS_time, PS_time, AGG_time;
int ITERATOR = 32;

unsigned int* execute_variant_with_PS(int variant=1, int dev=0, int size = 1024, unsigned int* res = gpu_res){

    vector<string> arguments;
    vector<int> param;

    cl_device_id d = device[dev][0];

    //Compute PS first
    uint _m_offset_size = size/ITERATOR;
    uint *_m_offset_arr = (uint *) calloc((uint)_m_offset_size, sizeof(uint));
    uint *_m_index_arr = (uint *) calloc((uint)size, sizeof(uint));

    stringstream _sStream;
    _sStream << " -DOFFSET_SIZE=" << _m_offset_size << " -DITERATOR=" << ITERATOR;

    //add data to the device
    add_data("input_data", bm, d, size);
    add_data("index_array", _m_index_arr, d, size);
    add_data("offset_array", _m_offset_arr, d, _m_offset_size);

    arguments.push_back("input_data");
    arguments.push_back("index_array");
    arguments.push_back("offset_array");

    //Add pps kernel
    add_kernel(PartialPrefixSum.name, d, PartialPrefixSum.src, _sStream.str());

    //Execute partial prefix-sum
    execute(d, PartialPrefixSum.name, arguments, param, (size_t) _m_offset_size,(size_t) 1);
    total_time += nanoSeconds;
    PPS_time = nanoSeconds;
    arguments.clear();
    arguments.push_back("index_array");
    arguments.push_back("offset_array");

    //Add final kernel
    add_kernel(FinalPS.name, d, FinalPS.src, _sStream.str());

    //Execute partial prefix-sum
    execute(d, FinalPS.name, arguments, param, (size_t) _m_offset_size,(size_t) 1);
    total_time += nanoSeconds;
    PS_time = nanoSeconds;
    //Add code for executing variants
    unsigned int *ps = get_data("index_array",d,size);

//    cout<<"PS"<<endl;
//    for(int i=0;i<size;i++){
//        cout<<ps[i]<<endl;
//    }

    //Create result buffer
    unsigned int result_size = ps[size - 1] + 1;

    ps_size= result_size;

    res = (unsigned int *) calloc((unsigned int)size, sizeof(unsigned int ));

    add_data("res", res, d, result_size);
    add_data("Prefix_sum", ps, d, size);

    arguments.clear();

    arguments.push_back("Prefix_sum");
    arguments.push_back("res");

    //Change kernel
    add_kernel(av[variant].name,d,av[variant].src);
    if(variant!=4)
        execute(d, av[variant].name, arguments, param, (size_t) size/av[variant].local_size,(size_t) 32);
    else
        execute(d, av[variant].name, arguments, param, (size_t) size/av[variant].local_size,(size_t) 32);

    total_time += nanoSeconds;
//    print_data("res",d,ps[size - 1]+1);

    unsigned int* r = get_data("res",d,ps[size - 1]+1);

//    for(int i = 0;i<result_size; i++){
//        cout<<r[i]<<endl;
//    }
//    cout<<"result"<<endl;

//    exit(0);
    clear_data(arguments,d);
    return r;
}

int compare;

int aggregate_value;

double gpu_PPS, gpu_PS;
double cpu_PPS, cpu_PS;

void compute_aggregate(int variant = 1, int data_size = 1024){

    unsigned int* res1 = execute_variant_with_PS(variant,1,data_size,gpu_res);

    gpu_time = total_time;
    gpu_PPS = PPS_time;
    gpu_PS = PS_time;

    total_time = 0;
    PS_time = 0;
    PPS_time = 0;

    unsigned int* res2 = execute_variant_with_PS(variant,0,data_size,cpu_res);

    cpu_time = total_time;
    cpu_PPS = PPS_time;
    cpu_PS = PS_time;

    total_time = 0;
    PS_time = 0;
    PPS_time = 0;

    compare=memcmp(&res1[2],&res2[2],sizeof(unsigned int)*(ps[data_size - 1]-2));
    compare = 0;
}


int* base_res;

double baseline;
void baseline_compute(int size){

    baseline=0;
    high_resolution_clock::time_point t1,t2;
    t1 = high_resolution_clock::now();

//    cout<<"Test"<<endl;
    base_res = new int[size];
//    cout<<"Test"<<endl;

    int j=0;
    int local_val = 1;

    for(int i = 1; i<size;i++){
        if(bm[i-1] != bm[i]){
            base_res[j]=local_val;
            local_val = 0;
            j++;
        }
        local_val++;
    }
//    cout<<"Test"<<endl;

    t2 = high_resolution_clock::now();
    baseline = duration_cast<nanoseconds>(t2 - t1).count();

//    cout<<baseline<<endl;
}

void test_single_variant(){

    cout<<"Baseline Test\n";
    prepare_data_val((int)pow(2,25), 32, 1);
    baseline_compute((int)pow(2,25));

    initialize_kernels();
    initialize_aggregates();
    setup_environment();
    int data_size = 1024;
    int iterate = 1;
//    cout<<"Testing local aggregate\n";

    prepare_data(data_size,32,1);
//    compute_aggregate(0,data_size);
//    compute_aggregate(1,data_size);
//    compute_aggregate(2,data_size);
//    compute_aggregate(3,data_size);
    compute_aggregate(0,data_size);
//    compute_aggregate(1,data_size);
    cout<<baseline<<"\t"<<cpu_time<<"\t"<<gpu_time<<endl;
}

void execute_repeats(){

    int iterate = 1;
    int steps = 2;
    cout<<"PS-size\tDataSize\ti\tj\tCPU_PPS_AVG\tCPU_PS_AVG\tCPU_AVG\tGPU_PPS_AVG\tGPU_AVG\tBaseline\tCompare\tQ\n";
    for(int data_size = (int) pow(2,6);data_size<=(int)pow(2,13);data_size*=steps) {
        for (int i = steps; i <= data_size; i *= steps) {
            for (int q = 0; q < 2; q++) {
                prepare_data_val(data_size, i, q);
                baseline_compute(data_size);
                for (int j = 0; j < 2; j++) {
                    double cpu_average = 0;
                    double cpu_pps_average = 0;
                    double cpu_ps_average = 0;
                    double gpu_average = 0;
                    double gpu_pps_average = 0;
                    double gpu_ps_average = 0;
                    for (int z = 0; z < iterate; z++) {
                        initialize_kernels();
                        setup_environment();
                        compute_aggregate(j, data_size);
                        cpu_average += cpu_time;
                        cpu_pps_average += cpu_PPS;
                        cpu_ps_average += cpu_PS;
                        gpu_average += gpu_time;
                        gpu_pps_average += gpu_PPS;
                        gpu_ps_average += gpu_PS;
                    }

                    cout    << ps_size << "\t" << data_size << "\t" << i << "\t" << j << "\t" << cpu_pps_average / iterate << "\t" << cpu_ps_average / iterate
                            << "\t" << cpu_average / iterate
                            << "\t" << gpu_ps_average / iterate
                            << "\t" << gpu_pps_average / iterate
                            << "\t" << gpu_average / iterate
                            << "\t" << baseline
                            << "\t" << compare << "\t" << q << "\n";
                }
            }
        }
    }

}

void DimensionsOrderedAggregation(){

    initialize_aggregates();
    size_t powerSize = 20;
    size_t inputDataSize = pow(2,powerSize);
    size_t avgSize = 10; //average iteration size
    short compareResult=0;

    cout<<"Input\tValuesPerGroup\tVariant\tCompareResult\tWorkGroupSize\tExecutionTime\tTestResult"<<endl;
//    for(inputDataSize=pow(2,18);inputDataSize<=pow(2,powerSize);inputDataSize*=2){
//        for(size_t groupSize = 32; groupSize <=inputDataSize;groupSize*=2){
            prepare_data(inputDataSize, 1, 0);
            for(int i = 0;pow(2,i)<=8192;i++){
                for(int variant =0;variant<=4;variant++) {

                    unsigned int *re = execute_variant(variant, 1, inputDataSize, pow(2,i));

                    double timer = 0;
                    for (int j = 0; j < avgSize; j++) {
                        unsigned int *res2 = execute_variant(variant, 1, inputDataSize, pow(2,i));
                        timer += nanoSeconds;
                        compareResult = memcmp(re, res2, sizeof(unsigned int) * (ps[inputDataSize - 1]));
                    }
                    cout << inputDataSize << "\t" << "32" << "\t"<< variant << "\t" << compareResult << "\t" << pow(2,i) << "\t"
                         << timer / avgSize << "\t" << re[0] << endl;
                }
            }
//    }
}

//Choose API
double choose(int variant=1, int dev=0, int size = 1024){

    string kernelFile = "kernels/ordered_aggregate_kernels/";
    string name,src;

    int local_size;

    switch(variant){

        case 0: name = "atomic_aggregate";
            src = readKernelFile(kernelFile + name + ".cl");
            local_size = 1;
            break;

        case 1: name = "atomic_sequential_aggregate";
            src = readKernelFile(kernelFile + name + ".cl");
            local_size = 32;
            break;

        case 2: name = "branched_aggregate";
            src = readKernelFile(kernelFile + name + ".cl");
            local_size = 32;
            break;

        case 3: name = "partial_sequential_aggregate";
            src = readKernelFile(kernelFile + name + ".cl");
            local_size = 32;
            break;

        case 4: name = "partial_atomic_aggregate";
            src = readKernelFile(kernelFile + name + ".cl");
            local_size = 1;
            break;

        default:    cout<<"Variant not found"<<endl;
            return -1;

    }

    vector<string> arguments;
    vector<int> param;

    cl_device_id d = device[dev][0];

    uint _m_offset_size = (uint) size/ITERATOR;
    uint *_m_offset_arr = (uint *) calloc((uint)_m_offset_size, sizeof(uint));
    uint *_m_index_arr = (uint *) calloc((uint)size, sizeof(uint));

    stringstream _sStream;
    _sStream << " -DOFFSET_SIZE=" << _m_offset_size << " -DITERATOR=" << ITERATOR;

    //add data to the device
    add_data("input_data", bm, d, size);
    add_data("index_array", _m_index_arr, d, size);
    add_data("offset_array", _m_offset_arr, d, _m_offset_size);

    arguments.push_back("input_data");
    arguments.push_back("index_array");
    arguments.push_back("offset_array");

    //Add pps kernel
    add_kernel(PartialPrefixSum.name, d, PartialPrefixSum.src, _sStream.str());

    //Execute partial prefix-sum
    execute(d, PartialPrefixSum.name, arguments, param, (size_t) _m_offset_size,(size_t) 1);
    total_time += nanoSeconds;

    arguments.clear();
    arguments.push_back("index_array");
    arguments.push_back("offset_array");

    //Add final kernel
    add_kernel(FinalPS.name, d, FinalPS.src, _sStream.str());


    //Execute partial prefix-sum
    execute(d, FinalPS.name, arguments, param, (size_t) _m_offset_size,(size_t) 1);
    total_time += nanoSeconds;

    //Add code for executing variants
    unsigned int *ps = get_data("index_array",d,size);

    //Create result buffer
    unsigned int result_size = ps[size - 1] + 1;

    ps_size= result_size;

    unsigned int *res = (unsigned int *) calloc((unsigned int)size, sizeof(unsigned int ));

    add_data("res", res, d, result_size);
    add_data("Prefix_sum", ps, d, size);

    arguments.clear();

    arguments.push_back("Prefix_sum");
    arguments.push_back("res");

    //Change kernel
    add_kernel(av[variant].name,d,av[variant].src);
    if(variant!=4)
        execute(d, av[variant].name, arguments, param, (size_t) size/local_size,(size_t) 1);
    else
        execute(d, av[variant].name, arguments, param, (size_t) size/local_size,(size_t) 32);

    total_time += nanoSeconds;

    clear_data(arguments,d);
    return total_time;
}