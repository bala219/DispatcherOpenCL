//
// Created by gurumurt on 4/6/18.
//

#ifndef OPENCLDISPATCHER_SELECTION_H
#define OPENCLDISPATCHER_SELECTION_H

#endif //OPENCLDISPATCHER_SELECTION_H

#include <cstring>
#include "../Environment.h";
#include "../globals.h";

class logical_kernel {

public:

    string name = "logical";
    string kernel_src;
    int global_size;

    logical_kernel(){
//        generate_kernel(name);
    }

    logical_kernel(string name){
        generate_kernel(name);
    }

    void set_global_size(int tuples){
        global_size =tuples/32;
    }



    void generate_kernel(string op, string additional_name = "logical") {

        name = additional_name + op;

        if (op.compare("and") == 0) {

            kernel_src = "__kernel void "+ name +" (   __global unsigned int* data1,   __global unsigned int* data2,   __global unsigned int* bitmap)\n"
                    "{\n"
                    "    bitmap[get_global_id(0)] = data1[get_global_id(0)] & data2[get_global_id(0)];\n"
                    "}";

        } else if (op.compare("or") == 0) {

                    kernel_src = "__kernel void "+ name +" (   __global unsigned int* data1,   __global unsigned int* data2,   __global unsigned int* bitmap)\n"
                    "{\n"
                    "    bitmap[get_global_id(0)] = data1[get_global_id(0)] | data2[get_global_id(0)];\n"
                    "}";
        } else {

            kernel_src = "__kernel void "+ name +" (   __global unsigned int* data1,   __global unsigned int* data2,   __global unsigned int* bitmap)\n"
                    "{\n"
                    "    bitmap[get_global_id(0)] = data1[get_global_id(0)] != data2[get_global_id(0)];\n"
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