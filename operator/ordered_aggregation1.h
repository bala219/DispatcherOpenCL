//
// Created by gurumurt on 7/27/18.
//

//Make sure atleast 50% of the data is repeated so that it wont reach 2³⁰ values

#ifndef OPENCLDISPATCHER_ORDERED_AGGREGATION_H
#define OPENCLDISPATCHER_ORDERED_AGGREGATION_H

#include "../include/headers.h"
#include "../include/Environment.h"
#include "../include/data_api.h"
#include "../include/kernel_api.h"
#include "../include/runtime_api.h"
#include "../include/Importkernel.h"
#include <math.h>
#include <chrono>

using namespace std::chrono;

unsigned int *input, *ps, *aggregates;
size_t Data_size, Result_size;

//Kernel Info structure
kernel_info PartialPrefixSum,InterOff, FinalPS;

unsigned int final_result_aggregate;
unsigned int itera = 1;

/*
 *
 * Distribution generations ; heavy hitter & interleaved
 *
 */

void heavy_hitter(size_t size = 1024, size_t HeavyHitterSize=2){

    for(int i=0; i<size-HeavyHitterSize;i++){
        input[i] = 1;
    }

    for(int i = size-HeavyHitterSize;i<size;i++){
        input[i] = (unsigned int) i;
    }
}

void interleaved(size_t size= 1024, size_t interval = 32){

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

void prepare_data_val(size_t size = 1024, size_t repeat=32,short type=0){

    if(!input)
        delete input;
    input = (unsigned int*)calloc(size,sizeof(unsigned int));
    Data_size = size;

    switch(type){

        case 0: interleaved(size,repeat);
            break;

        case 1: heavy_hitter(size,repeat);
            break;

        default:    cout<< "Not correct distribution"<<endl;
            break;
    }

}

double ps_partial, ps_complete,ps_total,ps_inter;
size_t ITERATOR = 4096;

/*
 * Prefix computation for grouped aggregation - three phases ; partial compute, interval computation and final compute of total prefix-sum
 */
void initialize_prefix_sum(short dev = 0){
    PartialPrefixSum.name = "partialPS";
    PartialPrefixSum.src = readKernelFile("kernels/prefix_sum/partialPS.cl");
    PartialPrefixSum.local_size = 2;

    InterOff.name = "interOff";
    InterOff.src = readKernelFile("kernels/prefix_sum/interOff.cl");
    InterOff.local_size = 2;


    FinalPS.name = "FinalPS";
    FinalPS.src = readKernelFile("kernels/prefix_sum/FinalPS.cl");
    FinalPS.local_size = 2;

    cl_device_id d = device[dev][0];
    size_t _m_offset_size = Data_size/ITERATOR;

    stringstream sStream;
    sStream << " -DDATA_SIZE=" << Data_size << " -DOFFSET_SIZE=" << _m_offset_size << " -DITERATOR=" << ITERATOR;
    add_kernel(PartialPrefixSum.name, d, PartialPrefixSum.src, sStream.str());
    add_kernel(InterOff.name, d, InterOff.src, sStream.str());
    add_kernel(FinalPS.name, d, FinalPS.src, sStream.str());
}

unsigned int * index_array;

//Serial implementation
size_t compute_serial_prefix_sum(short dev){

    index_array = (unsigned int*) calloc(Data_size, sizeof(unsigned int));

    high_resolution_clock::time_point t1,t2;
    t1 = high_resolution_clock::now();

    index_array[0] = 0;

    for(size_t i =1; i<Data_size;i++){

        index_array[i] = index_array[i - 1] + (input[i - 1] != input[i]);
    }

    t2 = high_resolution_clock::now();

    ps_total = duration_cast<nanoseconds>(t2 - t1).count();
    cl_device_id d = device[dev][0];
    add_data("index_array", index_array, d, Data_size);

    return index_array[Data_size - 1] + 1;
}


//OpenCl variant
size_t compute_prefix_sum(short dev){

    cl_device_id d = device[dev][0];
    vector<string> arguments;
    vector<int> parameters;

    size_t _m_offset_size = Data_size/ITERATOR;

    unsigned int *offset_array = (unsigned int*)
            calloc(_m_offset_size, sizeof(unsigned int));

    if(!index_array )
        delete index_array;

    index_array = (unsigned int*)
            calloc(Data_size, sizeof(unsigned int));

    if(!index_array)
        cout<<"error in calloc"<<endl;

    stringstream sStream;
    add_data("index_array", index_array, d, Data_size);
    add_data("offset_array", offset_array, d, _m_offset_size);

    arguments.push_back("input_data");
    arguments.push_back("index_array");
    arguments.push_back("offset_array");

    execute(d, PartialPrefixSum.name, arguments, parameters, _m_offset_size,1);
    ps_partial = nanoSeconds;

    arguments.clear();
    arguments.push_back("index_array");
    arguments.push_back("offset_array");

    execute(d, InterOff.name, arguments, parameters, _m_offset_size,_m_offset_size);
    ps_inter = nanoSeconds;

    execute(d, FinalPS.name, arguments, parameters, _m_offset_size,1);
    ps_complete = nanoSeconds;

    ps_total = ps_partial + ps_inter + ps_complete;

    size_t result_size = get_last_data("index_array",d,Data_size) + 1;

    delete_data("offset_array",d);
    delete offset_array;

    delete_data("input_data",d);
    if(!input)
        delete input;

    return result_size;
}

/*
 * Grouped aggregation serial baseline execution
 */

double baseline;

//Compiled serial C++ execution
void baseline_compute(size_t size){

    baseline=0;
    size_t result_size = 1;
    high_resolution_clock::time_point t1,t2;
    t1 = high_resolution_clock::now();

    for(size_t i = 1; i<size;i++){
        result_size +=(input[i-1] != input[i]);
    }

    auto *base_res = (unsigned int*)calloc(result_size,sizeof(unsigned int));

    unsigned int j=0;
    for(size_t i = 1; i<size;i++){
        base_res[j]++;
        j+=(input[i-1]!=input[i]);
    }

    t2 = high_resolution_clock::now();
    baseline = duration_cast<nanoseconds>(t2 - t1).count();
    final_result_aggregate = base_res[0];
}


void test_baseline(){
    cout<<"testing baseline"<<endl;
    size_t dataSize =pow(2,24);
    prepare_data_val(dataSize,dataSize,1);

    double BASE_TIME = 0;
    int BASE_result = 0;

    for(int i=0;i<itera;i++){
        baseline_compute(dataSize);
        BASE_result += final_result_aggregate;
        BASE_TIME += baseline;
    }
    cout<<"Total time : "<<BASE_TIME/itera<<endl;
    cout<<"PS result : "<<BASE_result/itera<<endl;

}


/*
 * Grouped aggregation parallel execution
 */

kernel_info av[5], zero_kernel;

void initialize_aggregate_variants(){

    zero_kernel.name = "zero_kernel";
    zero_kernel.src = readKernelFile("kernels/helper/zero_kernel.cl");
    zero_kernel.local_size = 1;

    av[0].name = "atomic_aggregate";
    av[0].src = readKernelFile("kernels/ordered_aggregate_kernels/atomic_aggregate.cl");
    av[0].local_size = 1;

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

double agg_ps;

void computeAggregate(size_t result_size, int dev, int variant = 0){

    cl_device_id d = device[dev][0];

    stringstream sStream;
    sStream << " -DDATA_SIZE=" << Data_size<< " -DOFFSET_SIZE=" << result_size << " -DITERATOR=" << ITERATOR;

    add_kernel(av[variant].name,d,av[variant].src,sStream.str());

    if(!aggregates)
        delete aggregates;

    aggregates = (unsigned int *) calloc(result_size, sizeof(unsigned int));

    add_data("res",aggregates,d, result_size);
    vector<string> arguments,zero_args;
    vector<int> parameters;

    arguments.push_back("index_array");
    arguments.push_back("res");

    if (variant == 0)
        execute(d, av[variant].name, arguments, parameters, Data_size, 1);
    else if (variant == 4)
        execute(d, av[variant].name, arguments, parameters, Data_size, ITERATOR);
    else
        execute(d, av[variant].name, arguments, parameters, Data_size / ITERATOR, 1);
    agg_ps = nanoSeconds;

    final_result_aggregate = get_first_data("res",d,result_size);

}

void test_atomic_variants(short dev = 0){

    setup_environment();
    initialize_prefix_sum();

    cout<<"Test atomic variant"<<endl;
    size_t dataSize = pow(2,24);
    prepare_data_val(dataSize,32,1);

    double ATOMIC_TIME = 0;
    int ATOMIC_result = 0;

    size_t wi[3];
    clGetDeviceInfo(device[dev][0],CL_DEVICE_MAX_WORK_ITEM_SIZES,
                    sizeof(size_t)*3,wi,NULL);
    ITERATOR = 32;
    while (dataSize / ITERATOR > wi[0]) {
        ITERATOR *= 2;
    }

    size_t result_size = compute_prefix_sum(dev);
    cout<<"Result size : "<< result_size<<endl;

    for(int i=0;i<itera;i++){
        computeAggregate(result_size,dev,3);
        ATOMIC_result += final_result_aggregate;
        ATOMIC_TIME += agg_ps;
    }
    cout<<"Total time : "<<ATOMIC_TIME/itera<<endl;
    cout<<"Atomic result : "<<ATOMIC_result/itera<<endl;

}

kernel_info GeneratePosition, FinalAtomicFreeResult;

void initialize_atomic_free(){

    GeneratePosition.name = "GeneratePosList";
    GeneratePosition.src = readKernelFile("kernels/ordered_aggregate_kernels/No_Atomics/GeneratePosList.cl");

    FinalAtomicFreeResult.name = "FinalAggregate";
    FinalAtomicFreeResult.src = readKernelFile("kernels/ordered_aggregate_kernels/No_Atomics/FinalResult.cl");
}

double ac_time;
void Compute_atomic_free_aggregate(size_t result_size, int dev = 1){

    initialize_atomic_free();

    aggregates = (unsigned int *) calloc(result_size,
                                         sizeof(unsigned int));

    unsigned int *offsets = (unsigned int *) calloc(result_size,
                                                    sizeof(unsigned int));
    cl_device_id d = device[dev][0];

    add_data("offsets",offsets,d, result_size);

    vector<string> arguments;
    vector<int> parameters;

    arguments.push_back("index_array");
    arguments.push_back("offsets");

    stringstream sStream;
    sStream << " -DDATA_SIZE=" << Data_size<< " -DOFFSET_SIZE=" << result_size << " -DITERATOR=" << ITERATOR;

    add_kernel(GeneratePosition.name,d,GeneratePosition.src,sStream.str());
    execute(d, GeneratePosition.name, arguments, parameters, Data_size, 1);
    ac_time = nanoSeconds;

    arguments.clear();
    add_data("res",aggregates,d,result_size);

    arguments.push_back("offsets");
    arguments.push_back("res");

    add_kernel(FinalAtomicFreeResult.name,d,FinalAtomicFreeResult.src,sStream.str());
    execute(d, FinalAtomicFreeResult.name, arguments, parameters, result_size, 1);
    ac_time += nanoSeconds;

    aggregates = get_data("res",d,result_size);
    final_result_aggregate = aggregates[0];

    delete_data("offsets",d);

    delete aggregates;
    delete offsets;
}

void test_non_atomic_variants(short dev = 0){

    setup_environment();
    initialize_prefix_sum();

    cout<<"Test Non-atomic baseline"<<endl;
    size_t dataSize = pow(2,24);
    prepare_data_val(dataSize,32,1);

    double ATOMIC_TIME = 0;
    int ATOMIC_result = 0;

    size_t wi[3];
    clGetDeviceInfo(device[dev][0],CL_DEVICE_MAX_WORK_ITEM_SIZES,
                    sizeof(size_t)*3,wi,NULL);
    ITERATOR = 32;
    while (dataSize / ITERATOR > wi[0]) {
        ITERATOR *= 2;
    }

    size_t result_size = compute_prefix_sum(dev);
    cout<<"Result size : "<< result_size<<endl;

    for(int i=0;i<itera;i++){
        Compute_atomic_free_aggregate(result_size,dev);
        ATOMIC_result = final_result_aggregate;
        ATOMIC_TIME += agg_ps;
    }
    cout<<"Total time : "<<ATOMIC_TIME/itera<<endl;
    cout<<"Atomic result : "<<ATOMIC_result<<endl;

}


void test_combined_atomics(short dev = 0){

    setup_environment();
    initialize_prefix_sum();
    cout<<"data_size\tGroup\tVariant\tPS_time\ttime\tres"<<endl;
    size_t dataSize =pow(2,22);

    size_t increment_size = dataSize*1/100;

    //Adding loop for different group sizes
    for(int group = increment_size;group<=dataSize;group+=increment_size) {
        prepare_data_val(group, 1, 0);

        //Testing sequential baseline
        double BASE_TIME = 0;
        int BASE_result = 0;

        for (int i = 0; i < itera; i++) {
            baseline_compute(group);
            BASE_result = final_result_aggregate;
            BASE_TIME += baseline;
        }


        double PS_TIME = 0;
        int ps_result = 0;
        for (int i = 0; i < itera; i++) {
            ps_result += compute_prefix_sum(dev);
            PS_TIME += ps_total;
        }

        size_t result_size = ps_result / itera;

        unsigned int ATOMIC_FREE_result = 0;
        unsigned int ATOMIC_FREE_time = 0;

        //Testing parallel baseline
        for(int i=0;i<itera;i++){
            Compute_atomic_free_aggregate(result_size,dev);
            ATOMIC_FREE_result = final_result_aggregate;
            ATOMIC_FREE_time += ac_time;
        }

        double ATOMIC_TIME = 0;
        int ATOMIC_result = 0;


        for (int variant = 0; variant < 4; variant++) {
            for (int i = 0; i < itera; i++) {
                computeAggregate(result_size, dev, variant);
                ATOMIC_result = final_result_aggregate;
                ATOMIC_TIME += agg_ps;
            }

            cout << dataSize << "\t" << group << "\t"
                 << variant << "\t" << PS_TIME / itera << "\t" << ATOMIC_TIME / itera << "\t"
                 << ATOMIC_result << endl;
        }
        cout << dataSize << "\t" << group << "\tbase\t0"
             << "\t" << BASE_TIME / itera
             << "\t" << BASE_result << endl;
        cout << dataSize << "\t" << group << "\tparallel_base\t"<< PS_TIME / itera
             << "\t" << ATOMIC_FREE_time / itera
             << "\t" << ATOMIC_FREE_result << endl;
    }
}


void test_atomic_free_aggregates(){

    cout<<"testing"<<endl;
    setup_environment();
    initialize_prefix_sum();
    size_t dataSize =pow(2,7);

    size_t wi[3];
    clGetDeviceInfo(device[1][0],CL_DEVICE_MAX_WORK_ITEM_SIZES,sizeof(size_t)*3,wi,NULL);
    cout<<"Total work Items available: "<<wi[0]<<endl;

    ITERATOR = 32;
    while(dataSize/ITERATOR >= wi[0]){
        ITERATOR*=2;
    }
    cout<<"iterator: "<<ITERATOR<<endl;

    prepare_data_val(dataSize,dataSize,0);
//    baseline_compute(dataSize);
    size_t res = compute_prefix_sum(1);
    cout<<"total results : "<<res<<endl;
    Compute_atomic_free_aggregate(res,1);
    cout<<"RunTime: "<<ac_time<<endl;
    cout<<"Final result : "<<final_result_aggregate<<endl;
}


void test_aggregates(){

    cout<<"testing aggregates"<<endl;
    setup_environment();
    initialize_prefix_sum();
    size_t dataSize =pow(2,18);

    size_t wi[3];
    clGetDeviceInfo(device[1][0],CL_DEVICE_MAX_WORK_ITEM_SIZES,sizeof(size_t)*3,wi,NULL);
    cout<<"Total work Items available: "<<wi[0]<<endl;

    ITERATOR = 32;
    cout<<"iterator: "<<ITERATOR<<endl;

    prepare_data_val(dataSize,32,1);
    baseline_compute(dataSize);
    size_t res = compute_prefix_sum(1);
    cout<<"total results : "<<res<<endl;
    computeAggregate(res,1,4);
    cout<<"RunTime: "<<agg_ps<<endl;
    cout<<"Final result : "<<final_result_aggregate<<endl;
}

void releaseInputBuffers(){

    for(multimap<pair<string, cl_device_id>, cl_mem>::iterator it=data_dictionary.begin();it!=data_dictionary.end();){

        err = clReleaseMemObject(it->second);
        clFinish(queue[it->first.second].queue);
        clFlush(queue[it->first.second].queue);
        if(err != CL_SUCCESS){
            std::cerr<<":Error on releasing buffer (" <<err
                     <<")"<<endl;
        }
    }
}

void testCPU(short dev=0, short typ=1){

    initialize_prefix_sum();
    initialize_aggregate_variants();
    ITERATOR = 32;
    size_t wi[3];
    setup_environment();
    clGetDeviceInfo(device[dev][0],CL_DEVICE_MAX_WORK_ITEM_SIZES,sizeof(size_t)*3,wi,NULL);
    cout<<"max Workitem Size: "<<wi[0]<<endl;
    size_t wg_size = 0;
    clGetDeviceInfo(device[dev][0],CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(size_t),&wg_size,NULL);
    cout<<"max Workgroup Size: "<<wg_size<<endl;

    clGetDeviceInfo(device[dev][0],CL_DEVICE_LOCAL_MEM_SIZE,sizeof(size_t),&wg_size,NULL);
    cout<<"max LocalMemory Size (4 bytes): "<<wg_size/4<<endl;
    cout<<"Device\tresultAggregate\tDataSize\tgroup\tVariant\tAggregationTime\tIterator\tPS Time"<<endl;

    size_t max_data_size = pow(2,18);


    for (size_t dataSize = (size_t) pow(2,6); dataSize <= (size_t) max_data_size; dataSize += max_data_size/100) {
        for (int i = 6; i >= 0; i--) {
            ITERATOR = 32;
            while (dataSize / ITERATOR > wi[0]) {
                ITERATOR *= 2;
            }
            size_t group =  dataSize;
            double print_time = 0;
            double print_ps_time = 0;
            short max_itr = 1;
            for(int itr = 0; itr<max_itr; itr++){
//                        setup_environment();
                prepare_data_val(dataSize,group, typ);
                if(i!=6){
                    size_t res = compute_prefix_sum(dev);
                    if(i==5){
                        Compute_atomic_free_aggregate(res,dev);
                        agg_ps = ac_time;
                    }
                    else
                        computeAggregate(res, dev, i);
                }
                else{
                    baseline_compute(dataSize);
                    agg_ps = baseline;
                    ps_total = 0;
                }
                print_time +=agg_ps;
                print_ps_time +=ps_total;

            }
            cout <<dev<<"\t"<< final_result_aggregate << "\t" << log(max_data_size)/log(2)  << "\t" << group
                 << "\t" << i << "\t" << (print_time+print_ps_time)/max_itr << "\t" << ITERATOR << "\t" << print_ps_time/max_itr << endl;
        }
    }
}


void testWriteBuffer(int size){

    setup_environment();

    for(int i=0;i<10000;i++){
        int *da = (int*)calloc(sizeof(int),size);
        add_data("da",&da,device[0][0],1);
        delete_data("da",device[0][0]);
        data_dictionary.clear();
        cout<<i<<endl;
        delete da;
    }

}

void test_error() {

    initialize_aggregate_variants();
    short dev = 1;
    size_t wi[3];
    short num_of_iterations = 100;

    //Prepare environment
    setup_environment();
    clGetDeviceInfo(device[dev][0], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * 3, wi, NULL);
    cout << "max Workitem Size: " << wi[0] << endl;
    size_t wg_size = 0;
    clGetDeviceInfo(device[dev][0], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &wg_size, NULL);
    cout << "max Workgroup Size: " << wg_size << endl;
    clGetDeviceInfo(device[dev][0], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(size_t), &wg_size, NULL);
    cout << "max LocalMemory Size (4 bytes): " << wg_size / 4 << endl;
    cout << "Device\tresultAggregate\tDataSize\tgroup\tVariant\tAggregationTime\tIterator\tPS Time" << endl;


//    wi[0] = 8;
    //Prepare test data
    size_t max_data_size = pow(2, 15);
    ITERATOR = 1;
    while (max_data_size / ITERATOR > wi[0]) {
        ITERATOR *= 2;
    }

    unsigned int resultant_aggregate = 0;
    for (int i = 100; i >= 0; i-=1) {

        size_t percent_value;
        if(i ==0)
            percent_value = 1;
        else
            percent_value = max_data_size * i / 100;

        percent_value += percent_value % 32;
        size_t group = percent_value;
        prepare_data_val(max_data_size, percent_value, 0);

        add_data("input_data",input,device[dev][0],max_data_size);

        initialize_prefix_sum(dev);
        double print_ps_time = 0;

        size_t res_size = compute_prefix_sum(dev);

        for (int variant = 6; variant >= 0; variant--) {
            double print_time = 0;

            for(int iteration = 0; iteration < num_of_iterations;iteration++){

                if (variant != 6) {
                    if (variant == 5) {
                        Compute_atomic_free_aggregate(res_size, dev);
                        resultant_aggregate = final_result_aggregate;
                        agg_ps = ac_time;
                    } else {
                        computeAggregate(res_size, dev, variant);
                        resultant_aggregate = final_result_aggregate;
                    }

                } else {
                    baseline_compute(max_data_size);
                    resultant_aggregate = final_result_aggregate;
                    agg_ps = baseline;
                }

                print_time += agg_ps;
                print_ps_time = ps_total;
                delete_data("res",device[dev][0]);

            }

            cout << dev << "\t" << resultant_aggregate << "\t" << i << "\t" << group
                 << "\t" << variant << "\t" << (print_time)/num_of_iterations << "\t" << ITERATOR << "\t" << print_ps_time
                 << endl;
        }

        delete_data("index_array",device[dev][0]);

    }
}


void test_variable_data_size(short disType = 1) {

    initialize_aggregate_variants();
    short dev = 0;
    size_t wi[3];
    short num_of_iterations = 100;

    //Prepare environment
    setup_environment();
    clGetDeviceInfo(device[dev][0], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * 3, wi, NULL);
    cout << "max Workitem Size: " << wi[0] << endl;
    size_t wg_size = 0;
    clGetDeviceInfo(device[dev][0], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &wg_size, NULL);
    cout << "max Workgroup Size: " << wg_size << endl;
    clGetDeviceInfo(device[dev][0], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(size_t), &wg_size, NULL);
    //Prepare test data
    size_t min_data_size = wi[0];
    size_t max_data_size = pow(2,20);

    cout<<"Min size : "<<min_data_size<<"\nMax size : "<<max_data_size<<endl;
    cout << "max LocalMemory Size (4 bytes): " << wg_size / 4 << endl;
    cout << "Device\tresultAggregate\tDataSize\tgroup\tVariant\tAggregationTime\tIterator\tPS Time" << endl;


    unsigned int resultant_aggregate = 0;
//    for (int i = min_data_size; i <= max_data_size; i+=32) {
    for (int i = min_data_size; i <= max_data_size; i*=2) {
//    for (int i = 1; i <= 100; i++) {

        size_t group = i;
        if(disType)
            prepare_data_val(i, 1, 1); //All unique distribution
        else
            prepare_data_val(i, i, 1); //All duplicate distribution

        add_data("input_data",input,device[dev][0],i);

        ITERATOR = 1;
        while (i / ITERATOR > wi[0]) {
            ITERATOR *= 2;
        }

        initialize_prefix_sum(dev);

        double print_ps_time = 0;

        size_t res_size = compute_prefix_sum(dev);

        for (int variant = 6; variant >= 0; variant--) {
            double print_time = 0;

            for(int iteration = 0; iteration < num_of_iterations;iteration++){

                if (variant != 6) {
                    if (variant == 5) {
                        Compute_atomic_free_aggregate(res_size, dev);
                        resultant_aggregate = final_result_aggregate;
                        agg_ps = ac_time;
                    } else {
                        computeAggregate(res_size, dev, variant);
                        resultant_aggregate = final_result_aggregate;
                    }
                } else {
                    baseline_compute(i);
                    resultant_aggregate = final_result_aggregate;
                    agg_ps = baseline;
                }

                print_time += agg_ps;
                print_ps_time = ps_total;
                delete_data("res",device[dev][0]);

            }

            cout << dev << "\t" << resultant_aggregate << "\t" << i << "\t" << group
                 << "\t" << variant << "\t" << (print_time)/num_of_iterations << "\t" << ITERATOR << "\t" << print_ps_time
                 << endl;
        }

        delete_data("index_array",device[dev][0]);

    }
}


void test_variable_dataGroup_size() {

    initialize_aggregate_variants();
    short dev = 0;
    size_t wi[3];
    short num_of_iterations = 100;

    //Prepare environment
    setup_environment();
    clGetDeviceInfo(device[dev][0], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * 3, wi, NULL);
    cout << "max Workitem Size: " << wi[0] << endl;
    size_t wg_size = 0;
    clGetDeviceInfo(device[dev][0], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &wg_size, NULL);
    cout << "max Workgroup Size: " << wg_size << endl;
    clGetDeviceInfo(device[dev][0], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(size_t), &wg_size, NULL);
    cout << "max LocalMemory Size (4 bytes): " << wg_size / 4 << endl;
    cout << "Device\tresultAggregate\tDataSize\tgroup\tVariant\tAggregationTime\tIterator\tPS Time" << endl;


    //Prepare test data
    size_t max_data_size = pow(2, 25);

    unsigned int resultant_aggregate = 0;

    for (int i = 32; i <= max_data_size; i*=2) {

        for(int group = 32; group<=i; group*=2) {
            prepare_data_val(i, group, 0);

            add_data("input_data", input, device[dev][0], i);

            ITERATOR = 1;
            while (i / ITERATOR > wi[0]) {
                ITERATOR *= 2;
            }

            initialize_prefix_sum(dev);

            double print_ps_time = 0;

            size_t res_size = compute_prefix_sum(dev);

            for (int variant = 6; variant >= 0; variant--) {
                double print_time = 0;

                for (int iteration = 0; iteration < num_of_iterations; iteration++) {

                    if (variant != 6) {
                        if (variant == 5) {
                            Compute_atomic_free_aggregate(res_size, dev);
                            resultant_aggregate = final_result_aggregate;
                            agg_ps = ac_time;
                        } else {
                            computeAggregate(res_size, dev, variant);
                            resultant_aggregate = final_result_aggregate;
                        }

                    } else {
                        baseline_compute(i);
                        resultant_aggregate = final_result_aggregate;
                        agg_ps = baseline;
                    }

                    print_time += agg_ps;
                    print_ps_time = ps_total;
                    delete_data("res", device[dev][0]);

                }

                cout << dev << "\t" << resultant_aggregate << "\t" << i << "\t" << group
                     << "\t" << variant << "\t" << (print_time) / num_of_iterations << "\t" << ITERATOR << "\t"
                     << print_ps_time
                     << endl;
            }

            delete_data("index_array", device[dev][0]);
        }

    }
}


double BanditFunction(short variant, short distribution, size_t group, size_t data_size, short dev, short num_of_iterations = 100){

    initialize_aggregate_variants();
    size_t wi[3];

    //Prepare environment
    setup_environment();
    clGetDeviceInfo(device[dev][0], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * 3, wi, NULL);
    size_t wg_size = 0;
    clGetDeviceInfo(device[dev][0], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &wg_size, NULL);

    //Prepare data
    prepare_data_val(data_size, group, distribution);

    add_data("input_data", input, device[dev][0], data_size);

    ITERATOR = 1;
    while (data_size / ITERATOR > wi[0]) {
        ITERATOR *= 2;
    }

    initialize_prefix_sum(dev);

    size_t res_size = compute_prefix_sum(dev);

    int resultant_aggregate = 0;

    for (int iteration = 0; iteration < num_of_iterations; iteration++) {

        if (variant != 6) {
            if (variant == 5) {
                Compute_atomic_free_aggregate(res_size, dev);
                resultant_aggregate = final_result_aggregate;
                agg_ps = ac_time;
            } else {
                computeAggregate(res_size, dev, variant);
                resultant_aggregate = final_result_aggregate;
            }

        } else {
            baseline_compute(data_size);
            resultant_aggregate = final_result_aggregate;
            agg_ps = baseline;
        }

        delete_data("res", device[dev][0]);

    }

    return agg_ps;
}


//Testing the atomic variants
void test_atomics(short disType =1){

    initialize_aggregate_variants();
    short dev = 1;
    size_t wi[3];
    short num_of_iterations = 100;

    //Prepare environment
    setup_environment();
    clGetDeviceInfo(device[dev][0], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * 3, wi, NULL);
    cout << "max Workitem Size: " << wi[0] << endl;
    size_t wg_size = 0;
    clGetDeviceInfo(device[dev][0], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &wg_size, NULL);
    cout << "max Workgroup Size: " << wg_size << endl;
    clGetDeviceInfo(device[dev][0], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(size_t), &wg_size, NULL);
    //Prepare test data
    size_t min_data_size = pow(2,20);
    size_t max_data_size = pow(2,24);

    cout<<"Min size : "<<min_data_size<<"\nMax size : "<<max_data_size<<endl;
    cout << "max LocalMemory Size (4 bytes): " << wg_size / 4 << endl;
    cout << "Device\tresultAggregate\tDataSize\tgroup\tVariant\tAggregationTime\tIterator\tPS Time" << endl;


    unsigned int resultant_aggregate = 0;
//    for (int i = min_data_size; i <= max_data_size; i+=32) {
    for (int i = min_data_size; i <= max_data_size; i*=2) {
//    for (int i = 1; i <= 100; i++) {

        size_t group = i;
        if(disType)
            prepare_data_val(i, 1, 1); //All duplicated distribution
        else
            prepare_data_val(i, i, 1); //All unique distribution

        add_data("input_data",input,device[dev][0],i);

        ITERATOR = 32;
        while (i / ITERATOR > wi[0]) {
            ITERATOR *= 2;
        }

        initialize_prefix_sum(dev);

        double print_ps_time = 0;

        size_t res_size = compute_prefix_sum(dev);

        //Test naive atomic variant
        double print_time = 0;

        for(int iteration = 0; iteration < num_of_iterations; iteration++){

            computeAggregate(res_size, dev, 0);
            resultant_aggregate = final_result_aggregate;
            print_time += agg_ps;
            print_ps_time = ps_total;
            delete_data("res", device[dev][0]);

        }

        cout << dev << "\t" << resultant_aggregate << "\t" << i << "\t" << group
             << "\t" << "0" << "\t" << (print_time)/num_of_iterations << "\t" << ITERATOR << "\t" << print_ps_time
             << endl;



        //Incrementing Iterator
        for(ITERATOR = 32; ITERATOR<i;ITERATOR*=2){
            print_time = 0;
            for(int iteration = 0; iteration < num_of_iterations; iteration++){
                computeAggregate(res_size, dev, 0);
                resultant_aggregate = final_result_aggregate;
                print_time += agg_ps;
                print_ps_time = ps_total;
                delete_data("res", device[dev][0]);
            }
            cout << dev << "\t" << resultant_aggregate << "\t" << i << "\t" << group
                 << "\t" << "1" << "\t" << (print_time)/num_of_iterations << "\t" << ITERATOR << "\t" << print_ps_time
                 << endl;
        }


        delete_data("index_array",device[dev][0]);
    }


}

#endif //OPENCLDISPATCHER_ORDERED_AGGREGATION_H