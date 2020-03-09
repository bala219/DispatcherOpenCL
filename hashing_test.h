//
// Created by gurumurt on 8/15/18.
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

#ifndef OPENCLDISPATCHER_HASHING_TEST_H
#define OPENCLDISPATCHER_HASHING_TEST_H

#endif //OPENCLDISPATCHER_HASHING_TEST_H

/*
 * Shared linear probing hashing - 3 steps: hash function - map; hash insert - to implement; hash probe - to implement;
 */

kernel_info hash_function_kernel, hash_insert, hash_probe;

unsigned int* input;
unsigned int* hash_table;
size_t tableSize;

void initialize_hash(){

    hash_function_kernel.name = "knuth_function";
    hash_function_kernel.src = readKernelFile("kernels/hash_kernels/hash_function/knuth_function.cl");
    hash_function_kernel.local_size = 1;

    hash_insert.name = "linear_probing_aggregate";
    hash_insert.src = readKernelFile("kernels/hash_kernels/hash_insert/linear_probing_aggregate.cl");
    hash_insert.local_size = 32;

    hash_probe.name = "linear_probing_probe";
    hash_probe.src = readKernelFile("kernels/hash_kernels/hash_probe/linear_probing_probe.cl");
    hash_probe.local_size = 32;
}

void initialize_hash_table(int size){

    hash_table = (unsigned int *) calloc(size, sizeof(unsigned int));
}

void prepare_data_val(int size = 10, short type=0){

    input = (unsigned int *) calloc(size, sizeof(unsigned int));
    initialize_hash_table(size);

    if(type){
        for(int i =0;i<size;i++){

            input[i] = rand()%(size/8);

        }
    } else{
        unsigned int a=1;
        for(unsigned int i =0;i<size;i++){

//            if(i%513==0){
//                a++;
//            }
                input[i] = a;
        }
    }
}

unsigned long hash_function(unsigned int key){
    return (1300000077*key)* tableSize>>32;
}

double baseline;
void test_baseline(int size=10,short type =0){

    tableSize= (size_t) size;
    prepare_data_val(size);
    high_resolution_clock::time_point t1,t2;
    t1 = high_resolution_clock::now();

    for(int i=0;i<size;i++) {
        int slot = (int)hash_function(input[i]);
        int initial_slot = slot;

        do {
            if (!hash_table[slot]) {
                hash_table[slot] = input[i];
                break;
            }
            slot = (++slot) % size;
        }while (slot != initial_slot);

    }

    t2 = high_resolution_clock::now();
    baseline = duration_cast<nanoseconds>(t2 - t1).count();
    cout<<baseline<<" : duration"<<endl;

//    for (int i = 0; i < size; ++i) {
//        cout<<hash_table[i]<<endl;
//    }

}

void test_hash(short dev=0,size_t size=pow(2,10)){

    test_baseline(size);

    setup_environment();
    initialize_hash();
    prepare_data_val(size);
    //Hash function map
    cl_device_id d = device[dev][0];

    unsigned int *slots = (unsigned int *) calloc(size, sizeof(unsigned int));
    unsigned int *result = (unsigned int *) calloc(size, sizeof(unsigned int));

    add_data("input",input,d,size);
    add_data("slots",slots,d,size);


    vector<string> arguments;
    vector<int> parameters;

    arguments.push_back("input");
    arguments.push_back("slots");

    stringstream sStream;
    sStream << " -DTABLE_SIZE=" <<hash_insert.local_size<<" -DTHREAD_SIZE="<<hash_insert.local_size<<endl;

    //Get hash slots
    add_kernel(hash_function_kernel.name,d,hash_function_kernel.src,sStream.str());
    execute(d, hash_function_kernel.name, arguments, parameters, size, 1);

//    add_data("hash_table",hash_table,d,size);
    add_new_data<unsigned int>("mutex",d,size);
    add_new_data<unsigned int>("hash_table",d,size);
    add_new_data<unsigned int>("aggregate",d,size);

    arguments.push_back("mutex");
    arguments.push_back("hash_table");
    arguments.push_back("aggregate");

//    print_data("hash_table",d,size);

    add_kernel(hash_insert.name,d,hash_insert.src,sStream.str());
    execute(d, hash_insert.name, arguments, parameters, size/hash_insert.local_size, 1);

    add_data("result",result,d,size);
    arguments.clear();


    arguments.push_back("input");
    arguments.push_back("slots");
    arguments.push_back("hash_table");
    arguments.push_back("result");

    add_kernel(hash_probe.name,d,hash_probe.src,sStream.str());
    execute(d, hash_probe.name, arguments, parameters, size/hash_probe.local_size, 1);

    unsigned int* ph = get_data("hash_table",d,size);
    unsigned int* sl = get_data("slots",d,size);
//
//    for (int j = 0; j < size; ++j) {
//        cout<<input[j]<<"---"<<sl[j]<<endl;
//    }

    unsigned int* get_pos = get_data("result",d,size);
    unsigned int* get_agg = get_data("aggregate",d,size);

    for (int i = 0; i < size; ++i) {
        if(ph[i])
            cout<<ph[i]<<"\t"<<get_agg[i]<<endl;
    }

//    print_data("input",d,size);
}