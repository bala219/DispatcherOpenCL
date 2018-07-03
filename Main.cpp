//
// Created by gurumurt on 3/19/18.
//

#include "include/Environment.h"
#include "include/primitives/header/BitonicSorting.h"
#include "include/primitives/header/Aggregating.h"
#include "include/distribution.h"
#include "include/primitives/VariantTest/MergeVariantTest.h"
#include "include/primitives/VariantTest/VariantTesting.h"
#include "include/primitives/header/Merging.h"

void call();
void call_BitonicSort(cl_device_id _DEVICE, uint m_arr[], int m_size);
void call_Merge(cl_device_id _DEVICE);
void call_Aggregation(cl_device_id _DEVICE, uint m_arr[], int m_size);

int main(int argc, char *argv[]) {

    bs_evaluation(9); // loop for executing the bitonic sort in
    evaluate_Merge(15,256,5); //(Power of max element size allowed, max_work_group_size, loop_size)
    agg_evaluation(4);
    //call();

    return 0;
}

void call() {

    // Generate random numbers
    int m_size = pow(2, 8);

    cout << "ELEMENT SIZE :: " << m_size << endl;

    // Set the Generate Size in distribution.h file
    setGen(m_size);

    // Create an array of _size
    uint m_rand_arr[m_size];
    for (int i = 0; i < m_size; i++) {
        m_rand_arr[i] = UniformRandom();
    }

    setup_environment();
    print_environment();

    // Get Device ID
    //cl_device_id CPU, GPU;
    cl_device_id DEVICE;

    DEVICE = device[0][0]; // GPU
    //DEVICE = device[0][1]; // CPU

    cl_ulong DEV_SIZE;
    //clGetDeviceInfo(DEVICE, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(cl_ulong), &DEV_SIZE, NULL);

    clGetDeviceInfo(DEVICE, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &DEV_SIZE, NULL); //CL_DEVICE_MAX_COMPUTE_UNITS //CL_DEVICE_GLOBAL_MEM_SIZE //CL_DEVICE_LOCAL_MEM_SIZE
    cout << "DEV_SIZE :: " << DEV_SIZE << "\tDEV SIZE :: " << log(DEV_SIZE)/log(2) << endl;

    // Initialize the events required
    //evt = new cl_event[3];

    //Main().call_BitonicSort(GPU, evt[0]);
    /*Main().*/ call_BitonicSort(DEVICE, m_rand_arr, m_size);
    cout << "EXECUTION TIME :: " << execution_time_ns / 1000000.0 << endl;

    /*Main().*/ //call_Merge(DEVICE);
    /* Main().*/ call_Aggregation(DEVICE, m_rand_arr, m_size);
    cout << "EXECUTION TIME :: " << execution_time_ns / 1000000.0 << endl;
}

void /*Main::*/call_BitonicSort(cl_device_id _DEVICE, uint m_arr[], int m_size) {

    // Call the Bitonic Sorting
    /*BitonicSorting().*/BitonicSort(_DEVICE, m_arr, m_size);
}

void /*Main::*/call_Merge(cl_device_id _DEVICE) {

    //Creating 2 sorted arrays
    uint _m_size_left = 8;
    uint _m_size_right = 8;
    uint _m_size_chunk = 4;

    uint _m_arr_left[_m_size_left] = {1, 1, 2, 3, 4, 8, 8, 9};
    uint _m_arr_right[_m_size_right] = {1, 3, 5, 8, 11, 12, 13, 14};
    int *_m_arr_result = (int *) calloc((int)_m_size_left, sizeof(int));
    //int _m_arr_result[_m_size_left];

    // Call merge function
    Merge(_DEVICE, _m_arr_left, _m_arr_right, _m_arr_result, _m_size_left, _m_size_right,_m_size_chunk);

}

void /*Main::*/call_Aggregation(cl_device_id _DEVICE, uint m_arr[], int m_size) {

    // Call the Aggregation
    /*Aggregating().*/ //Aggregate(_DEVICE, _m_arr, _m_size);
    /*Aggregation().*/ Aggregate(_DEVICE, m_arr, m_size, 16, 16);

    //print_execution_time(evt);


}