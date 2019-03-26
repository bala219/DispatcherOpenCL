//
// Created by gurumurt on 11/6/18.
//

#ifndef EXEC_ENGINE_COMPARISON_HEADERS_H
#define EXEC_ENGINE_COMPARISON_HEADERS_H

#pragma once

#include <iostream>
//#include <fstream>
//#include <vector>
#include <bitset>
#include <boost/chrono.hpp>
#include <iomanip>
#include <map>
#include <vector>
#include <list>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

enum RETURN_TYPE {
    INT,
    FLOAT,
    DOUBLE,
    CHAR
};

#endif //EXEC_ENGINE_COMPARISON_HEADERS_H
