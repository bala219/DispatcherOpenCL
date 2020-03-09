//
// Created by tompi on 5/26/18.
//

/*#include "../header/BitonicSorting.h"

void BitonicSorting::BitonicSort(cl_device_id _DEVICE, cl_event _event, int *_elements, size_t _element_size) {

    uint _m_stage = log(_element_size) / log(2);

    // Add data to the respective devices
    add_data("a", _elements, _DEVICE, _element_size);
    //add_data("a",_elements,CPU,_element_size);

    lookup_data_buffer();

    print_data("a", _DEVICE, _element_size);

    //Add kernels to the respective devices
    string kernel_name = "_kernel_bitonic_sort";
    string kernel_src = readKernelFile("../include/primitives/kernel/BitonicSort.cl");

    if(!kernel_src.empty()) {
        // Add the values that you need for defining at Kernel
        stringstream _sStream;
        _sStream << "-DARR_SIZE=" << _element_size;

        add_kernel(kernel_name, _DEVICE, kernel_src, _sStream.str());
        //add_kernel(kernel_name, _DEVICE, kernel_src);

        // Pass array of elements
        vector<string> arguments;
        arguments.push_back("a");

        // Pass contant Params
        vector<int> param;
        param.push_back(_m_stage);

        execute(_DEVICE, kernel_name, _event, arguments, param, _element_size / 2, _element_size / 2);
        //execute(_DEVICE, kernel_name,arguments,_element_size,param);

        cout << endl;
        print_data("a", _DEVICE, _element_size);
        //print_data("a",_DEVICE,_element_size);
    }
    else {
        cout << "Error in reading kernel source file.\n";
    }
}/**/