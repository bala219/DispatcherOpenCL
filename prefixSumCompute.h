//
// Created by gurumurt on 7/9/18.
//

#ifndef OPENCLDISPATCHER_PREFIXSUMCOMPUTE_H
#define OPENCLDISPATCHER_PREFIXSUMCOMPUTE_H

#endif //OPENCLDISPATCHER_PREFIXSUMCOMPUTE_H

#include "include/headers.h"
#include "include/Environment.h"
#include "include/data_api.h"
#include "include/kernel_api.h"
#include "include/runtime_api.h"
#include "include/Importkernel.h"

void prefixSumCompute(cl_device_id _DEVICE, uint *_sorted_elements, uint _element_size, uint *_index_arr, uint *_offset_arr,
                      uint _offset_size, vector<string> _arguments, uint _global_size, uint ITERATOR){
    // Add data to the respective devices
    add_data(ELEMENT_ARR, _sorted_elements, _DEVICE, _element_size);
    add_data(INDEX_ARR, _index_arr, _DEVICE, _element_size);
    add_data(OFFSET_ARR, _offset_arr, _DEVICE, _offset_size);

    // Add kernels to the respective devices
    string kernel_name = KERNEL_INDEX_VALUE;
    string kernel_src = readKernelFile("kernels/prefix_sum/IndexValue.cl");

    // Add the values that you need for defining at Kernel
    stringstream _sStream;
    _sStream << " -DOFFSET_SIZE=" << _offset_size << " -DITERATOR=" << ITERATOR;

    if (!kernel_src.empty()) {
        add_kernel(kernel_name, _DEVICE, kernel_src, _sStream.str());

        // Pass array of elements
        _arguments.push_back(ELEMENT_ARR);
        _arguments.push_back(INDEX_ARR);
        _arguments.push_back(OFFSET_ARR);

        // Pass constant Params
        vector<int> param;

        execute(_DEVICE, kernel_name, _arguments, param, _global_size,1);

        //Final PS compute

        string kernel_name = "FinalPS";
        string kernel_src = readKernelFile("kernels/prefix_sum/FinalPS.cl");

        stringstream _sStream;
        _sStream << " -DITERATOR=" << ITERATOR;

        _arguments.clear();

        add_kernel(kernel_name, _DEVICE, kernel_src, _sStream.str());

        _arguments.push_back(INDEX_ARR);
        _arguments.push_back(OFFSET_ARR);

        execute(_DEVICE, kernel_name, _arguments, param, _global_size,1);

//        print_data(INDEX_ARR, _DEVICE, _element_size);


        //Copy data to another kernel if possible

    } else {
        cout << "Error in reading kernel source file.\n";
    }
}

void preparePS(cl_device_id _DEVICE, uint *_sorted_elements, uint _element_size,int ITERATOR){
    vector<string> arguments;
    uint _m_offset_size = _element_size/ITERATOR;
//    cout<<_m_offset_size<<" : Offset Size"<<endl;
    uint *_m_offset_arr = (uint *) calloc((uint)_m_offset_size, sizeof(uint));
    uint *_m_index_arr = (uint *) calloc((uint)_element_size, sizeof(uint));

    // Call function for calculating Prefix Sum
    prefixSumCompute(_DEVICE, _sorted_elements, _element_size, _m_index_arr, _m_offset_arr, _m_offset_size, arguments,
               _m_offset_size, ITERATOR);
}