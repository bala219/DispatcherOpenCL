//
// Created by tompi on 5/26/18.
//

#pragma once

#include "cmath"
#include "CL/cl.h"
#include "data_api.h"
#include "kernel_api.h"
#include "runtime_api.h"
#include "ImportKernelSource.h"

#ifndef OPENCLDISPATCHER_BITONICSORT_H
#define OPENCLDISPATCHER_BITONICSORT_H

void bitonicSort(cl_device_id _DEVICE, int _elements[], size_t _element_size) {

    uint _m_stage = log(_element_size) / log(2);

    // Add data to the respective devices
    add_data("a", _elements, _DEVICE, _element_size);
    //add_data("a",_elements,CPU,_element_size);

    lookup_data_buffer();

    print_data("a", _DEVICE, _element_size);

    //Add kernels to the respective devices
    string kernel_name = "_kernel_bitonic_sort";
    string kernel_src = readKernelFile("../kernels/bitonicSort.cl", "pass_arg");

    add_kernel(kernel_name, _DEVICE, kernel_src);
    //add_kernel(kernel_name, _DEVICE, kernel_src);

    // Pass array of elements
    vector<string> arguments;
    arguments.push_back("a");

    // Pass contant Params
    vector<int> param;
    param.push_back(_m_stage);

    execute(_DEVICE, kernel_name, arguments, _element_size, param);
    //execute(_DEVICE, kernel_name,arguments,_element_size,param);

    print_data("a", _DEVICE, _element_size);
    //print_data("a",_DEVICE,_element_size);
}

//vector<int> output;
//execute(_DEVICE, kernel_name, arguments, _element_size, output);

//print_data("c",GPU,100);

// No of steps
/*int _m_no_of_steps = log(16) / log(2);
cout << "\nNO. OF STEPS :: " << _m_no_of_steps;

int _m_no_of_groups;
double _m_pow;

for (int i = 1; i <= _m_no_of_steps; i++) { // Loop for STEPS

    _m_pow = pow(2, i);
    _m_no_of_groups = _element_size / _m_pow;
    cout << "\nGROUP OF :: " << _m_pow << "\tNO. OF GROUPS :: " << _m_no_of_groups;

    for (int j = 1; j <= _m_no_of_groups; j++) {

        switch (j % 2) {
            case 1: {
                cout << "\nASCENDING ORDER";
                cout << "\nGROUP NO :: " << j << "\tPASSES :: " << i;
            }
                break;
            case 0: {
                cout << "\nDECENDING ORDER";
                cout << "\nGROUP NO :: " << j << "\tPASSES :: " << i;
            }
                break;
            default: {
                cout << "\nNothing To Do";
            }
        }

        //cout << "\nMODULAR :: " << i % 2;
    }
}/**/

/*
class BitonicSort {
public:
    //
    static void bitonic_sorting();
    //static void env_setup();
    static void bitonic_sorting(cl_device_id _DEVICE, int _elements[], int _arr_size);
    // For Direction of flow of elements
    //extern char ASCENDING = 'A';
    //extern char DESENDING = 'D';
};
 */

#endif //OPENCLDISPATCHER_BITONICSORT_H*/