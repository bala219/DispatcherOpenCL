//
// Created by tompi on 6/1/18.
//

#ifndef OPENCLDISPATCHER_MERGE_H
#define OPENCLDISPATCHER_MERGE_H

/*
#include "../../data_api.h"
#include "../../runtime_api.h"
#include "../../kernel_api.h"
#include "../../ImportKernelSource.h"
 */

void Merge(cl_device_id _DEVICE, int _arr_left[], int _arr_right[], int _arr_result[], size_t _size_left,
           size_t _size_right) {
    //Adding data to divide
    add_data("_arr_left", _arr_left, _DEVICE, _size_left);
    add_data("_arr_right", _arr_right, _DEVICE, _size_right);
    add_data("_arr_result", _arr_result, _DEVICE, _size_left);

    lookup_data_buffer();

    //printing to verify
    print_data("_arr_left", _DEVICE, _size_left);
    cout << endl;
    print_data("_arr_right", _DEVICE, _size_right);
    cout << endl;

    string kernel_name = KERNEL_MERGE;

    string kernel_src = readKernelFile("../include/primitives/kernel/Merge.cl");
    //string kernel_src="";

    add_kernel(kernel_name, _DEVICE, kernel_src, "");

    // Pass left and right sorted arrays
    vector<string> arguments;
    arguments.push_back("_arr_left");
    arguments.push_back("_arr_right");
    arguments.push_back("_arr_result");

    // Pass size of left and right arrays
    vector<int> param;
    param.push_back(_size_left);
    param.push_back(_size_right);

    size_t local_size = _size_left / 4;
    size_t global_size = _size_left / 4;

    execute(_DEVICE, kernel_name, evt, arguments, param, global_size, local_size);


    //print result array
    print_data("_arr_result", _DEVICE, _size_left);
}

/*class Merging {
public: static void Merge();
};/**/

#endif //OPENCLDISPATCHER_MERGE_H
