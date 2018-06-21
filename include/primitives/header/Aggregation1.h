//
// Created by tompi on 6/3/18.
//

#ifndef OPENCLDISPATCHER_AGGREGATION_H
#define OPENCLDISPATCHER_AGGREGATION_H

void BitMask(cl_device_id _DEVICE, uint *_sorted_elements, uint _element_size, uint *_bit_mask);
uint *PrefixSum(uint *_m_bit_mask, uint _element_size);

#pragma once

#include "../../data_api.h"
#include "../../kernel_api.h"
#include "../../runtime_api.h"
#include "../../ImportKernelSource.h"

/*class Aggregation1 {
    public:
        void Aggregate(cl_device_id _DEVICE, int _sorted_elements[], uint _element_size, uint _no_distinct_val);
};/**/

void Aggregate(cl_device_id _DEVICE, uint *_sorted_elements, uint _element_size) {

    // Create BitMask
    uint *_m_bit_mask = new uint[_element_size];

    // Call the function for Calculating the BitMask
    BitMask(_DEVICE, _sorted_elements, _element_size, _m_bit_mask);

    // Fetch Bit Mask Array from buffer
    read_data(BIT_MASKED, _DEVICE, _m_bit_mask, _element_size);

    // Call function for Calculating PrefixSum
    uint *_m_prefix_sum = PrefixSum(_m_bit_mask, _element_size);

    // Calculate Unique Element Count & Make Resultant array for Aggregation
    uint _m_unique_element = _m_prefix_sum[_element_size - 1] + 1;
    uint *_m_agg_res = new uint[_m_unique_element];

    // Calculate the count of each element
    // Add data to the respective devices
    add_data(PREFIX_SUM, _m_prefix_sum, _DEVICE, _element_size);
    add_data(RESULTANT_ARR, _m_agg_res, _DEVICE, _m_unique_element);

    lookup_data_buffer();

    //print_data(PREFIX_SUM, _DEVICE, _element_size);
    //print_data(RESULTANT_ARR, _DEVICE, _element_size);

    //Add kernels to the respective devices
    string kernel_name = KERNEL_AGGREGATE;
    string kernel_src = readKernelFile("../include/primitives/kernel/Aggregate1.cl");

    // Add the values that you need for defining at Kernel
    stringstream _sStream;
    _sStream << "-DARR_SIZE=" << _element_size << " -DUNIQUE_ELEMENT=" << _m_unique_element << " -DITERATOR=" << ITERATOR;

    if (!kernel_src.empty()) {
        add_kernel(kernel_name, _DEVICE, kernel_src, _sStream.str());

        // Pass array of elements
        vector<string> arguments;
        arguments.push_back(PREFIX_SUM);
        arguments.push_back(RESULTANT_ARR);

        // Pass constant Params
        vector<int> param;

        execute(_DEVICE, kernel_name, event, arguments, param, _element_size / ITERATOR, _element_size / ITERATOR);

        cout << endl;
        print_data(RESULTANT_ARR, _DEVICE, _m_unique_element);

    } else {
        cout << "Error in reading kernel source file.\n";
    }

}

void BitMask(cl_device_id _DEVICE, uint *_sorted_elements, uint _element_size, uint *_bit_mask) {

    // Add data to the respective devices
    add_data(ELEMENT_ARR, _sorted_elements, _DEVICE, _element_size);
    add_data(BIT_MASKED, _bit_mask, _DEVICE, _element_size);

    lookup_data_buffer();

    //print_data(ELEMENT_ARR, _DEVICE, _element_size);
    //print_data(BIT_MASKED, _DEVICE, _element_size);

    //Add kernels to the respective devices
    string kernel_name = KERNEL_BIT_MASKED;
    string kernel_src = readKernelFile("../include/primitives/kernel/BitMasked.cl");

    // Add the values that you need for defining at Kernel
    stringstream _sStream;
    _sStream << "-DARR_SIZE=" << _element_size << " -DITERATOR=" << ITERATOR;

    if (!kernel_src.empty()) {
        add_kernel(kernel_name, _DEVICE, kernel_src, _sStream.str());

        // Pass array of elements
        vector<string> arguments;
        arguments.push_back(ELEMENT_ARR);
        arguments.push_back(BIT_MASKED);

        // Pass constant Params
        vector<int> param;

        execute(_DEVICE, kernel_name, event, arguments, param, _element_size / ITERATOR, _element_size / ITERATOR);

        //cout << endl;
        //print_data(ELEMENT_ARR, _DEVICE, _element_size);
        //cout << endl;
        //print_data(BIT_MASKED, _DEVICE, _element_size);

    } else {
        cout << "Error in reading kernel source file.\n";
    }
}

uint *PrefixSum(uint *_bit_mask, uint _element_size) {

    uint *_prefix_sum = new uint[_element_size];

    for (int i = 0; i < _element_size; i++) {

        if(i == 0) {
            _prefix_sum[i] = i;
        } else {
            _prefix_sum[i] = _prefix_sum[i - 1] + _bit_mask[i];
        }
    }
    return _prefix_sum;
}

#endif //OPENCLDISPATCHER_AGGREGATION_H
