//
// Created by tompi on 6/1/18.
//

#ifndef OPENCLDISPATCHER_MERGE_H
#define OPENCLDISPATCHER_MERGE_H

#pragma once
#include "../../data_api.h"
#include "../../runtime_api.h"
#include "../../kernel_api.h"
#include "../../ImportKernelSource.h"


void Merge(cl_device_id _DEVICE, uint _arr_left[], uint _arr_right[], int _arr_result[], uint _size_left,
           uint _size_right, uint _size_chunk) {
    //Adding data to divide
    add_data(LEFT_ARR, _arr_left, _DEVICE, _size_left);
    add_data(RIGHT_ARR, _arr_right, _DEVICE, _size_right);
    add_data(RESULTANT_ARR, _arr_result, _DEVICE, _size_left);
    //add_data("_arr_result", _arr_result, _DEVICE, _size_left);

    //lookup_data_buffer();

    //printing to verify
    //print_data("_arr_left", _DEVICE, _size_left);
    //cout << endl;
    //print_data("_arr_right", _DEVICE, _size_right);
    //cout << endl;

    string kernel_name = KERNEL_MERGE;
    string kernel_src = readKernelFile("../include/primitives/kernel/Merge.cl");
    //string kernel_src="";
    if(!kernel_src.empty()) {
        // Add the values that you need for defining at Kernel
        stringstream _sStream;
        _sStream << "-DITER=" << _size_chunk;

        add_kernel(kernel_name, _DEVICE, kernel_src, _sStream.str());
        //add_kernel(kernel_name, _DEVICE, kernel_src, "");

        // Pass left and right sorted arrays
        vector<string> arguments;
        arguments.push_back(LEFT_ARR);
        arguments.push_back(RIGHT_ARR);
        arguments.push_back(RESULTANT_ARR);
        //arguments.push_back("_arr_result");

        // Pass size of left and right arrays
        vector<int> param;
        param.push_back(_size_left);
        param.push_back(_size_right);

        size_t local_size = _size_left / _size_chunk;
        size_t global_size = _size_left / _size_chunk;

        execute(_DEVICE, kernel_name, arguments, param, global_size, local_size);

        //Read data in RESULTANT_ARR
        read_data(RESULTANT_ARR,_DEVICE,_arr_result, _size_left);

        //print data
        //print_data(RESULTANT_ARR,_DEVICE,_size_left);

        //clear buffer
        clear_data(_DEVICE,arguments);
    }
    else {
        cout << "Error in reading kernel source file.\n";
    }
}


/*class Merging {
public: static void Merge();
};/**/

#endif //OPENCLDISPATCHER_MERGE_H
