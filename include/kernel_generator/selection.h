//
// Created by gurumurt on 4/6/18.
//

#ifndef OPENCLDISPATCHER_SELECTION_H
#define OPENCLDISPATCHER_SELECTION_H

#endif //OPENCLDISPATCHER_SELECTION_H

#include <cstring>
#include "../Environment.h";
#include "../globals.h";

class selection_kernel {

public:

    string name = "selection";
    string kernel_src;
    int global_size;
    selection_kernel(string name){
        generate_kernel(name);
    }

    void set_global_size(int tuples){
        global_size /=32;
    }

    void generate_kernel(string op, string additional_name = "selection") {

        name = additional_name + op;

        if (op.compare("between") == 0) {
            kernel_src = "__kernel void " + name +
                         "("
                                 "   __global unsigned int* data,"
                                 "   __global unsigned int* bitmap,"
                                 "unsigned int c1, unsigned int c2) {"
                                 " size_t pos = 32 * get_global_id(0); "
                                 " unsigned int bm = 0;"
                                 " for (unsigned int i=0; i<32; ++i) {"
                                 "   bm |= ((c1 < data[pos])&(data[pos] < c2))  ? 0x1 << i : 0;"
                                 "   pos++;"
                                 " }"
                                 " bitmap[get_global_id(0)] = bm;"
                                 "}";
            global_size /=32;

        } else if (op.compare("<") == 0) {
            kernel_src = "__kernel void " + name +
                         "("
                                 "   __global unsigned int* data,"
                                 "   __global unsigned int* bitmap,"
                                 "   unsigned int c) {"
                                 " size_t pos = 32 * get_global_id(0); "
                                 " unsigned int bm = 0;"
                                 " for (unsigned int i=0; i<32; ++i) {"
                                 "   bm |= data[pos++] < c  ? 0x1 << i : 0;"
                                 " }"
                                 " bitmap[get_global_id(0)] = bm;"
                                 "}";
        } else if (op.compare("=") == 0) {
            kernel_src = "__kernel void " + name +
                         "("
                                 "   __global unsigned int* data,"
                                 "   __global unsigned int* bitmap,"
                                 "   unsigned int c) {"
                                 " size_t pos = 32 * get_global_id(0); "
                                 " unsigned int bm = 0;"
                                 " for (unsigned int i=0; i<32; ++i) {"
                                 "   bm |= data[pos++] == c  ? 0x1 << i : 0;"
                                 " }"
                                 " bitmap[get_global_id(0)] = bm;"
                                 "}";
        } else {
            kernel_src = "__kernel void " + name +
                         "("
                                 "   __global unsigned int* data,"
                                 "   __global unsigned int* bitmap,"
                                 "   unsigned int c) {"
                                 " size_t pos = 32 * get_global_id(0); "
                                 " unsigned int bm = 0;"
                                 " for (unsigned int i=0; i<32; ++i) {"
                                 "   bm |= data[pos++] > c  ? 0x1 << i : 0;"
                                 " }"
                                 " bitmap[get_global_id(0)] = bm;"
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