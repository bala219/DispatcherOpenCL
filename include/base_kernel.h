//
// Created by gurumurt on 4/11/18.
//

#ifndef OPENCLDISPATCHER_BASE_KERNEL_H
#define OPENCLDISPATCHER_BASE_KERNEL_H

#endif //OPENCLDISPATCHER_BASE_KERNEL_H

#include "headers.h"
#include "globals.h"

class base_kernel{
public:

    string name,kernel_src;
    cl_kernel kernel;
    short no_of_argument;
    short no_of_parameter;
    Param_Type param_type[];
};