//
// Created by gurumurt on 4/11/18.
//

#ifndef OPENCLDISPATCHER_DAG_QUERY_H
#define OPENCLDISPATCHER_DAG_QUERY_H

#endif //OPENCLDISPATCHER_DAG_QUERY_H

#include "headers.h"
#include "globals.h"

class node {

    pair<string, cl_device_id> id;
    cl_kernel kernel;
    bool type;
    node* dependent_node;
};