//
// Created by gurumurt on 7/20/18.
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

#ifndef OPENCLDISPATCHER_ORDERED_AGGREGATION_TEST_H
#define OPENCLDISPATCHER_ORDERED_AGGREGATION_TEST_H
//Default values
unsigned int *input, *ps, *aggregates;
unsigned int Dsize, Rsize;


//Kernel Info structure
struct kernel_info{
    string name;
    string src;
    int local_size;
} PartialPrefixSum, FinalPS;

//Prepare data
void heavy_hitter(int size = 1024, int HeavyHitterSize=2){

    for(int i=0; i<size-HeavyHitterSize;i++){

        input[i] = 1;
    }

    for(int i = size-HeavyHitterSize;i<size;i++){

        input[i] = (unsigned int) i;
    }

}


void interleaved_values(int size= 1024, int interval = 32){

    unsigned int add_num = 1;

    for(int i = 0; i < size; i++){
        for(int j = 0; (j<interval-1)&&(i<size-1);j++){
            input[i] = add_num;
            i++;
        }
        input[i] = add_num;
        add_num++;
    }

}

//Type  { 0 -> Interleaved; 1 -> Heavy Hitter}
void prepare_data_val(int size = 1024, int repeat=2,short type=0){

    input = new unsigned int[size];
    Dsize = size;
//    ps = new unsigned int[size];

    switch(type){

        case 0: interleaved_values(size,repeat);
                break;

        case 1: heavy_hitter(size,repeat);
                break;

        default:    cout<< "Not correct distribution"<<endl;
                    break;
    }

}

void clear_local_data(){
    delete input;
    delete ps;
}



double baseline;


void baseline_compute(int size){

    baseline=0;
    high_resolution_clock::time_point t1,t2;
    t1 = high_resolution_clock::now();

    auto *base_res = new int[size];

    int j=0;
    int local_val = 1;

    for(int i = 1; i<size;i++){
        if(input[i-1] != input[i]){
            base_res[j]=local_val;
            local_val = 0;
            j++;
        }
        local_val++;
    }

    t2 = high_resolution_clock::now();
    baseline = duration_cast<nanoseconds>(t2 - t1).count();
//    cout<<"Total aggregates: "<<j+1<<endl;
//    cout<<"value of aggregates: "<<base_res[j-1]<<endl;
//    clear_local_data();
//    delete base_res;

}





#endif //OPENCLDISPATCHER_ORDERED_AGGREGATION_TEST_H


//Prefix Sum computation kernels

//Prefix sum parameter
int ITERATOR = 32;

struct aggregate_kernel{
    string name;
    string src;
    int local_size;
}av[5];

void initialize_aggregate_variants(){
    av[0].name = "atomic_aggregate";
    av[0].src = readKernelFile("kernels/ordered_aggregate_kernels/atomic_aggregate.cl");
    av[0].local_size = 1;

//    av[1].name = "local_copy_atomic_aggregate";
//    av[1].src = readKernelFile("kernels/ordered_aggregate_kernels/local_copy_atomic_aggregate.cl");
//    av[1].local_size = 1;

    av[1].name = "atomic_sequential_aggregate";
    av[1].src = readKernelFile("kernels/ordered_aggregate_kernels/atomic_sequential_aggregate.cl");
    av[1].local_size = 32;

    av[2].name = "branched_aggregate";
    av[2].src = readKernelFile("kernels/ordered_aggregate_kernels/branched_aggregate.cl");
    av[2].local_size = 32;

    av[3].name = "partial_sequential_aggregate";
    av[3].src = readKernelFile("kernels/ordered_aggregate_kernels/partial_sequential_aggregate.cl");
    av[3].local_size = 32;

    av[4].name = "partial_atomic_aggregate";
    av[4].src = readKernelFile("kernels/ordered_aggregate_kernels/partial_atomic_aggregate.cl");
    av[4].local_size = 1;
}

kernel_info partial_agg, complete_agg;

void intialize_partial_agg(){

    partial_agg.name = "partial_agg_compute";
    partial_agg.src = readKernelFile("kernels/ordered_aggregate_kernels/two_step/PartialAggCompute.cl");
    partial_agg.local_size = 2;

    complete_agg.name = "final_agg_compute";
    complete_agg.src = readKernelFile("kernels/ordered_aggregate_kernels/two_step/FinalAggCompute.cl");
    complete_agg.local_size = 2;
}

void initialize_prefix_sum(){
    PartialPrefixSum.name = "_kernel_index_value";
    PartialPrefixSum.src = readKernelFile("kernels/prefix_sum/IndexValue.cl");
    PartialPrefixSum.local_size = 2;

    FinalPS.name = "FinalPS";
    FinalPS.src = readKernelFile("kernels/prefix_sum/FinalPS.cl");
    FinalPS.local_size = 2;
}

double ps_partial, ps_complete;

int compute_prefix_sum(short dev){


    cl_device_id d = device[dev][0];
    vector<string> arguments;
    vector<int> parameters;

    int _m_offset_size = Dsize/ITERATOR;

    unsigned int *offset_array = (unsigned int*) calloc((size_t)_m_offset_size, sizeof(unsigned int));
    unsigned int *index_array = (unsigned int*) calloc((size_t)Dsize, sizeof(unsigned int));

    stringstream sStream;

    sStream << " -DOFFSET_SIZE=" << _m_offset_size << " -DITERATOR=" << ITERATOR;

    //add data to the device
    add_data("input_data", input, d, Dsize);
    add_data("index_array", index_array, d, Dsize);
    add_data("offset_array", offset_array, d, _m_offset_size);

    arguments.push_back("input_data");
    arguments.push_back("index_array");
    arguments.push_back("offset_array");


    //Add pps kernel
    add_kernel(PartialPrefixSum.name, d, PartialPrefixSum.src, sStream.str());
    execute(d, PartialPrefixSum.name, arguments, parameters, (size_t) _m_offset_size,(size_t) 1);
    ps_partial = nanoSeconds;
//    print_data("index_array",d,Dsize);
//    print_data("index_array",d,Dsize);
//    print_data("index_array",d,Dsize);

    arguments.clear();
    arguments.push_back("index_array");
    arguments.push_back("offset_array");

    //Execute Final sum
    add_kernel(FinalPS.name, d, FinalPS.src, sStream.str());
    execute(d, FinalPS.name, arguments, parameters, (size_t) _m_offset_size,(size_t) 1);
    ps_complete = nanoSeconds;

    return get_last_data("index_array",d,Dsize);
}

double agg_ps;

unsigned int* computeAggregate(int result_size, int dev, int variant = 0){

    initialize_aggregate_variants();
    //Create result array
    aggregates = (unsigned int *) calloc((unsigned int)result_size, sizeof(unsigned int ));

    cl_device_id d = device[dev][0];

    add_data("res",aggregates,d,(size_t)result_size);

    vector<string> arguments;
    vector<int> parameters;

    arguments.push_back("index_array");
    arguments.push_back("res");


    //Change kernel
    add_kernel(av[variant].name,d,av[variant].src);
    execute(d, av[variant].name, arguments, parameters, (size_t) Dsize/av[variant].local_size,(size_t) 32);

    agg_ps = nanoSeconds;

    aggregates = get_data("res",d,result_size);

    clear_data(arguments,d);
    return aggregates;
}

int partial_aggregate_count;

double agg_par_clock, agg_complete_clock;

unsigned int* compute_aggregate_partial(int dev){


    cl_device_id d = device[dev][0];
    vector<string> arguments;
    vector<int> parameters;

    int _m_offset_size = Dsize/ITERATOR;
    unsigned int *offset_array = (unsigned int*) calloc((size_t)_m_offset_size, sizeof(unsigned int));
    unsigned int *index_array = (unsigned int*) calloc((size_t)Dsize, sizeof(unsigned int));
    unsigned int *aggregate_size = (unsigned int*) calloc((size_t)1, sizeof(unsigned int));

    stringstream sStream;

    cout<<"Offset required: "<<_m_offset_size<<endl;

    sStream << " -DOFFSET_SIZE=" <<
                  _m_offset_size <<
                  " -DITERATOR=" << ITERATOR;

    //add data to the device
    add_data("input_data", input, d, Dsize);
    add_data("index_array", index_array, d, Dsize);
    add_data("offset_array", offset_array, d, _m_offset_size);
    add_data("aggregate_size", aggregate_size, d, 1);

    arguments.push_back("input_data");
    arguments.push_back("index_array");
    arguments.push_back("offset_array");
    arguments.push_back("aggregate_size");

    parameters.push_back(Dsize);

    //Get the total work item size
    size_t work_item_size[3];
    clGetDeviceInfo(	d, CL_DEVICE_MAX_WORK_ITEM_SIZES,3*sizeof(size_t),
                        &work_item_size,NULL);

    cout<<"Total Work Items"<<work_item_size[0]<<endl;

    //Add pps kernel
    add_kernel(partial_agg.name, d, partial_agg.src, sStream.str());
    execute(d, partial_agg.name, arguments, parameters, (size_t) _m_offset_size,(size_t) 1);



//    cout<<"Offset"<<endl;
//    print_data("index_array",d,Dsize);
//    cout<<"Offset"<<endl;
//    print_data("offset_array",d,_m_offset_size);

    agg_par_clock = nanoSeconds;

    unsigned int *res_size = get_data("aggregate_size",d,1);

    //Create result array
    unsigned int *res = (unsigned int*) calloc((size_t)*res_size, sizeof(unsigned int));
    add_data("aggregates", res, d, *res_size);

    arguments.clear();
    arguments.push_back("index_array");
    arguments.push_back("offset_array");
    arguments.push_back("aggregates");

    parameters.clear();

    add_kernel(complete_agg.name, d, complete_agg.src, sStream.str());
    execute(d, complete_agg.name, arguments, parameters, (size_t) _m_offset_size,(size_t) 1);
    agg_complete_clock = nanoSeconds;

    cout<<"results"<<endl;
    print_data("aggregates",d,*res_size);

    aggregates = get_data("aggregates",d,*res_size);

    partial_aggregate_count = *res_size;

    clear_data();

    return aggregates;
}


void test_baseline(unsigned int data_size= (unsigned int)pow(2,25)){

    cout<<data_size<<endl;
    prepare_data_val(data_size,2,0);
    baseline_compute(data_size);
    cout<<"Baseline Time: "<<baseline<<endl;
}

void test_prefix_sum(unsigned int data_size= (unsigned int)pow(2,25)){

    setup_environment();
    prepare_data_val(data_size,8,0);

    cout<<"total result size: "<<compute_prefix_sum(1)+1;

}

void test_partial_aggregation(unsigned int data_size= (unsigned int)pow(2,6), int dev = 0, int variant = 0, int type = 1){

    setup_environment();
    intialize_partial_agg();

    prepare_data_val(data_size,32,type);
    compute_aggregate_partial(dev);
    cout<<"Total aggregates: "<<partial_aggregate_count<<endl;
    cout<<"value of aggregates: "<<aggregates[partial_aggregate_count-1]<<endl;
}

void aggregation(unsigned int data_size= (unsigned int)pow(2,25), int dev = 0, int variant = 0, int type = 1){

    setup_environment();
    prepare_data_val(data_size,1,0);
    int aggregate_count = compute_prefix_sum(dev)+1;
    computeAggregate(aggregate_count, dev, 0);
    cout<<"Total aggregates: "<<aggregate_count<<endl;
    cout<<"value of aggregates: "<<aggregates[aggregate_count-2]<<endl;
}


void test_variants(unsigned int data_size= (unsigned int)pow(2,25), int dev = 0, int type = 1){

    setup_environment();
    prepare_data_val(data_size,32,0);

    //Baseline compute
    baseline_compute(data_size);

    //Prefix-sum variants compute
    int aggSize = compute_prefix_sum(dev)+1;
    computeAggregate(aggSize, dev, 0);
    cout<<"Total aggregates: "<<aggSize<<endl;
    cout<<"value of aggregates: "<<aggregates[aggSize-1]<<endl;

    //Combined prefix-sum variant
    compute_aggregate_partial(dev);
    cout<<"Total aggregates: "<<partial_aggregate_count<<endl;
    cout<<"value of aggregates: "<<aggregates[partial_aggregate_count-1]<<endl;
}


void test_case(){

    int iterate = 1;
    int steps = 2;

    intialize_partial_agg();
    initialize_prefix_sum();

    cout<<"DataSize"<<"\t"<<"Repeats"<<"\t"<< "Device" << "\t" <<"Distribution"
        <<"\t"<<"Baseline"<<"\t"<<"PrefixSumPartial"<<"\t"<<"PrefixSumComplete"
        <<"\t"<<"Variant"<<"\t"<<"PrefixSumAggregation"<<"\t"<<"AggregateResult"<<endl;

    for(int data_size = (int) pow(2,20);data_size <= (int)pow(2,25);data_size*=steps) {
        for (int repeat = 32; repeat <= data_size; repeat *= steps) {
            for(short dev = 1;dev<2;dev++){
                for(int distribution = 0;distribution<2;distribution++) {

                    prepare_data_val(data_size, repeat, distribution);
                    //Baseline
                    double baseline_clock = 0L;
                    for (int i = 0; i < iterate; i++) {
                        baseline_compute(data_size);
                        baseline_clock += baseline;
                    }
                    for (int variant = 0; variant < 5; variant++) {

                        double prefix_sum_partial_clock = 0L, prefix_sum_complete_clock = 0L, ps_agg_clock = 0L;
                        for (int i = 0; i < iterate; i++) {
                            //Compute Prefix-Sum
                            setup_environment();
                            int aggregate_count = compute_prefix_sum(dev) + 1;
                            prefix_sum_partial_clock += ps_partial;
                            prefix_sum_complete_clock += ps_complete;
                            computeAggregate(aggregate_count, dev, variant);
                            ps_agg_clock += agg_ps;
                        }
                        cout << data_size << "\t" << repeat << "\t" << dev << "\t" << distribution << "\t" << baseline_clock / iterate
                             << "\t"
                             << prefix_sum_partial_clock / iterate << "\t" << prefix_sum_complete_clock / iterate << "\t"
                             << variant << "\t"<< ps_agg_clock / iterate
                             << "\t" << aggregates[0] << "\n";
                    }

                }
            }
        }
    }

//    //Running dual aggregate variant
//    setup_environment();
//    for(int data_size = (int) pow(2,6);data_size <= (int)pow(2,15);data_size*=steps) {
//        for (int repeat = 32; repeat <= data_size; repeat *= steps) {
//            for(short dev = 0;dev<2;dev++){
//                for(int distribution = 0;distribution<2;distribution++) {
//
//                    prepare_data_val(data_size, repeat, distribution);
//                    //Baseline
//                    double baseline_clock = 0L;
//                    for (int i = 0; i < iterate; i++) {
//                        baseline_compute(data_size);
//                        baseline_clock += baseline;
//                    }
//
//                        double aggregate_partial_clock = 0L, aggregate_complete_clock = 0L;
//                        for (int i = 0; i < iterate; i++) {
//                            //Compute Prefix-Sum
//
//                            compute_aggregate_partial(dev);
//                            aggregate_partial_clock += agg_par_clock;
//                            aggregate_complete_clock += agg_complete_clock;
//                        }
//
//                        cout << data_size << "\t" << repeat << "\t" << dev << "\t" << distribution << "\t" << baseline_clock / iterate<<"\t"
//                             << aggregate_partial_clock/ iterate << "\t" << aggregate_complete_clock/ iterate << "\t"<<"6"<<"\t-\t"<<aggregates[0]
//                             << endl;
//                }
//            }
//        }
//    }

}