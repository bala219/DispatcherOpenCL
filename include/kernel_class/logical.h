//
// Created by gurumurt on 4/11/18.
//

#ifndef OPENCLDISPATCHER_LOGICAL_H
#define OPENCLDISPATCHER_LOGICAL_H

#endif //OPENCLDISPATCHER_LOGICAL_H
#include <cstring>
#include "../Environment.h";
#include "../globals.h";


class logical_kernel {

public:

    string name = "logical";
    string kernel_src;
    int global_size;
    logical_kernel(string name){
        generate_kernel(name);
    }

    void set_global_size(int tuples){
        global_size /=32;
    }

    void generate_kernel(string op, string additional_name = "logical") {

        name = additional_name + op;

        if (op.compare("AND") == 0) {
            kernel_src = "__kernel void " + name +
                         "("
                                 "   __global unsigned int* data1,"
                                 "   __global unsigned int* data2,"
                                 "   __global unsigned int* bitmap) {"
                                 " bitmap[get_global_id(0)] = data1[get_global_id(0)] & data2[get_global_id(0)];"
                                 "}";
            global_size /=32;

        } else if (op.compare("OR") == 0) {
            kernel_src = "__kernel void " + name +
                         "("
                                 "   __global unsigned int* data1,"
                                 "   __global unsigned int* data2,"
                                 "   __global unsigned int* bitmap) {"
                                 " bitmap[get_global_id(0)] = data1[get_global_id(0)] | data2[get_global_id(0)];"
                                 "}";
        } else if (op.compare("XOR") == 0) {
            kernel_src = "__kernel void " + name +
                         "("
                                 "   __global unsigned int* data1,"
                                 "   __global unsigned int* data2,"
                                 "   __global unsigned int* bitmap) {"
                                 " bitmap[get_global_id(0)] = data1[get_global_id(0)] ^ data2[get_global_id(0)];"
                                 "}";
        }
    }

    string get_kernel_name(){
        return this->name;
    }

    string get_kernel_source(){
        return this->kernel_src;
    }
};