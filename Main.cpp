//
// Created by gurumurt on 3/19/18.
//

//#include "Main.h"
#include "include/Environment.h"
#include "include/primitives/header/BitonicSorting.h"
#include "include/primitives/header/Aggregating.h"
//#include "include/primitives/header/Aggregation.h"
#include "include/primitives/header/Merging.h"
#include "include/distribution.h"

void call_BitonicSort(cl_device_id _DEVICE, uint m_arr[], int m_size);

void call_Merge(cl_device_id _DEVICE);

void call_Aggregation(cl_device_id _DEVICE, uint m_arr[], int m_size);

int main(int argc, char *argv[]) {

    /*int A = 16, B = 9;

    A = B*(A>B) + A*(A<B);
    B = A*(B>A) + B*(B<A);

    cout << "A :: " << A << "\t B ::" << B;*/

    // Generate random numbers
    int m_size = pow(2, 9);

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
    cl_device_id GPU;

    GPU = device[0][0]; // GPU
    //GPU = device[0][1]; // CPU


    // Initialize the events required
    //evt = new cl_event[3];

    //Main().call_BitonicSort(GPU, evt[0]);
    /*Main().*/ call_BitonicSort(GPU, m_rand_arr, m_size);
    /*Main().*/ //call_Merge(GPU);
    /* Main().*/ call_Aggregation(GPU, m_rand_arr, m_size);

    // Execution Time
    //print_execution_time(evt[0]);
    //print_execution_time(event);

    return 0;
}

void /*Main::*/call_BitonicSort(cl_device_id _DEVICE, uint m_arr[], int m_size) {

    /*int m_size = pow(2, 4);
    int m_rand_arr[m_size];
    for (int i = 0; i < m_size; i++) {
        m_rand_arr[i] = rand() % m_size;
    }*/

    // Call the Bitonic Sorting
    /*BitonicSorting().*/BitonicSort(_DEVICE, m_arr, m_size);
}

void /*Main::*/call_Merge(cl_device_id _DEVICE) {

    //Creating 2 sorted arrays
    int _m_size_left = 8;
    int _m_size_right = 8;


    int _m_arr_left[_m_size_left] = {1, 1, 2, 3, 4, 4, 9, 9};
    int _m_arr_right[_m_size_right] = {1, 3, 5, 8, 11, 12, 13, 14};
    int _m_arr_result[_m_size_left];

    // Call merge function
    Merge(_DEVICE, _m_arr_left, _m_arr_right, _m_arr_result, _m_size_left, _m_size_right);
}

void /*Main::*/call_Aggregation(cl_device_id _DEVICE, uint m_arr[], int m_size) {


    //int _m_size = 16;
    //uint _m_arr[_m_size] = {1, 2, 3, 3, 6, 6, 7, 9, 9, 10, 10, 11, 11, 12, 13, 15};

    //int _m_size = 20;
    //uint _m_arr[_m_size] = {1, 1, 2, 3, 3, 6, 6, 7, 9, 9, 10, 10, 11, 11, 12, 13, 15, 15, 16, 16};

    //int _m_size = 32;
    //uint _m_arr[_m_size] = {1, 2, 3, 3, 6, 6, 7, 9, 9, 10, 10, 11, 11, 12, 13, 15, 15, 15, 16, 16, 16, 16, 17, 19, 19, 20, 21, 21, 21, 22, 23, 25};

    // Call the Aggregation
    /*Aggregating().*/ //Aggregate(_DEVICE, _m_arr, _m_size);
    /*Aggregation().*/ Aggregate(_DEVICE, m_arr, m_size);

    //print_execution_time(evt);


}