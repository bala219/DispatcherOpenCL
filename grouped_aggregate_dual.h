//
// Created by gurumurt on 6/19/18.
//

#ifndef OPENCLDISPATCHER_GROUPED_AGGREGATE_DUAL_H
#define OPENCLDISPATCHER_GROUPED_AGGREGATE_DUAL_H

#endif //OPENCLDISPATCHER_GROUPED_AGGREGATE_DUAL_H

#include "include/headers.h"
#include "include/Environment.h"
#include "include/data_api.h"
#include "include/kernel_api.h"
#include "include/runtime_api.h"
#include <math.h>


void dual_aggregate(){

    int data_size[1024];

    string kernel_name = "ga_kernel_one";

    string kernel_src_1 =   "void kernel ga_kerel_one(\n"
                            "        global const int* PS,\n"
                            "        global const int* BM,\n"
                            "        global int* res1,\n"
                            "        global int* res2,\n"
                            "        global int* res3\n"
                            ") {\n"
                            "\n"
                            "size_t pos = 32 * get_global_id(0);\n"
                            "unsigned int local_aggregate = 0;\n"
                            "for (unsigned int i=0; i<32; ++i)\n"
                            "{\n"
                            "local_aggregate++;\n"
                            "if(BM[pos]){\n"
                            "atomic_add(&res[PS[pos]-1],local_aggregate);\n"
                            "local_aggregate=0;\n"
                            "}\n"
                            "pos++;\n"
                            "}\n"
                            "res2[get_global_id(0)] = local_aggregate;\n"
                            "res3[get_global_id(0)] = PS[pos-1];\n";
    string kernel_src_2 =
}