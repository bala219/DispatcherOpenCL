//
// Created by gurumurt on 3/19/18.
//

#ifndef OPENCLTEST_GLOBALS_H
#define OPENCLTEST_GLOBALS_H

#endif //OPENCLTEST_GLOBALS_H

#pragma once

#include "headers.h"

/*
 *
 * openCL constants used inside dispatcher
 *
 */


double nanoSeconds;

cl_int err;

const cl_command_queue_properties cqp = CL_QUEUE_PROFILING_ENABLE;
const cl_mem_flags mem_flag = CL_MEM_READ_WRITE;

/*
 *
 * openCL input variables used in dispatcher
 *
 */


cl_platform_id  * platform;
cl_uint platform_count;

cl_uint *device_local_count;
cl_device_id ** device;

cl_context * context;


static int count;
cl_event* evt;
static size_t num_of_available_events = 0;

double execution_time_ns;

/*
 *
 * Enumeration used
 *
 */

enum Param_Type {
    INTEGER,
    FLOATER,
    DOUBLER,
    CHARER,
    STRINGER
};

enum map_operation {
    BETWEEN,
    LESS_THAN_OR_EQUAL,
    LESS,
    GREATER_THAN_OR_EQUAL,
    GREATER,
    EQUAL,
    NOT_EQUAL,
    AND,
    OR,
    XOR
};

enum access_pattern{
    BLOCKED,
    INTERLEAVED
};

/*
 *
 * Dictionaries used for storing execution objects
 *
 */

struct device_queue{
    cl_context context;
    cl_command_queue queue;
};

struct kernelObject{

    string name;
    vector<string> arguments;
    vector<int> parameters;
    size_t globalSize;
    size_t localSize;
};

class my_label_writer {
public:
    my_label_writer(vector<kernelObject> _name){

        Kernelname = _name;
    }

    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& v) const {
        out << "[label=\"" << Kernelname[v].name << "\"]";
    }
private:
    vector<kernelObject> Kernelname;
};


map<cl_device_id, device_queue> queue;

multimap<pair<string, cl_device_id>, cl_mem> data_dictionary;

map<pair<string, cl_device_id>,cl_kernel> kernel_dictionary;

/*
 *
 * kernel information and other kernel related variables
 *
 */

struct kernel_info{
    string name;
    string src;
    int local_size;
};

const string KERNEL_BITONIC_SORT = "_kernel_bitonic_sort";
const string KERNEL_MERGE = "_kernel_merge";
const string KERNEL_INDEX_VALUE = "_kernel_index_value";
const string KERNEL_AGGREGATE = "_kernel_aggregate";

const string KERNEL_BIT_MASKED = "_kernel_bit_masked";

// KERNEL ARGUMENTS
const string ELEMENT_ARR = "element_arr";
const string INDEX_ARR = "index_arr";
const string OFFSET_ARR = "offset_arr";
const string RESULTANT_ARR = "resultant_arr";
const string LEFT_ARR = "left_arr";
const string RIGHT_ARR = "right_arr";



const string BIT_MASKED = "bit_masked";
const string PREFIX_SUM = "prefix_sum";

cl_mem data_buffer;



typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,size_t> Graph;

typedef pair<size_t,size_t> kernelEdges;

typedef boost::graph_traits<Graph>::vertex_descriptor kernelVertex;

string executionPrimitives[11] = {
        "selection_between_block",
        "selection_less_than_block",
        "selection_greater_than_block",
        "logical_block",
        "materialize_block",
        "arithmetic_block",
        "aggregate_block",
        "partialPS",
        "interOff",
        "FinalPS",
        "atomic_aggregate"
};

vector<kernelObject> kernelArr;
double GraphExecutionTime = 0;
size_t vectorSize = 32;