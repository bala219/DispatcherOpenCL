//
// Created by gurumurt on 12/5/18.
//

#include "../include/headers.h"
#include "../include/Environment.h"
#include "../include/data_api.h"
#include "../include/kernel_api.h"
#include "../include/runtime_api.h"
#include "../include/Importkernel.h"
#include <math.h>
#include <chrono>


using namespace boost;
using namespace std;


unsigned int *input, *aggregates;
size_t dataSize, resultSize;

short dev = 0;
size_t maxLocalMem = 1024; // global size = total data size / iterator size

//desirable global size = 8192
//Iterator size = total size / 8192



/*
 *
 * Distribution generations ; heavy hitter & interleaved
 *
 */

void heavy_hitter(size_t size = 1024, size_t HeavyHitterSize=2){

    for(int i=0; i<size-HeavyHitterSize;i++){
        input[i] = 1;
    }

    for(int i = size-HeavyHitterSize;i<size;i++){
        input[i] = (unsigned int) i;
    }
}

void interleaved(size_t size= 1024, size_t interval = 32){

    unsigned int add_num = 1;
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

        default:    cout<< "Not correct distribution"<<endl;
            break;
    }
}

void print_input(size_t size=1024){

    for(int i=0;i<size;i++)
            cout<<input[i]<<endl;

}


vector<kernelObject> PrefixKernelObjects;
string prefixSumKernels[3] = {"partialPS","interOff","FinalPS"};

void clearBuffers(){

    delete_data("index",device[dev][0]);
    delete_data("offset",device[dev][0]);
    delete_data("res",device[dev][0]);

    clear_data();
}

double aggregationTime = 0;
short Iter = 32;

void computeAggregation(size_t result_size, short variant, short iter=32, short localSize=1024){

    vector<string> aggArgs;
    vector<int> aggParam;

    cl_device_id d = device[dev][0];

    add_new_data<int>("res",d,result_size);
    aggArgs.push_back("index");
    aggArgs.push_back("res");

    stringstream sStream;
    sStream << " -DDATA_SIZE=" << dataSize<< " -DOFFSET_SIZE=" << result_size << " -DITERATOR=" << iter;

    add_kernel(av[variant].name,d,av[variant].src,sStream.str());

    short no_of_repeat = 10;
    aggregationTime = 0;

    for (int i = 0; i < no_of_repeat + 1; ++i) {

        if(variant)
            execute(d, av[variant].name, aggArgs, aggParam, dataSize/iter, (localSize>maxLocalMem)?maxLocalMem:localSize);
        else
            execute(d, av[variant].name, aggArgs, aggParam, dataSize, (dataSize>maxLocalMem )?maxLocalMem :dataSize);

        if(i==0)
            continue;

        aggregationTime += nanoSeconds;
    }

    aggregationTime /= no_of_repeat;
    delete_data("res",d);
    delete_kernel(av[variant].name,d);

}

double prefixSumTime = 0;

void compilePrefixSum(){

    //Compiling the kernels

    cl_device_id d = device[dev][0];

    stringstream sStream;
    sStream << " -DDATA_SIZE=" << dataSize << " -DOFFSET_SIZE=" << dataSize / Iter << " -DITERATOR=" << Iter;

    for(string kern : prefixSumKernels){

        string kernelSource = "kernels/prefix_sum/"+ kern +".cl";
        string src = readKernelFile(kernelSource);
        add_kernel(kern,d,src,sStream.str());
    }

}

unsigned int computePrefixSum(){

    prefixSumTime = 0;

    cl_device_id d = device[dev][0];
    add_data("input",input,d,dataSize);
    add_new_data<unsigned int>("index",d,dataSize);
    add_new_data<unsigned int>("offset",d,dataSize/Iter );

    vector<string> arguments;
    vector<int> parameters;

    kernelObject ko;

    ko.name = prefixSumKernels[0];
    ko.arguments.push_back("input");
    ko.arguments.push_back("index");
    ko.globalSize = dataSize/Iter ;
    ko.localSize = (ko.globalSize>maxLocalMem)?maxLocalMem:ko.globalSize;
    PrefixKernelObjects.push_back(ko);

    ko = kernelObject();
    ko.name = prefixSumKernels[1];
    ko.arguments.push_back("index");
    ko.arguments.push_back("offset");
    ko.globalSize = dataSize/Iter ;
    ko.localSize = (ko.globalSize>maxLocalMem)?maxLocalMem:ko.globalSize;
    PrefixKernelObjects.push_back(ko);

    ko = kernelObject();
    ko.name = prefixSumKernels[2];
    ko.arguments.push_back("index");
    ko.arguments.push_back("offset");
    ko.globalSize = dataSize/Iter ;
    ko.localSize = (ko.globalSize>maxLocalMem)?maxLocalMem:ko.globalSize;
    PrefixKernelObjects.push_back(ko);


    Graph kernelGraph;
    typedef pair<size_t,size_t> kernelEdges;
    typedef graph_traits<Graph>::vertex_descriptor kernelVertex;

    kernelVertex partial,inter,final;
    partial = add_vertex(0,kernelGraph);
    inter = add_vertex(1,kernelGraph);
    final = add_vertex(2,kernelGraph);

    add_edge(partial,inter,kernelGraph);
    add_edge(inter,final,kernelGraph);

    typedef std::list<kernelVertex> MakeOrder;
    MakeOrder make_order;
    topological_sort(kernelGraph, std::front_inserter(make_order));

    size_t N = 1;
    std::vector<int> time(N, 0);
    int maxdist = 0;
    for (MakeOrder::iterator i = make_order.begin(); i != make_order.end(); ++i) {
        if (in_degree (*i, kernelGraph ) > 0) {
            Graph::in_edge_iterator j, j_end;

            for (boost::tie(j, j_end) = in_edges(*i,kernelGraph); j != j_end; ++j)
                maxdist = std::max(time[source(*j, kernelGraph)], maxdist);

            time[*i]=maxdist+1;
        }
    }

    vector<cl_event> relatedEvents,returnedEvents, totalEvents;
    Graph::vertex_iterator  iv, iend;
    short prevTime = 0;
    for (boost::tie(iv, iend) = vertices(kernelGraph); iv != iend; ++iv){

        if(time[*iv]==prevTime){

            returnedEvents.push_back(
                    *queueExecution(device[dev][0],
                                    PrefixKernelObjects[kernelGraph[*iv]].name,
                                    PrefixKernelObjects[kernelGraph[*iv]].arguments,
                                    PrefixKernelObjects[kernelGraph[*iv]].parameters,
                                    PrefixKernelObjects[kernelGraph[*iv]].globalSize,&relatedEvents[0],
                                    relatedEvents.size(),
                                    PrefixKernelObjects[kernelGraph[*iv]].localSize));
        }
        else{
            relatedEvents = returnedEvents;
            returnedEvents.clear();
            returnedEvents.push_back(
                    *queueExecution(device[dev][0],
                                    PrefixKernelObjects[kernelGraph[*iv]].name,
                                    PrefixKernelObjects[kernelGraph[*iv]].arguments,
                                    PrefixKernelObjects[kernelGraph[*iv]].parameters,
                                    PrefixKernelObjects[kernelGraph[*iv]].globalSize,
                                    &relatedEvents[0],
                                    relatedEvents.size(),
                                    PrefixKernelObjects[kernelGraph[*iv]].localSize));
            prevTime = time[*iv];
        }
        totalEvents.insert(totalEvents.end(),returnedEvents.begin(),returnedEvents.end());
    }

    executeGraph(d);

    print_data("offset",d,dataSize/Iter);

    delete_data("offset",d);
    delete_data("input",d);

    delete input;

    size_t result_size = get_last_data("index",d,dataSize) + 1;

    delete_data("index",d);
    for(cl_event getTimeEvent : totalEvents){

        extractExecutionTime(&getTimeEvent);
        prefixSumTime +=nanoSeconds;
    }

    return result_size;
}

void testOA(){


    setup_environment();
    initialize_aggregates();

    size_t maxSize = 25;

    for (int maxSize = 10; maxSize < 26; ++maxSize) {

        for (int j = 0; j <=maxSize ; ++j) {

//            ITERATOR = pow(2,maxSize) / maxLocalMem;
            prepareInput(pow(2,maxSize),pow(2,j),0);

            unsigned int result = computePrefixSum();

            for (int iter = 0; iter <= 13; ++iter) {
                for (int localSize = 0; localSize < iter; ++localSize) {

                    computeAggregation(result,0,iter,localSize);

                    cout    <<maxSize
                            <<"\t"<< j
                            <<"\t"<< iter
                            <<"\t"<< localSize
                            <<"\t"<<aggregationTime;

                    computeAggregation(result,2,iter,localSize);
                    cout
                            <<"\t"<<aggregationTime
                            <<endl;
                }

            }

            clearBuffers();
        }

    }

}


void benchmarkVariants(){

    setup_environment();
    initialize_aggregates();
    compilePrefixSum();

    //Input has three parameters
    for (int inputSize = 6; inputSize <= 6; ++inputSize) {
        for (int groupSize = 0; groupSize <= inputSize; ++groupSize) {
            for (int distribution = 0; distribution < 1; ++distribution) {

                prepareInput(pow(2,inputSize),pow(2,groupSize),distribution);

//                Iter = pow(2,inputSize)/maxLocalMem;
                cout<<inputSize
                    <<"\t"<<groupSize
                    <<"\t"<<distribution
//                    <<"\t"<<Iter
//                    ;
                    <<endl;
                unsigned int resultSize = computePrefixSum();

                cout<<"\t"<<resultSize
                    <<"\t"<<prefixSumTime
                          <<endl;
            }
        }
    }

}