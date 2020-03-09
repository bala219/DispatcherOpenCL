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

#include <algorithm>

using namespace boost;
using namespace std;


/*
 *
 * Distribution generations ; heavy hitter & interleaved
 *
 */

//string AggregationKernels[4] = {"atomic_aggregate_local","atomic_sequential_aggregate_local","branched_aggregate_local","private_array_sequential_aggregate_local"}; //For CPU
//string AggregationKernels[4] = {"atomic_aggregate","atomic_sequential_aggregate","branched_aggregate","private_array_sequential_aggregate"}; //For CPU
string AggregationKernels[9] = {"atomic_aggregate","atomic_sequential_aggregate","branched_aggregate","private_array_sequential_aggregate",
                                "atomic_aggregate_local_test","atomic_sequential_aggregate_local_test","branched_aggregate_local_test",
                                "private_array_sequential_aggregate_local_test",/*"He_method"*/"barrier_based"};

//                                "branched_aggregate_local","private_array_sequential_aggregate_local"}; //For GPU
//string AggregationKernels[4] = {"atomic_aggregate","atomic_sequential_aggregate_GPU_local","branched_aggregate_GPU","private_array_sequential_aggregate_GPU"}; //For GPU

cl_device_id d;
int maxLocalMem = 1024;
short dev = 0;

void initDevice(short dev = 0){

    d = device[dev][0];

    if(dev)
        maxLocalMem = 1024;

    else
        maxLocalMem = 1024;



}

unsigned int *input;
size_t dataSize = 0;

/*
 * Real-life distributions
 */

void expo_distribution(size_t size=1024,double lambda = 0.5){

    for(int i=0;i<size;i++){

        int val = 1 + ((rand()/1.0+RAND_MAX)*(size+1));
        input[i] = (unsigned int) log((double) (val/lambda));
    }
}

void self_similar_distribution(size_t size=1024,double h=1){

    for(int i=0;i<size;i++)
        input[i] = (int) (size * pow(rand(), log(h) / log(1.0 - h)));
}

void heavy_hitter(size_t size = 1024, size_t HeavyHitterSize=2){

    for(int i=0; i<size-HeavyHitterSize;i++){
        input[i] = 0;
    }

    int a = 0;
    for(int i = size-HeavyHitterSize;i<size;i++){
        input[i] = (unsigned int) a++;
    }
}

void interleaved(size_t size= 1024, size_t interval = 32){

    unsigned int add_num = 0;
    for(int i = 0; i < size; i++){
        for(int j = 0; (j<interval-1)&&(i<size-1);j++){
            input[i] = add_num;
            i++;
        }
        input[i] = add_num;
        add_num++;
    }

}

void random_distribution(size_t size= 1024){

    size_t min = 1;
    size_t max = size;
    size_t GroupSize = 0;
    size_t ptr = 0;
    unsigned int add_num = 1;

    while(max){
        GroupSize = min + ( std::rand() % ( max - min + 1 ) );
        for(int i = 0;i<GroupSize;i++){
            input[ptr++] = add_num;
        }
        add_num++;
        max = max - GroupSize;
    }
}

void prepareInput(size_t size = 1024, size_t repeat=32,short type=0){

    if(!input)
        delete input;

    input = (unsigned int*)calloc(size,sizeof(unsigned int));
    dataSize = size;

    switch(type){

        case 0: interleaved(size,repeat);
            break;

        case 1: heavy_hitter(size,repeat);
            break;

        case 2: random_distribution(size);
            break;
        case 3: self_similar_distribution(size,2);
            break;
        case 4: expo_distribution(size);
            break;

        default:    cout<< "Not correct distribution"<<endl;
            break;
    }
}


/*
 * Helper functions
 */

void print_input(size_t size=1024){

    for(int i=0;i<size;i++)
        cout<<input[i]<<endl;

}

void clearBuffers(){

    delete_data("index",device[dev][0]);
    delete_data("offset",device[dev][0]);
    delete_data("res",device[dev][0]);

    clear_data();
}

string PrefixSumKernels[3] = {"partialPS","interOff","FinalPS"};

short IteratorSize = 2048;

void compilePrefixSum(){

    stringstream sStream;
    sStream << " -DDATA_SIZE=" << dataSize << " -DOFFSET_SIZE=" << dataSize / IteratorSize << " -DITERATOR=" << IteratorSize;

    for(string kern : PrefixSumKernels){

        string kernelSource = "kernels/prefix_sum/"+ kern +".cl";
        string src = readKernelFile(kernelSource);
        add_kernel(kern,d,src,sStream.str());
    }

}

double prefixSumTime;

unsigned int prefixSum(){

//    cout<<dataSize<<" : "<<maxLocalMem<<endl;
//    IteratorSize = dataSize /maxLocalMem;
    IteratorSize = 4096;

    compilePrefixSum();

    prefixSumTime = 0;

    add_data("input",input,d,dataSize);
    add_new_data<unsigned int>("index",d,dataSize);
    add_new_data<unsigned int>("offset",d,dataSize/IteratorSize);

    vector<string> arg;
    vector<int> param;

    arg.push_back("input");
    arg.push_back("index");

    unsigned int globalSize = dataSize/IteratorSize;
    unsigned int localSize = globalSize> maxLocalMem ?maxLocalMem:globalSize;

    execute(d,PrefixSumKernels[0],arg,param,globalSize,localSize);
    prefixSumTime += nanoSeconds;

    delete_data("input",d);

    arg.clear();
    arg.push_back("index");
    arg.push_back("offset");

    execute(d,PrefixSumKernels[1],arg,param,globalSize,localSize);
    prefixSumTime += nanoSeconds;

    execute(d,PrefixSumKernels[2],arg,param,globalSize,localSize);
    prefixSumTime += nanoSeconds;

//    print_data("index",d,dataSize);

    unsigned int resultSize = get_last_data("index",d,dataSize) + 1;

    delete_data("offset",d);

    return resultSize;
}

/*
 * Atomic free count aggregation
 */

kernel_info GeneratePosition, FinalAtomicFreeResult;

void initialize_atomic_free(){

    GeneratePosition.name = "GeneratePosList";
    GeneratePosition.src = readKernelFile("kernels/ordered_aggregate_kernels/No_Atomics/GeneratePosList.cl");

    FinalAtomicFreeResult.name = "FinalAggregate";
    FinalAtomicFreeResult.src = readKernelFile("kernels/ordered_aggregate_kernels/No_Atomics/FinalResult.cl");
}

double ac_time;
double AggregationTime;
unsigned int* parallelResult;
unsigned int Compute_atomic_free_aggregate(size_t resultSize){

    initialize_atomic_free();
    AggregationTime = 0;
    add_new_data<unsigned int>("offsets",d,resultSize);

    vector<string> arguments;
    vector<int> parameters;

    arguments.push_back("index");
    arguments.push_back("offsets");

    stringstream sStream;
    sStream << " -DDATA_SIZE=" << dataSize << " -DOFFSET_SIZE=" << resultSize << " -DITERATOR=" << IteratorSize;

    add_kernel(GeneratePosition.name,d,GeneratePosition.src,sStream.str());
    execute(d, GeneratePosition.name, arguments, parameters, dataSize, 1);
    AggregationTime = nanoSeconds;

    arguments.clear();
    add_new_data<unsigned int>("result",d,resultSize);

    arguments.push_back("offsets");
    arguments.push_back("result");

    add_kernel(FinalAtomicFreeResult.name,d,FinalAtomicFreeResult.src,sStream.str());
    execute(d, FinalAtomicFreeResult.name, arguments, parameters, resultSize, 1);
    AggregationTime += nanoSeconds;

    parallelResult = get_data("result",d,resultSize);

    unsigned int final_result_aggregate = get_first_data("result",d,resultSize);

    delete_data("offsets",d);
    delete_data("result",d);

    return final_result_aggregate;
}

/*
 * Aggregation computation
 *
 */

unsigned int computeAggregationAtomicFree(short variant, size_t resultSize, size_t intent = 32,size_t localsize = 32){

    AggregationTime = 0;
    vector<string> ar;
    vector<int> para;

    stringstream sStream;

    sStream << " -DDATA_SIZE=" << dataSize << " -DOFFSET_SIZE=" << resultSize << " -DLOCAL_DATA_SIZE=" << localsize << " -DITERATOR=" << intent;

    string kern = "GeneratePosList";
    string kernelSource = "kernels/ordered_aggregate_kernels/No_Atomics/"+ kern +".cl";
    string src = readKernelFile(kernelSource);
    add_kernel(kern,d,src,sStream.str());

    kernelSource = "kernels/ordered_aggregate_kernels/barrier_based.cl";
    src = readKernelFile(kernelSource);
    add_kernel("barrier_based",d,src,sStream.str());

    add_data("index",input,d,dataSize);
    add_new_data<unsigned int>("offlist",d,resultSize);
    ar.push_back("index");
    ar.push_back("offlist");

    size_t prs = dataSize;
    execute(d, kern,ar,para,dataSize-1,1);
    ar.clear();

    ar.push_back("index");
    add_new_data<unsigned int>("resultSet",d,resultSize);
    ar.push_back("resultSet");

    size_t itera = 1; //100 it was
    intent = 1;
    unsigned int* offlist = get_data("offlist",d,resultSize);
    size_t ds = dataSize;
    for (int ite = 0; ite < itera + 1; ++ite) {

        for(int ps=0;ps<resultSize;ps++){

            para.clear();
            //Computing data size
            dataSize = (((ps==resultSize -1)?ds-1: offlist[ps]) - ((ps==0)?0:offlist[ps-1]+1) + 1);
            size_t s_ds = dataSize;
            para.push_back((ps==0)?0:offlist[ps-1]+1);
            para.push_back(ps);
            dataSize = s_ds - (s_ds%localsize);

            while(dataSize>localsize && !(dataSize%localsize)){
                execute(d, AggregationKernels[variant],ar,para,dataSize,localsize,1);
                dataSize/=localsize;
            }
            dataSize=s_ds%localsize;
            if(dataSize)
                execute(d, AggregationKernels[variant],ar,para,dataSize,dataSize,1);
        }
        AggregationTime += nanoSeconds;
    }

    AggregationTime /= itera;

    return 1;
}



unsigned int computeAggregation(short variant, size_t resultSize, size_t intent = 32,size_t localsize = 32){
//    cout<<"here - compute aggregation"<<endl;
    AggregationTime = 0;

    stringstream sStream;

    sStream << " -DDATA_SIZE=" << dataSize << " -DOFFSET_SIZE=" << resultSize << " -DLOCAL_DATA_SIZE=" << localsize << " -DITERATOR=" << intent;

    for(string kern : AggregationKernels){

        string kernelSource = "kernels/ordered_aggregate_kernels/"+ kern +".cl";
        string src = readKernelFile(kernelSource);
        add_kernel(kern,d,src,sStream.str());
    }

    add_data("index",input,d,dataSize);
    add_new_data<unsigned int>("result",d,resultSize);
    if(variant == 8){

        add_new_data<unsigned int>("offlist",d,resultSize);
        add_new_data<unsigned int>("poslist",d,resultSize);
    }

    vector<string> ar;
    vector<int> para;

    ar.push_back("index");
    ar.push_back("result");



    size_t itera = 1; //100 it was

    intent = (variant>0)?intent:1;

    size_t globalsize = dataSize / intent;

    for (int ite = 0; ite < itera + 1; ++ite) {

    //    execute(d, AggregationKernels[variant],ar,para,globalsize,localsize,1);

        if(variant==0||variant==4)
            execute(d, AggregationKernels[variant],ar,para,globalsize,localsize,1);
        else
            execute(d, AggregationKernels[variant],ar,para,globalsize,localsize,1,intent);

        if(!ite)
            continue;

        AggregationTime += nanoSeconds;
    }

    AggregationTime /= itera;

    unsigned int final_result_aggregate = get_first_data("result",d,resultSize) / (itera +1);

//    parallelResult = get_data("result",d,resultSize);

//    for(int i=0;i<resultSize;i++)
//            parallelResult[i]/=itera+1;

    delete_data("result",d);

    return final_result_aggregate;
}

unsigned int* serialResult;
void computeSerial(size_t resultSize){

    serialResult = (unsigned int*) calloc(resultSize,sizeof(unsigned int));

    int pointer =0;
    serialResult[pointer]=1;
    for(int i =0;i<dataSize-1;i++){
//        cout<<input[i]<<"\t"<<input[i+1]<<"\t"<<pointer<<endl;
        pointer+=(input[i+1]!=input[i]);
        serialResult[pointer]++;
    }

//    for(int i =0;i<pointer;i++)
//        cout<<serialResult[i]<<endl;
//    exit(0);

}


void compareResult(size_t resultSize){
    cout<<"\nSERIAL\tPARALLEL"<<endl;
    for(int i=0;i<100;i++)
        cout<<serialResult[i]<<"\t"<<parallelResult[i]<<endl;
    cout<<"-----------"<<endl;
}

void BenchMark(){

    setup_environment();

    //
    initDevice(1);
    cout<<"InputSize\tGroupSize\tDistribution\tresultSize\tPSTime\tVariant\tIntent\tlocalSize\tAggregate\tAggTime"<<endl;

    //Input has three parameters
    for (int inSize = 10; inSize< 12; inSize++) {
//    for (int inSize = 10; inSize< 12; inSize++) {
//        dataSize = 0;
        for (int distribution = 0; distribution <2; distribution++) { // give value 3
            for (int groupSize = 0; groupSize <= inSize; groupSize+=inSize){

                prepareInput(pow(2,inSize),pow(2,groupSize),distribution);

//                unsigned int resultSize = prefixSum();
                unsigned int resultSize = input[(int)pow(2,inSize)-1]+1;

                for (int variant = 8; variant <= 8
                        ; variant+=4) {

                    for (int inten = 1; (inten <= pow(2,10)); inten*=2) {

                        int intent = (variant!=0&&variant!=4)?inten:1;

                        size_t globalsize = dataSize / intent;

                        for (int localSize = 2; pow(2,localSize) <= maxLocalMem && (pow(2,localSize) <= globalsize ); ++localSize){

                            cout
                                <<inSize
                                <<"\t"<<groupSize
                                <<"\t"<<distribution
                                <<"\t"<<resultSize
//                                <<"\t"<<prefixSumTime
                                <<"\t"<<variant
                                <<"\t"<<intent
                                <<"\t"<<localSize;

//                            computeSerial(resultSize);
                            unsigned int aggResult;

                            if(variant == 8)
                                aggResult = computeAggregationAtomicFree(variant,resultSize,intent,pow(2,localSize));
                            else
                                aggResult = computeAggregation(variant,resultSize,intent,pow(2,localSize));

//                            unsigned int aggResult = computeAggregation(variant,resultSize,intent,pow(2,localSize));

                            cout
                                <<"\t"<<aggResult
                                <<"\t"<<AggregationTime
//                                <<"\t"<<comparison
                                <<endl;
                        }

                        if(variant ==0 || variant ==4)
                            break;
                    }
                }

                delete_data("index",d);
                delete input;
            }
        }
    }
}

void TestVariant(int variant = 0, int inten_power = 0, int localSize = 10){

    setup_environment();
    initDevice(1);
    cout<<"InputSize\tGroupSize\tDistribution\tresultSize\tPSTime\tVariant\tIntent\tlocalSize\tAggregate\tAggTime"<<endl;

    //Input has three parameters
//    for (int inSize = 27; inSize< 29; inSize++) {
////    for (int inSize = 10; inSize< 12; inSize++) {
////        dataSize = 0;
//        for (int distribution = 0; distribution <2; distribution++) { // give value 3
//            for (int groupSize = 0; groupSize <= inSize; groupSize+=inSize){

                int inSize = 26;
                int distribution = 0; // distribution --> number of unique groups in result === 0 --> 1 | 1 --> 1024
                int groupSize = inSize;
                int inten = pow(2,inten_power);

                prepareInput(pow(2,inSize),pow(2,groupSize),distribution);

                unsigned int resultSize = input[(int)pow(2,inSize)-1]+1;
                int intent = (variant!=0&&variant!=4)?inten:1;

                size_t globalsize = dataSize / intent;

                cout
                        <<inSize
                        <<"\t"<<groupSize
                        <<"\t"<<distribution
                        <<"\t"<<resultSize
                        <<"\t"<<variant
                        <<"\t"<<intent
                        <<"\t"<<localSize;

                unsigned int aggResult;
                if(variant == 8)
                    aggResult = computeAggregationAtomicFree(variant,resultSize,intent,pow(2,localSize));
                else
                    aggResult = computeAggregation(variant,resultSize,intent,pow(2,localSize));
                cout
                        <<"\t"<<aggResult
                        <<"\t"<<AggregationTime
                        <<endl;

                delete_data("index",d);
                delete input;
}

void Atomic_Free_BenchMark(){
    cout<<"Testing atomic free"<<endl;
    setup_environment();
    initDevice(0);
    cout<<"InputSize\tGroupSize\tDistribution\tresultSize\tPSTime\tlocalSize\tAggregate\tAggTime"<<endl;

    //Input has three parameters
    for (int inSize = 24; inSize< 26; inSize++) {
        dataSize = 0;
        for (int distribution = 0; distribution <3; distribution++) { // give value 3
            for (int groupSize = 0; groupSize <= 100; groupSize+=10){

                prepareInput(pow(2,inSize),pow(2,inSize)*groupSize/100,distribution);

                unsigned int resultSize = prefixSum();
//                for (int variant = 5; variant <= 5
//                        ; variant++) {

//                    for (int inten = 2; (inten < pow(2,10)); inten*=2) {
//
//                        int intent = (variant!=0&&variant!=4)?inten:1;
//
                        size_t globalsize = dataSize;

                        for (int localSize = 1; pow(2,localSize) <= maxLocalMem && (pow(2,localSize) <= globalsize ); ++localSize){

                            cout<<inSize
                                <<"\t"<<groupSize
                                <<"\t"<<distribution
                                <<"\t"<<resultSize
                                <<"\t"<<prefixSumTime
                                <<"\t"<<localSize;

                            computeSerial(resultSize);

                            unsigned int aggResult = Compute_atomic_free_aggregate(resultSize);
                            int comparison = memcmp(serialResult, parallelResult,resultSize);
                            cout
                                    <<"\t"<<aggResult
                                    <<"\t"<<AggregationTime
                                    <<"\t"<<comparison
                                    <<endl;
                        }

//                        if(variant ==0 || variant ==4)
//                            break;
//                    }
//                }

                delete_data("index",d);
                delete input;
            }
        }
    }
}

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
    unsigned int a = 1;
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
    return sortCol.size();
}


void TpchBenchmark(size_t siz = 10,string query = "query"){


    siz-=siz%(int)pow(2,floor(log(siz)/log(2)));
//    IteratorSize = 32;
    dataSize = siz;
    unsigned int resultSize = prefixSum();

    //Adding aggregation count
    for (int variant = 0; variant <= 7; variant++) {

        for (int inten = pow(2,4); (inten <= pow(2,4)); inten*=2) {
            int intent = (variant!=0&&variant!=4)?inten:1;
            dataSize = siz;
            size_t globalsize = dataSize / intent;
            for (int localSize = 1; pow(2,localSize) <= maxLocalMem && (pow(2,localSize) <= globalsize ); ++localSize){

                cout<<dataSize
                    <<"\t"<<resultSize
                    <<"\t"<<prefixSumTime
                    <<"\t"<<variant
                    <<"\t"<<dataSize
                    <<"\t"<<intent
                    <<"\t"<<globalsize
                    <<"\t"<<pow(2,localSize);

//                computeSerial(resultSize);
                unsigned int aggResult = computeAggregation(variant,resultSize,intent,pow(2,localSize));
//                int comparison = memcmp(serialResult,parallelResult,resultSize);
//                if(comparison)
//                    compareResult(resultSize);

                cout
                    <<"\t"<<aggResult
                    <<"\t"<<AggregationTime
//                    <<"\t"<<comparison
                    <<"\t"<<query
                    <<endl;
            }

            if(variant ==0 || variant ==4)
                break;
        }
    }
    delete_data("index",d);
}

void benchExec(string q = "query"){

    size_t sizes = tpchSorter();
    TpchBenchmark(sizes,q);
}
void TPCHqueriesTest(){

    setup_environment();
    initDevice(1);
    InitializeTPCH();
    runtime::importTPCH("data/tpch/sf1/",tpch);

    cout<<"DataSize\tGroupSize\tPSTime\tVariant\tTotalSize\tIntent\tGlobalSize\tlocalSize\tAggregate\tAggTime\tComparison\tQuery"<<endl;
    //Testing Q1

    columns.insert(make_pair("lineitem","l_returnflag"));
    columns.insert(make_pair("lineitem","l_linestatus"));
    benchExec("Q1");
//
//    columns.insert(make_pair("orders","o_orderkey"));
//    columns.insert(make_pair("orders","o_orderdate"));
//    columns.insert(make_pair("orders","o_shippriority"));
//    benchExec("Q3");
//
//    columns.insert(make_pair("orders","o_orderpriority"));
//    benchExec("Q4");

//    columns.insert(make_pair("customer","c_custkey"));
//    columns.insert(make_pair("customer","c_name"));
//    columns.insert(make_pair("customer","c_acctbal"));
//    columns.insert(make_pair("customer","c_address"));
//    columns.insert(make_pair("customer","c_phone"));
//    columns.insert(make_pair("customer","c_comment"));
//    benchExec("Q10");

//    columns.insert(make_pair("partsupp","ps_partkey"));
//    benchExec("Q11");

    columns.insert(make_pair("part","p_brand"));
    columns.insert(make_pair("part","p_type"));
    columns.insert(make_pair("part","p_size"));
    benchExec("Q16");
}