//
// Created by gurumurt on 3/19/18.
//

//#include "Main.h"
#include "include/Environment.h"
#include "include/primitives/header/BitonicSorting.h"
//#include "include/primitives/header/Aggregation.h"
#include "include/primitives/header/Aggregation1.h"
#include "include/primitives/header/Merging.h"

void call_BitonicSort(cl_device_id _DEVICE, cl_event _event);
void call_Merge(cl_device_id _DEVICE);
void call_Aggregation(cl_device_id _DEVICE);

int main(int argc, char *argv[]) {

    //cross_device();
    //test_addition();
    setup_environment();
    print_environment();

    // Get Device ID
    cl_device_id CPU, GPU;

    GPU = device[0][0];
    //CPU = device[0][1];

    // Initialize the events required
    //evt = new cl_event[3];

    //Main().call_BitonicSort(GPU, evt[0]);
    /*Main().*/ //call_BitonicSort(GPU, event);
    /*Main().*/ //call_Merge(GPU);
    /* Main().*/ call_Aggregation(GPU);

    // Execution Time
    //print_execution_time(evt[0]);
    //print_execution_time(event);

    return 0;
}

void /*Main::*/call_BitonicSort(cl_device_id _DEVICE, cl_event _event) {

    // Array of elements
    int _m_size = pow(2, 4);
    int _m_arr[_m_size];
    for (int i = 0; i < _m_size; i++) {
        _m_arr[i] = rand() % _m_size;
    }

    // Call the Bitonic Sorting
    /*BitonicSorting().*/BitonicSort(_DEVICE, _event, _m_arr, _m_size);
}

void /*Main::*/call_Merge(cl_device_id _DEVICE) {

    //Creating 2 sorted arrays
    int _m_size_left=8;
    int _m_size_right=8;


    int _m_arr_left[_m_size_left]={1,1,2,3,4,4,9,9};
    int _m_arr_right[_m_size_right]={1,3,5,8,11,12,13,14};
    int _m_arr_result[_m_size_left];

    // Call merge function
    Merge(_DEVICE,_m_arr_left,_m_arr_right,_m_arr_result,_m_size_left,_m_size_right);
}

void /*Main::*/call_Aggregation(cl_device_id _DEVICE) {

    int _m_size = 16;
    uint _m_arr[_m_size] = {1, 2, 3, 3, 6, 6, 7, 9, 9, 10, 10, 11, 11, 12, 13, 15};

    //int _m_size = 20;
    //uint _m_arr[_m_size] = {1, 1, 2, 3, 3, 6, 6, 7, 9, 9, 10, 10, 11, 11, 12, 13, 15, 15, 16, 16};

    //int _m_size = 32;
    //uint _m_arr[_m_size] = {1, 2, 3, 3, 6, 6, 7, 9, 9, 10, 10, 11, 11, 12, 13, 15, 15, 15, 16, 16, 16, 16, 17, 19, 19, 20, 21, 21, 21, 22, 23, 25};

    // Call the Aggregation
    /*Aggregation().*/ //Aggregate(_DEVICE, _m_arr, _m_size);
    /*Aggregation1().*/ Aggregate(_DEVICE, _m_arr, _m_size);


}