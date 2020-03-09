//
// Created by gurumurt on 11/1/18.
//

#ifndef OPENCLDISPATCHER_KERNELTESTGRAPH_H
#define OPENCLDISPATCHER_KERNELTESTGRAPH_H

#endif //OPENCLDISPATCHER_KERNELTESTGRAPH_H

#include "headers.h"
#include "globals.h"
#include "../include/Importkernel.h"
#include "data_api.h"
#include "kernel_api.h"
#include "runtime_api.h"
//#include "pipeline/nodeInfo.h"

#include <iostream> // std::cout
#include <utility> // std::pair

#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graphviz.hpp>

using namespace boost;
using namespace std;

size_t sizes = 512;

void initializedataBuffers(){

    int a[512];
//    int **b = (int**)calloc((int)512, sizeof(uint));
    int *c = (int*)calloc((int)512, sizeof(uint));

    for(size_t i =0; i<sizes;i++){
        c[i] = 512 - i;
    }

    for(size_t i =0; i<sizes;i++){
        a[i] = i;
    }

    add_data("sort1",c,device[0][0],sizes);
    add_data("sortPosition",a,device[0][0],sizes);
    add_data("scatter1",a,device[0][0],sizes);

//    add_data("input",a,device[0][0],sizes);
//    add_data("output",b,device[0][0],sizes);
//    add_data("selection1",a,device[0][0],sizes);
//    add_data("selection1Result",b,device[0][0],sizes);
//    add_data("selection2",a,device[0][0],sizes);
//    add_data("selection2Result",b,device[0][0],sizes);
//    add_data("logicalResult",b,device[0][0],sizes);
//    add_data("materializeResult",b,device[0][0],sizes);
//    add_data("arithmeticResult",b,device[0][0],sizes);
//    add_data("aggregateResult",b,device[0][0],1);
}



void connectingKernelsWithBuffes(){

    kernelObject ko;

//    ko.name = executionPrimitives[0];
//    ko.arguments.push_back("selection1");
//    ko.arguments.push_back("selection1Result");
//    ko.parameters.push_back(32);
//    ko.globalSize = 512/32;
//    kernelArr.push_back(ko);
//
//    ko = kernelObject();
//    ko.name = executionPrimitives[0];
//    ko.arguments.push_back("selection2");
//    ko.arguments.push_back("selection2Result");
//    ko.parameters.push_back(32);
//    ko.globalSize = 512/32;
//    kernelArr.push_back(ko);
//
//    ko = kernelObject();
//    ko.name = executionPrimitives[1];
//    ko.arguments.push_back("selection1Result");
//    ko.arguments.push_back("selection2Result");
//    ko.arguments.push_back("logicalResult");
//    ko.globalSize = 512/32;
//    kernelArr.push_back(ko);
//
//    ko = kernelObject();
//    ko.name = executionPrimitives[2];
//    ko.arguments.push_back("selection1");
//    ko.arguments.push_back("logicalResult");
//    ko.arguments.push_back("materializeResult");
//    ko.globalSize = 512/32;
//    kernelArr.push_back(ko);
//
//    ko = kernelObject();
//    ko.name = executionPrimitives[3];
//    ko.arguments.push_back("materializeResult");
//    ko.arguments.push_back("selection1");
//    ko.arguments.push_back("arithmeticResult");
//    ko.globalSize = 512/32;
//    kernelArr.push_back(ko);
//
//    ko = kernelObject();
//    ko.name = executionPrimitives[4];
//    ko.arguments.push_back("arithmeticResult");
//    ko.arguments.push_back("aggregateResult");
//    ko.globalSize = 512/32;
//    kernelArr.push_back(ko);
//

    ko = kernelObject();
    ko.name = executionPrimitives[5];
    ko.arguments.push_back("sort1");
    ko.arguments.push_back("sortPosition");
    uint _m_stage = (uint)(log(512) / log(2));
    ko.parameters.push_back(_m_stage);
    ko.globalSize = 512/2;
    ko.localSize = 512/2;
    kernelArr.push_back(ko);

    ko = kernelObject();
    ko.name = executionPrimitives[6];
    ko.arguments.push_back("sort1");
    ko.arguments.push_back("sortPosition");
    ko.arguments.push_back("scatter1");
    ko.globalSize = 512;
    kernelArr.push_back(ko);
}

void compileKernels(size_t dev = 0){

    cl_device_id d = device[dev][0];
    for(string kern : executionPrimitives){
        string kernelSource = "kernels/mapper/"+ kern +".cl";

//        cout<<"---"<<kern<<"---\n";


        string src = readKernelFile(kernelSource);
//        cout<<src<<endl;
        add_kernel(kern,d,src);
    }
}




void kernelGraphCreation(){

    initializedataBuffers();
    connectingKernelsWithBuffes();



    Graph kernelGraph;

    typedef pair<size_t,size_t> kernelEdges;

    typedef graph_traits<Graph>::vertex_descriptor kernelVertex;

//    kernelVertex select1,select2,logical1,material,ari,aggre;
    kernelVertex sort1, shuffle;

    size_t N = 1;

//    select1 = add_vertex(0,kernelGraph);
//    select2 = add_vertex(1,kernelGraph);
//    logical1 = add_vertex(2,kernelGraph);
//    material = add_vertex(3,kernelGraph);
//    ari = add_vertex(4,kernelGraph);
//    aggre = add_vertex(5,kernelGraph);
    sort1 = add_vertex(0,kernelGraph);
    shuffle = add_vertex(1,kernelGraph);

    add_edge(sort1,shuffle,kernelGraph);
//    add_edge(select1,logical1,kernelGraph);
//    add_edge(select2,logical1,kernelGraph);
//    add_edge(logical1,material,kernelGraph);
//    add_edge(material,ari,kernelGraph);
//    add_edge(ari,aggre,kernelGraph);

    //Testing graphviz
    write_graphviz(std::cout,kernelGraph,my_label_writer(kernelArr));

    typedef std::list<kernelVertex> MakeOrder;
    MakeOrder make_order;
    topological_sort(kernelGraph, std::front_inserter(make_order));

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

//    cout<<"maxdist : "<<maxdist+2<<endl;

    compileKernels();

    vector<cl_event> relatedEvents,returnedEvents;
    Graph::vertex_iterator  iv, iend;
    short prevTime = 0;
    for (boost::tie(iv, iend) = vertices(kernelGraph); iv != iend; ++iv){

        size_t localSize = 1;
        if(kernelArr[kernelGraph[*iv]].localSize == 512/2)
            localSize = 512/2;

        if(time[*iv]==prevTime){

            returnedEvents.push_back(
                    *queueExecution(device[0][0],
                                    kernelArr[kernelGraph[*iv]].name,
                                    kernelArr[kernelGraph[*iv]].arguments,
                                    kernelArr[kernelGraph[*iv]].parameters,
                                    kernelArr[kernelGraph[*iv]].globalSize,&relatedEvents[0],relatedEvents.size(), localSize));
        }
        else{
            relatedEvents = returnedEvents;
            returnedEvents.clear();
            returnedEvents.push_back(
                    *queueExecution(device[0][0],
                                    kernelArr[kernelGraph[*iv]].name,
                                    kernelArr[kernelGraph[*iv]].arguments,
                                    kernelArr[kernelGraph[*iv]].parameters,
                                    kernelArr[kernelGraph[*iv]].globalSize,&relatedEvents[0],relatedEvents.size(),localSize));
            prevTime = time[*iv];
        }
    }

    executeGraph(device[0][0]);

    print_data("scatter1",device[0][0],sizes);
//    print_data("output",device[0][0],sizes);
    cout<<endl;
}