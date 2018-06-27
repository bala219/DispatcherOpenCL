//
// Created by tompi on 6/3/18.
//

#ifndef OPENCLDISPATCHER_AGGREGATION_H
#define OPENCLDISPATCHER_AGGREGATION_H

#pragma once

#include "../../data_api.h"
#include "../../kernel_api.h"
#include "../../runtime_api.h"
#include "../../ImportKernelSource.h"

void IndexValue(cl_device_id _DEVICE, uint *_sorted_elements, uint _element_size, uint *_offset_arr, uint _offset_size) ;

/*class Aggregating {
    public:
        void Aggregate(cl_device_id _DEVICE, int _sorted_elements[], uint _element_size, uint _no_distinct_val);
};/**/

void Aggregate(cl_device_id _DEVICE, uint *_sorted_elements, uint _element_size) {

    uint _m_offset_size = _element_size/ITERATOR;
    uint *_m_offset_arr = (uint *) calloc((uint)_m_offset_size, sizeof(uint));

    // Call function for calculating Prefix Sum
    IndexValue(_DEVICE, _sorted_elements, _element_size, _m_offset_arr, _m_offset_size);

    // Fetch Offset Value from Buffer
    read_data(OFFSET_ARR, _DEVICE, _m_offset_arr, _m_offset_size);

    // Get unique count of elements & Make an array for Result of unique count
    uint _m_unique_count = _m_offset_arr[_m_offset_size - 1] + 1;
    uint *_m_agg_res = (uint *) calloc((uint)_m_unique_count, sizeof(uint));

    // Preparing to calling Kernel
    add_data(RESULTANT_ARR, _m_agg_res, _DEVICE, _m_unique_count);

    lookup_data_buffer();

    //Add kernels to the respective devices
    string kernel_name = KERNEL_AGGREGATE;
    string kernel_src = readKernelFile("../include/primitives/kernel/Aggregate.cl");

    // Add the values that you need for defining at Kernel
    stringstream _sStream;
    _sStream << "-DARR_SIZE=" << _element_size << " -DITERATOR=" << ITERATOR;

    if (!kernel_src.empty()) {
        add_kernel(kernel_name, _DEVICE, kernel_src, _sStream.str());

        // Pass array of elements
        vector<string> arguments;
        arguments.push_back(INDEX_ARR);
        arguments.push_back(OFFSET_ARR);
        arguments.push_back(RESULTANT_ARR);

        // Pass constant Params
        vector<int> param;

        execute(_DEVICE, kernel_name, evt, arguments, param, _element_size / ITERATOR, ITERATOR);

        cout << endl << "res" << endl;
        print_data(RESULTANT_ARR, _DEVICE, _m_unique_count);

    } else {
        cout << "Error in reading kernel source file.\n";
    }
}

void IndexValue(cl_device_id _DEVICE, uint *_sorted_elements, uint _element_size, uint *_offset_arr, uint _offset_size) {

    uint *_m_index_arr = (uint *) calloc((uint)_element_size, sizeof(uint));

    // Add data to the respective devices
    add_data(ELEMENT_ARR, _sorted_elements, _DEVICE, _element_size);
    add_data(INDEX_ARR, _m_index_arr, _DEVICE, _element_size);
    add_data(OFFSET_ARR, _offset_arr, _DEVICE, _offset_size);

    lookup_data_buffer();

    //print_data(ELEMENTS, _DEVICE, _element_size);

    // Add kernels to the respective devices
    string kernel_name = KERNEL_INDEX_VALUE;
    string kernel_src = readKernelFile("../include/primitives/kernel/IndexValue.cl");

    // Add the values that you need for defining at Kernel
    stringstream _sStream;
    _sStream << "-DARR_SIZE=" << _element_size << " -DOFFSET_SIZE=" << _offset_size << " -DITERATOR=" << ITERATOR;

    if (!kernel_src.empty()) {
        add_kernel(kernel_name, _DEVICE, kernel_src, _sStream.str());

        // Pass array of elements
        vector<string> arguments;
        arguments.push_back(ELEMENT_ARR);
        arguments.push_back(INDEX_ARR);
        arguments.push_back(OFFSET_ARR);

        // Pass constant Params
        vector<int> param;

        execute(_DEVICE, kernel_name, evt, arguments, param, _element_size / ITERATOR, ITERATOR);

        cout << endl << "index arr" << endl;
        print_data(INDEX_ARR, _DEVICE, _element_size);
        cout << endl << "offset" << endl;
        print_data(OFFSET_ARR, _DEVICE, _offset_size);

    } else {
        cout << "Error in reading kernel source file.\n";
    }
}

#endif //OPENCLDISPATCHER_AGGREGATION_H
