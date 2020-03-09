//
// Created by gurumurt on 4/6/18.
//

#ifndef OPENCLDISPATCHER_SELECTION_H
#define OPENCLDISPATCHER_SELECTION_H

#endif //OPENCLDISPATCHER_SELECTION_H

#include <cstring>
#include "../Environment.h";
#include "../globals.h";

class selection_kernel_coal {

public:

    string name = "selection_coal";
    string kernel_src;
    int global_size;

    selection_kernel_coal(){
//        generate_kernel(name);
    }

    selection_kernel_coal(string name){
        generate_kernel(name);

    }

    void set_global_size(int tuples){
        global_size =tuples;
    }



    void generate_kernel(string op, string additional_name = "selection_coal") {

        name = additional_name + op;

        if (op.compare("between") == 0) {
            kernel_src = "__kernel void " + name +
                    "(\n"
                            "        __global unsigned int* in,\n"
                            "        __global unsigned int* out,\n"
                            "        unsigned int cmp1,"
                            "        unsigned int cmp2) {\n"
                            "\n"
                            "    size_t ip_pos = get_global_id(0);\n"
                            "\n"
                            "    size_t result_pos = (get_group_id(0)*get_local_size(0))+(get_local_id(0));\n"
                            "    if ((in[ip_pos] <= cmp1)&&(in[ip_pos] <= cmp2)) atomic_or(&out[result_pos/32], ((uint)0x1) << (get_local_id(0) % 32));"
                            "}";

        } else if (op.compare("<") == 0) {
            kernel_src = "__kernel void " + name +
                         "(\n"
                                 "        __global unsigned int* in,\n"
                                 "        __global unsigned int* out,\n"
                                 "        unsigned int cmp) {\n"
                                 "\n"
                                 "    size_t ip_pos = get_global_id(0);\n"
                                 "\n"
                                 "    size_t result_pos = (get_group_id(0)*get_local_size(0))+(get_local_id(0));\n"
                                 "    if (in[ip_pos] < cmp) atomic_or(&out[result_pos/32], ((uint)0x1) << (get_local_id(0) % 32));";
                                 "}";
        } else if (op.compare("=") == 0) {
            kernel_src = "__kernel void " + name +
                    "(\n"
                            "        __global unsigned int* in,\n"
                            "        __global unsigned int* out,\n"
                            "        unsigned int cmp) {\n"
                            "\n"
                            "    size_t ip_pos = get_global_id(0);\n"
                            "\n"
                            "    size_t result_pos = (get_group_id(0)*get_local_size(0))+(get_local_id(0));\n"
                            "    if (in[ip_pos] == cmp) atomic_or(&out[result_pos/32], ((uint)0x1) << (get_local_id(0) % 32));";
                            "}";
        } else {
            kernel_src = "__kernel void " + name +
                    "(\n"
                            "        __global unsigned int* in,\n"
                            "        __global unsigned int* out,\n"
                            "        unsigned int cmp) {\n"
                            "\n"
                            "    size_t ip_pos = get_global_id(0);\n"
                            "\n"
                            "    size_t result_pos = (get_group_id(0)*get_local_size(0))+(get_local_id(0));\n"
                            "    if (in[ip_pos] > cmp) atomic_or(&out[result_pos/32], ((uint)0x1) << (get_local_id(0) % 32));";
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