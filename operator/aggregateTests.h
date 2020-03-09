//
// Created by gurumurt on 18/02/20.
//

#ifndef OPENCLDISPATCHER_AGGREGATETESTS_H
#define OPENCLDISPATCHER_AGGREGATETESTS_H

#include "evaluate_atomics.h"


/*
 * Test case variant: SIMPLE ATOMICS
 *
 * Variables {No. of threads, Data size}
 */

/**
 * Tests only on reduce and unique dataset
 *
 * reduce: puts pressure on atomic component in memory controller
 * unique: puts pressure on memory controller for memory accesses
 *
 */
void test_simple_atomics(size_t maxThreadSize=10, size_t max_data_size=28){

    short dev_id = 1;
    short variant_id = 4;
    short dataDis = 0;
    short min_data_size = 10;
    short min_thread_size = 5;
    cout<<"Reduce"<<endl;
    cout<<"data size\tthread size\tvariant\tresult size\tresult\ttime"<<endl;
    for (int v_num = variant_id; v_num < variant_id+2; ++v_num) {
        for (int d_size = min_data_size; d_size <=max_data_size; ++d_size) {
            for (int t_size = min_thread_size; t_size <=maxThreadSize; ++t_size) {

                prepare_input(dataDis , pow(2,d_size), pow(2,d_size-2));
                cout<<d_size<<"\t"<<t_size<<"\t";
                test_atomic_operation(v_num ,dev_id,0,d_size,1,pow(2,t_size));
                cout<<endl;
            }
        }
    }

    dataDis = 1;
    cout<<"Unique"<<endl;
    cout<<"data size\tthread size\tvariant\tresult size\tresult\ttime"<<endl;
    for (int v_num = variant_id; v_num < variant_id+2; ++v_num) {
        for (int d_size = min_data_size; d_size <=max_data_size; ++d_size) {
            for (int t_size = min_thread_size; t_size <=maxThreadSize; ++t_size) {

                prepare_input(dataDis, pow(2,d_size), pow(2,d_size-2));
                cout<<d_size<<"\t"<<t_size<<"\t";
                test_atomic_operation(v_num ,dev_id,0,d_size,1,pow(2,t_size));
                cout<<endl;
            }
        }
    }
}

/*
 * Test case variant: BRANCHED ATOMICS - SINGLE VARIABLE
 * Variables {No. of threads, Data size, Intent}
 */

void test_branched_atomics(size_t maxThreadSize=10, size_t max_data_size=28,size_t maxIntentSize=10){

    short dev_id = 1;
    short variant_id = 0;
    short dataDis = 0;
    short min_data_size = 10;
    short min_thread_size = 5;
    short min_intent_size = 5;
    cout<<"Reduce"<<endl;
    cout<<"data size\tthread size\tintent size\tvariant\tresult size\tresult\ttime"<<endl;
    for (int v_num = variant_id; v_num < variant_id+2; ++v_num) {
        for (int d_size = min_data_size; d_size <=max_data_size; ++d_size) {
            for (int t_size = min_thread_size; t_size <=maxThreadSize; ++t_size) {
                for (int intent = min_intent_size ; intent < maxIntentSize; ++intent) {

                    prepare_input(dataDis , pow(2,d_size), pow(2,d_size-2));
                    cout<<d_size<<"\t"<<t_size<<"\t";
                    test_atomic_operation(v_num ,dev_id,0,d_size,1,pow(2,t_size));
                    cout<<endl;
                }
            }
        }
    }

    dataDis = 1;
    cout<<"Unique"<<endl;
    cout<<"data size\tthread size\tvariant\tresult size\tresult\ttime"<<endl;
    for (int v_num = variant_id; v_num < variant_id+2; ++v_num) {
        for (int d_size = min_data_size; d_size <=max_data_size; ++d_size) {
            for (int t_size = min_thread_size; t_size <=maxThreadSize; ++t_size) {

                prepare_input(dataDis, pow(2,d_size), pow(2,d_size-2));
                cout<<d_size<<"\t"<<t_size<<"\t";
                test_atomic_operation(v_num ,dev_id,0,d_size,1,pow(2,t_size));
                cout<<endl;
            }
        }
    }
}

#endif //OPENCLDISPATCHER_AGGREGATETESTS_H
