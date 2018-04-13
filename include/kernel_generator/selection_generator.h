//
// Created by gurumurt on 4/11/18.
//

#ifndef OPENCLDISPATCHER_SELECTION_GENERATOR_H
#define OPENCLDISPATCHER_SELECTION_GENERATOR_H

#endif //OPENCLDISPATCHER_SELECTION_GENERATOR_H

#include "../headers.h"
#include "../globals.h"
#include "../kernel_class/selection.h"

class generate_selection : public selection1{

    generate_selection(string op){
        generate_source(op);
    }

    string generate_source(string op){

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
};