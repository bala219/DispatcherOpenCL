//
// Created by bala on 11.12.19.
//

#ifndef OPENCLDISPATCHER_EVALUATE_ATOMICS_H
#define OPENCLDISPATCHER_EVALUATE_ATOMICS_H

#include "../include/headers.h"
#include "../include/Environment.h"
#include "../include/data_api.h"
#include "../include/kernel_api.h"
#include "../include/runtime_api.h"
#include "../include/Importkernel.h"
#include <math.h>
#include <chrono>
#include "../include/common/runtime/Import.hpp"
#include "../include/common/runtime/Database.hpp"

#include <array>
#include <algorithm>
#include <random>
using namespace boost;
using namespace std;

unsigned int *agg_input;
size_t data_size;
size_t res_size;

/*
 * Data Distribution: Reduce
 *
 * This creates a input array of given size and fills with a single value - here 0
 */

void generate_reduce_data(size_t size){

    data_size = size;
    agg_input = (unsigned int*)calloc(data_size,sizeof(unsigned int));

//    std::fill_n(agg_input,size,<any other value>); //Optional: if we need to fill with any other value
}


/********************************
 *  Data Distributions: Ordered *
 ********************************/

int current;
int repeat;
int interval;
int up_bound;

//Sequential
int UniqueNumber (){ return current++;}

//equal interval
int EqualInterval(){ return (repeat++%interval)==0?++current:current;}

//heavy hitter
int heavyHitter(){ return (repeat++<interval)?current:current++;}

//random sorted - data are sorted without any gaps but they repeat random number of times
int generateRandom(){return (int)(std::rand() % (up_bound +1));}


/**
 * Pseudo random distribution
 */

void generate_uniform_random(size_t size, size_t ul){

    std::random_device rd;
    std::mt19937 mte(rd());
    uniform_int_distribution<int> dist(0,ul);
    std::generate_n(agg_input,data_size,[&] () {return dist(mte);});
}

void generate_exponential(size_t size, size_t up_bound, double l){

    std::random_device rd;
    std::mt19937 mte(rd());
    exponential_distribution<> dist(l);
    std::generate_n(agg_input,data_size,[&] () {return (dist(mte)<1.0)?current++:current;});
}

void generate_weibull(size_t size, size_t up_bound, double a=2.0, double b=4.0){

    std::random_device rd;
    std::mt19937 mte(rd());
    weibull_distribution<double> dist(a,b);
    std::generate_n(agg_input,data_size,[&] () {return (dist(mte)<10)?current++:current;});
}

void generate_normal(size_t size, size_t up_bound, int mean=5, int stddev=2){

    std::random_device rd;
    std::mt19937 mte(rd());
    normal_distribution<> dist(mean,stddev);
    std::generate_n(agg_input,data_size,[&] () {while(true) {int v = dist(mte); if (v>=0 && v<up_bound) return v;}});
}

void prepare_input(short type=0, size_t size = 1024, size_t inter=8, double lambda = 0.05){

    if(!agg_input)
        delete agg_input;
    current = 0;
    agg_input = (unsigned int*)calloc(size,sizeof(unsigned int));
    data_size = size;

    switch(type){

        /*  ORDERED DATA DISTRIBUTIONS  */
        //generating reduce dataset
        case 0: break;

        //generating ordered unique - sequential
        case 1: std::generate_n(agg_input,data_size,UniqueNumber);
                break;

        //generating ordered - equal interval
        case 2: repeat=0;
                interval = inter;
                std::generate_n(agg_input,data_size,EqualInterval);
                break;

        //generating ordered - heavy hitter (interval = heavy hitter size)
        case 3: repeat=0;
                interval = data_size - inter;
                std::generate_n(agg_input,data_size,heavyHitter);
                break;

        //generating ordered - random (upper bound = number of unique values possible from 0 and must be less than size_t)
        case 4: up_bound = inter;
                std::generate_n(agg_input,data_size,generateRandom); //default rand distribution
                std::sort(agg_input,agg_input + data_size);
                break;

        //generating ordered - uniform distribution (upper bound = number of unique values / groups possible from 0 and must be less than size_t)
        case 5: generate_uniform_random(data_size,inter);
                std::sort(agg_input,agg_input + data_size);
                break;

        //generating ordered - exponential distribution (upper bound = number of unique values / groups possible from 0 and must be less than size_t)
        case 6: generate_exponential(data_size,inter,lambda);
                break;

        //generating ordered - weibull distribution
        case 7: generate_weibull(data_size,inter);
                break;

        //generating ordered - normal Gaussian distribution
        case 8: generate_normal(data_size,inter);
                std::sort(agg_input,agg_input + data_size);
                break;

        /*  UN-ORDERED DATA DISTRIBUTIONS  */

        //generating unordered - dense unique random
        case 9: std::generate_n(agg_input,data_size,UniqueNumber);
                std::shuffle(agg_input,agg_input+size,std::default_random_engine());
                break;

        //generating unordered - uniform distribution
        case 10: generate_uniform_random(data_size,inter);
                break;

        //generating unordered - exponential distribution
        case 11: generate_exponential(data_size,inter,lambda);
                std::shuffle(agg_input,agg_input+size,std::default_random_engine());
                break;

        //generating unordered - weibull distribution
        case 12: generate_weibull(data_size,inter);
                 std::shuffle(agg_input,agg_input+size,std::default_random_engine());
                 break;

        //generating unordered - normal Gaussian distribution
        case 13: generate_normal(data_size,inter);
                 break;

        //generating ordered - heavy hitter (interval = heavy hitter size)
        case 14:repeat=0;
                interval = data_size - inter;
                std::generate_n(agg_input,data_size,heavyHitter);
                std::shuffle(agg_input,agg_input+size,std::default_random_engine());
                break;

        default: cout<< "Not distribution found"<<endl;
             break;
    }


//    for(int i =0;i<data_size;i++)
//        cout<<agg_input[i]<<endl;

    res_size = *max_element(agg_input,agg_input+data_size) + 1;
}

double barrier_reduce_time =0;
size_t iteration_size = 100;
unsigned int recursive_reduce(short device_ID,
                      size_t start_location,
                      size_t d_size,
                      size_t localSize = pow(2,10)){

    unsigned int* return_result;
    localSize = localSize>d_size?d_size:localSize;

    d_size = (d_size%localSize==0)?d_size:d_size - (d_size%localSize); //making sure the work dimensions are aligned with global memory

    if(d_size==0)
        return 1;

    localSize = localSize>d_size?d_size:localSize;

    cl_device_id d = device[device_ID][0];
    size_t IteratorSize = 32;

    delete_all_kernel();
    stringstream sStream;
    sStream << " -DDATA_SIZE=" << d_size << " -DLOCAL_DATA_SIZE=" << localSize << " -DOFFSET_SIZE=" << data_size  / IteratorSize << " -DITERATOR=" << IteratorSize;
    string kern = "barrier_reduce";
    string kernelSource = "operator/testKernels/"+ kern +".cl";
    string src = readKernelFile(kernelSource);
    add_kernel(kern,d,src,sStream.str());

    vector<string> data_array;
    vector<int> param;
    //Execute with sample data

    double execution_time = 0;

//Simple parallel reduce kernel

        //First iteration with values
        add_new_data<int>("input",d,d_size);
        string old_res = "oldRes";

        add_new_data<int>(old_res,d,d_size/localSize);
        data_array.push_back("input");
        data_array.push_back(old_res);
        execute(d,kern,data_array,param,d_size,localSize,localSize);
        execution_time+=nanoSeconds;
        d_size /=localSize;
        d_size = (d_size%localSize==0)?d_size:d_size - (d_size%localSize); //making sure the work dimensions are aligned with global memory
        string new_res = "newRes";
        delete_data("input",d);

        //Doing simple reduce
        while(d_size >= localSize){ //localSize
            data_array.clear();
            add_new_data<int>(new_res,d,d_size/localSize);
            data_array.push_back(old_res);
            data_array.push_back(new_res);
            execute(d,kern,data_array,param,d_size,localSize,localSize);
            execution_time+=nanoSeconds;
            delete_data(old_res,d);
            old_res = new_res;
            new_res = old_res + "1";
            d_size /=localSize;
        }

        //final iteration
        if(d_size){
            data_array.clear();
            localSize = d_size;
            add_new_data<int>("resFinal",d,1);
            data_array.push_back(old_res);
            data_array.push_back("resFinal");
            execute(d,kern,data_array,param,d_size,localSize,localSize);
            execution_time+=nanoSeconds;
            delete_data(old_res,d);
        }
        return_result =  get_data("resFinal",d,1);
        delete_data("resFinal",d);

        barrier_reduce_time += execution_time;
    return *return_result;
}

/*
 * Software-based reduce operation
 */
void test_barrier_reduce(short device_ID,
                         short data_distribution=0,
                         size_t d_size=pow(2,25),
                         size_t no_of_group = 10,
                         size_t localSize = pow(2,10)){

    time_t start = clock();
    barrier_reduce_time=0;

    localSize = localSize>d_size?d_size:localSize;

    //Setup Opencl
    cl_device_id d = device[device_ID][0];
    size_t IteratorSize = 32;

    //Find group start location from prefix sum
    add_data("input",agg_input,d,d_size);
    add_new_data<int>("res",d,res_size);
    string kern = "barrier_reduce_location";
    string kernelSource = "operator/testKernels/"+ kern +".cl";
    string src = readKernelFile(kernelSource);

    //Execute with sample data
    vector<string> data_array;
    vector<int> param;
    data_array.push_back("input");
    data_array.push_back("res");

    add_kernel(kern,d,src);
    double count_time = 0;
    execute(d,kern,data_array,param,d_size,localSize);//localSize
    count_time = nanoSeconds;
    delete_data("input",d);
    delete agg_input;
    unsigned int *r = get_data("res",d,res_size);
    delete_data("res",d);
    r[res_size - 1] = data_size-1;
    unsigned int total_agg = 0;
    //for each of the group perform reduce
    iteration_size=1;

//    for(int iter = 0; iter<iteration_size;iter++) {
//        for (unsigned int i = 0; i < res_size; i++) {
//
//            size_t group_start = i == 0 ? 0 : r[i];
//            size_t group_end = (i == res_size - 1) ? d_size : r[i + 1];
//            unsigned int aggregate = 1; //default value of aggregation has to be given here
//            if ((group_end-group_start - 2>0)&&(group_end > group_start + 1)) {
//               aggregate = recursive_reduce(device_ID, group_start, group_end - group_start);
//               delete_all_data();
//            }
//            total_agg += aggregate;
//        }
//    }
    //Last value done here

    time_t end = clock();
    cout
//        <<(double)total_agg
        <<"\t"
        <<(double)(count_time + (barrier_reduce_time/iteration_size))
//        <<"\t";
        <<endl;

}


/*
 * Type of atomic being used
 */

string Kernels[9] = {"branched_aggregate",                          //0
                     "branched_aggregate_ordered",                  //1
                     "private_array_sequential_aggregate",          //2
                     "private_array_sequential_aggregate_local",    //3
                     "atomic_local_aggregate_ordered",              //4
                     "atomic_aggregate"                             //5
                      };


string unordered_Kernels[9] = {	"atomic_aggregate",                            //0
                     			"direct_mapped_unordered",              	   //1
                     			"direct_mapped_unordered_local",               //2
                     			"branched_aggregate",                          //3
                     			"branched_aggregate_ordered",                  //4
                     			"private_array_sequential_aggregate",          //5
                     			"private_array_sequential_aggregate_local"};   //6


void test_atomic_operation( short kernel_ID,
                            short device_ID,
                            short data_distribution=0,
                            size_t d_size=pow(2,25),
                            size_t no_of_group = 10,
                            size_t localSize = pow(2,10)){

    localSize=pow(2,10);
    data_size = d_size;
    //Setup Opencl
    size_t max_local_mem = 16384;
    cl_device_id d = device[device_ID][0];
    size_t IteratorSize = 8; //local size of 2¹⁰ can have maximum of 16 values in ITERATOR

    if(kernel_ID != 4 && kernel_ID != 5 ) //These are atomic aggregates that is done individually for each threads
        data_size /=IteratorSize;

    localSize = localSize>data_size?data_size:localSize;
    //Max local memory size : 16384
    //Compile kernel
    stringstream sStream;
    sStream << " -DDATA_SIZE=" << d_size << " -DLOCAL_DATA_SIZE=" << localSize << " -DOFFSET_SIZE=" << data_size  / IteratorSize << " -DITERATOR=" << IteratorSize<<" -DLOCAL_SPACE=" << IteratorSize/2;
    string kern = Kernels[kernel_ID]; //unordered_Kernels
    string kernelSource = "operator/testKernels/"+ kern +".cl";
    string src = readKernelFile(kernelSource);
    add_kernel(kern,d,src,sStream.str());

    cout<<kern<<"\t"<<res_size<<"\t";

    //Execute with sample data
    vector<string> data_array;
    vector<int> param;
    add_data("input",agg_input,d,d_size);
    add_new_data<int>("res",d,res_size);
    data_array.emplace_back("input");
    data_array.emplace_back("res");

//    size_t iteration_size = 1;
    double execution_time = 0;
    for(int i = 0; i <=iteration_size; i++){
        execute(d,kern,data_array,param,data_size,localSize);//localSize

        if(i)
            execution_time += nanoSeconds;
    }

    unsigned int* r = get_data("res",d,1);
    delete_data("input",d);
    delete_data("res",d);

    cout<<r[0]<<"\t"; //*1000000000
    cout<<(double)execution_time/(iteration_size)
//    <<endl; //*1000000000
    <<"\t"; //*1000000000
}


/*
 *
 * Testing TPCH queries
 *
 */


unsigned int *input;
size_t sizeTPCH=0;

string lineItemColumn[15]
        = {
                "l_shipdate",
                "l_quantity",
                "l_discount",
                "l_orderkey",
                "l_partkey",
                "l_suppkey",
                "l_linenumber",
                "l_extendedprice",
                "l_tax",
                "l_returnflag",
                "l_linestatus",
                "l_commitdate",
                "l_receiptdate",
                "l_shipinstruct",
                "l_shipmode"
        };

string partColumn[9]
        ={
                "p_partkey",
                "p_name",
                "p_mfgr",
                "p_brand",
                "p_type",
                "p_size",
                "p_container",
                "p_retailprice",
                "p_comment"
        };

string partsuppColumn[5]
        ={
                "ps_partkey",
                "ps_suppkey",
                "ps_availqty",
                "ps_supplycost",
                "ps_comment"
        };

string customerColumn[8]
        ={
                "c_custkey",
                "c_name",
                "c_address",
                "c_nationkey",
                "c_phone",
                "c_acctbal",
                "c_mktsegment",
                "c_comment"
        };

string ordersColumn[9]
        ={
                "o_orderkey",
                "o_custkey",
                "o_orderstatus",
                "o_totalprice",
                "o_orderdate",
                "o_orderpriority",
                "o_clerk",
                "o_shippriority",
                "o_comment"
        };

multimap<string, string*> TPCHTables;

void InitializeTPCH(){
    TPCHTables.insert(make_pair("lineitem",lineItemColumn));
    TPCHTables.insert(make_pair("orders",ordersColumn));
    TPCHTables.insert(make_pair("customer",customerColumn));
    TPCHTables.insert(make_pair("partsupp",partsuppColumn));
    TPCHTables.insert(make_pair("part",partColumn));
}


void transpose(vector<vector<int> > &b)
{
    if (b.size() == 0)
        return;

    vector<vector<int> > trans_vec(b[0].size(), vector<int>());

    for (int i = 0; i < b.size(); i++)
    {
        for (int j = 0; j < b[i].size(); j++)
        {
            trans_vec[j].push_back(b[i][j]);
        }
    }

    b = trans_vec;    // <--- reassign here
}

static runtime::Database tpch;
multimap<string,string> columns;
vector<unsigned int> inputvector;


size_t tpchSorter(){

    vector<vector<int>> sortCol;
    for(multimap<string, string>::iterator itr = columns.begin(); itr != columns.end();++itr ) {

        size_t colSize = tpch[(*itr).first][(*itr).second].data_.size();
        int* colvals = tpch[(*itr).first][(*itr).second].data<int>();
        vector<int> tmpVector(colvals,colvals + colSize);
        sortCol.push_back(tmpVector);
    }

    //Transpose the vector
    transpose(sortCol);

    //Multidimensional sorting
    std::sort(sortCol.begin(),sortCol.end(),[](const std::vector< int >& a, const std::vector< int >& b){ return a[1] > b[1]; } );

    inputvector.clear();
    unsigned int a = 0;
    inputvector.push_back(a);

    for(int i =1;i<sortCol.size();i++){

        if(std::all_of(sortCol[i].begin(), sortCol[i].end(), [](int ia) { return ia==0; })){
            inputvector.push_back(a);
            continue;
        }

        if(!(std::equal(sortCol[i].begin(),sortCol[i].end(),sortCol[i-1].begin())))
            a++;
        inputvector.push_back(a);
    }

    input = &inputvector[0];

//    for (int j = 0; j < inputvector.size(); ++j)
//        cout<<inputvector[j]<<endl;

//    cout<<"size of inputvector: "<<inputvector.size()<<"\tSize of sortCol: "<<sortCol.size()<<endl;
    res_size=inputvector[inputvector.size()-1];
    return inputvector.size();
}

void TpchBenchmarkAtomics(size_t siz = 10,string query = "query", short device_ID = 1, short kernel_ID=0){


    size_t localSize=pow(2,10);
    data_size = sizeTPCH;
    //Setup Opencl
    size_t max_local_mem = 16384;
    cl_device_id d = device[device_ID][0];
    size_t IteratorSize = 8; //local size of 2¹⁰ can have maximum of 16 values in ITERATOR

    if(kernel_ID != 4 && kernel_ID != 5 ) //These are atomic aggregates that is done individually for each threads
        data_size /=IteratorSize;

    localSize = localSize>data_size?data_size:localSize;
    //Max local memory size : 16384
    //Compile kernel
    stringstream sStream;
    sStream << " -DDATA_SIZE=" << data_size << " -DLOCAL_DATA_SIZE=" << localSize << " -DOFFSET_SIZE=" << data_size  / IteratorSize << " -DITERATOR=" << IteratorSize<<" -DLOCAL_SPACE=" << IteratorSize/2;
    string kern = Kernels[kernel_ID]; //unordered_Kernels
    string kernelSource = "operator/testKernels/"+ kern +".cl";
    string src = readKernelFile(kernelSource);
    add_kernel(kern,d,src,sStream.str());

    cout<<kern<<"\t"<<res_size<<"\t";

    //Find group start location from prefix sum
    add_data("input",input,d,sizeTPCH);
    add_new_data<int>("res",d,res_size);
    kern = "barrier_reduce_location";
    kernelSource = "operator/testKernels/"+ kern +".cl";
    src = readKernelFile(kernelSource);

    //Execute with sample data
    vector<string> data_array;
    vector<int> param;
    data_array.push_back("input");
    data_array.push_back("res");

    add_kernel(kern,d,src);
    double count_time = 0;
    execute(d,kern,data_array,param,sizeTPCH,localSize);//localSize
    count_time = nanoSeconds;
    delete_data("input",d);
//    delete input;

    unsigned int *r = get_data("res",d,res_size);
    delete_data("res",d);
    r[res_size - 1] = sizeTPCH-1;
    unsigned int total_agg = 0;
    //for each of the group perform reduce
    iteration_size=1;

    for(int iter = 0; iter<iteration_size;iter++) {
        for (unsigned int i = 0; i < res_size; i++) {

            size_t group_start = i == 0 ? 0 : r[i];
            size_t group_end = (i == res_size - 1) ? sizeTPCH : r[i + 1];
            unsigned int aggregate = 1; //default value of aggregation has to be given here
            if ((group_end-group_start - 2>0)&&(group_end > group_start + 1)) {
                aggregate = recursive_reduce(device_ID, group_start, group_end - group_start);
                delete_all_data();
            }
            total_agg += aggregate;
        }
    }

    time_t end = clock();
    cout    << query
            <<"\t"
            << sizeTPCH
            <<"\t"
            << res_size
            <<"\t"
            <<total_agg
            <<"\t"
            <<(double)(count_time + (barrier_reduce_time/iteration_size))
            <<endl;
}

void TpchBenchmark(size_t siz = 10,string query = "query", short device_ID = 1) {

    time_t start = clock();
    barrier_reduce_time=0;

    size_t localSize = pow(2,10);
    localSize = localSize>sizeTPCH?sizeTPCH:localSize;

    sizeTPCH = (sizeTPCH%localSize==0)?sizeTPCH:sizeTPCH - (sizeTPCH%localSize);

    cout<<"SIZE TPCH : "<<sizeTPCH<<"\t local : "<<localSize<<endl;
    //Setup Opencl
    cl_device_id d = device[device_ID][0];
    size_t IteratorSize = 32;

    //Find group start location from prefix sum
    add_data("input",input,d,sizeTPCH);
    add_new_data<int>("res",d,res_size);
    string kern = "barrier_reduce_location";
    string kernelSource = "operator/testKernels/"+ kern +".cl";
    string src = readKernelFile(kernelSource);

    //Execute with sample data
    vector<string> data_array;
    vector<int> param;
    data_array.push_back("input");
    data_array.push_back("res");

    add_kernel(kern,d,src);
    double count_time = 0;
    execute(d,kern,data_array,param,sizeTPCH,localSize);//localSize
    count_time = nanoSeconds;
    delete_data("input",d);
//    delete input;

    unsigned int *r = get_data("res",d,res_size);
    delete_data("res",d);
    r[res_size - 1] = sizeTPCH-1;
    unsigned int total_agg = 0;
    //for each of the group perform reduce
    iteration_size=1;

    for(int iter = 0; iter<iteration_size;iter++) {
        for (unsigned int i = 0; i < res_size; i++) {

            size_t group_start = i == 0 ? 0 : r[i];
            size_t group_end = (i == res_size - 1) ? sizeTPCH : r[i + 1];
            unsigned int aggregate = 1; //default value of aggregation has to be given here
            if ((group_end-group_start - 2>0)&&(group_end > group_start + 1)) {
                aggregate = recursive_reduce(device_ID, group_start, group_end - group_start);
                delete_all_data();
            }
            total_agg += aggregate;
        }
    }

    time_t end = clock();
    cout    << query
            <<"\t"
            << sizeTPCH
            <<"\t"
            << res_size
            <<"\t"
            <<total_agg
            <<"\t"
            <<(double)(count_time + (barrier_reduce_time/iteration_size))
            <<endl;
}


void benchExec(string q = "query",short DeviceID = 1){

    sizeTPCH = tpchSorter();
    TpchBenchmark(sizes,q,DeviceID );
    TpchBenchmarkAtomics(sizes,q,DeviceID,0);
    TpchBenchmarkAtomics(sizes,q,DeviceID,1);
    TpchBenchmarkAtomics(sizes,q,DeviceID,2);
    TpchBenchmarkAtomics(sizes,q,DeviceID,3);
    TpchBenchmarkAtomics(sizes,q,DeviceID,4);
    TpchBenchmarkAtomics(sizes,q,DeviceID,5);
}

void TPCHqueriesTest(short DeviceID = 1){

    setup_environment();
    print_environment();
    InitializeTPCH();
    runtime::importTPCH("data/tpch/sf1/",tpch);

    cout<<"DataSize\tGroupSize\tPSTime\tVariant\tTotalSize\tIntent\tGlobalSize\tlocalSize\tAggregate\tAggTime\tComparison\tQuery"<<endl;
    //Testing Q1

    columns.insert(make_pair("lineitem","l_returnflag"));
    columns.insert(make_pair("lineitem","l_linestatus"));
    benchExec("Q1",DeviceID );

    columns.insert(make_pair("orders","o_orderkey"));
    columns.insert(make_pair("orders","o_orderdate"));
    columns.insert(make_pair("orders","o_shippriority"));
    benchExec("Q3",DeviceID );

    columns.insert(make_pair("orders","o_orderpriority"));
    benchExec("Q4",DeviceID );

    columns.insert(make_pair("customer","c_custkey"));
    columns.insert(make_pair("customer","c_name"));
    columns.insert(make_pair("customer","c_acctbal"));
    columns.insert(make_pair("customer","c_address"));
    columns.insert(make_pair("customer","c_phone"));
    columns.insert(make_pair("customer","c_comment"));
    benchExec("Q10",DeviceID );

    columns.insert(make_pair("partsupp","ps_partkey"));
    benchExec("Q11",DeviceID );

    columns.insert(make_pair("part","p_brand"));
    columns.insert(make_pair("part","p_type"));
    columns.insert(make_pair("part","p_size"));
    benchExec("Q16",DeviceID );
}


void test_barrier_TPCH(){


}
#endif //OPENCLDISPATCHER_EVALUATE_ATOMICS_H
