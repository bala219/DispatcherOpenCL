//
// Created by tompi on 5/26/18.
//

#ifndef OPENCLDISPATCHER_BITONICSORT_H
#define OPENCLDISPATCHER_BITONICSORT_H

#pragma once

#include "../../data_api.h"
#include "../../kernel_api.h"
#include "../../runtime_api.h"
#include "../../ImportKernelSource.h"


/*class BitonicSorting {
    public:
        void BitonicSort(cl_device_id _DEVICE, cl_event _event, int _elements[], size_t _element_size);
};/**/

void BitonicSort(cl_device_id _DEVICE, uint _elements[], size_t _element_size) {

    uint _m_stage = log(_element_size) / log(2);

    // Add data to the respective devices
    add_data(ELEMENT_ARR, _elements, _DEVICE, _element_size);

    //lookup_data_buffer();

    //print_data(ELEMENT_ARR, _DEVICE, _element_size);

    //Add kernels to the respective devices
    string kernel_name = KERNEL_BITONIC_SORT;
    string kernel_src = readKernelFile("../include/primitives/kernel/BitonicSort.cl");

    if(!kernel_src.empty()) {
        // Add the values that you need for defining at Kernel
        stringstream _sStream;
        _sStream << "-DARR_SIZE=" << _element_size;

        add_kernel(kernel_name, _DEVICE, kernel_src, _sStream.str());

        // Pass array of elements
        vector<string> arguments;
        arguments.push_back(ELEMENT_ARR);

        // Pass contant Params
        vector<int> param;
        param.push_back(_m_stage);

        execute(_DEVICE, kernel_name, arguments, param, _element_size / 2, _element_size / 2);

        //cout << endl;
        //print_data(ELEMENT_ARR, _DEVICE, _element_size);

        // Read from buffer
        read_data(ELEMENT_ARR, _DEVICE, _elements, _element_size);

        // Clear data from buffer
        clear_data(_DEVICE, arguments);
    }
    else {
        cout << "Error in reading kernel source file.\n";
    }

}/**/

#endif //OPENCLDISPATCHER_BITONICSORT_H