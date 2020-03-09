//
// Created by gurumurt on 10/18/18.
//

#ifndef OPENCLDISPATCHER_KERNELGRAPHS_H
#define OPENCLDISPATCHER_KERNELGRAPHS_H

#endif //OPENCLDISPATCHER_KERNELGRAPHS_H

#include "headers.h"
#include "globals.h"
#include "../include/Importkernel.h"

#include <iostream> // std::cout
#include <utility> // std::pair

#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>


using namespace boost;
using namespace std;

struct primitives{

    string name, src;
}bitmap, logical, materialize, arith,source1;

cl_event evdp;
vector<primitives> KernelArray;

void initialize_primitives(){

    source1.name = "Source data";
    KernelArray.push_back(source1);

    bitmap.name = "selection_block";
    bitmap.src = readKernelFile("kernels/mapper/selection_block.cl");
    KernelArray.push_back(bitmap);

    logical.name = "logical_block";
    logical.src = readKernelFile("kernels/mapper/logical_block.cl");
    KernelArray.push_back(logical);

    materialize.name = "materialize_block";
    materialize.src = readKernelFile("kernels/mapper/materialize_block.cl");
    KernelArray.push_back(materialize);

    arith.name = "arithmetic_block";
    arith.src = readKernelFile("kernels/mapper/arithmetic_block.cl");
    KernelArray.push_back(arith);
};


void compilekernels(size_t dev = 0){

    cl_device_id d = device[dev][0];
    for(primitives kern : KernelArray){

        add_kernel(kern.name,d,kern.src);
    }

}


void kernelGraphs(){


    initialize_primitives();

    typedef adjacency_list<vecS, vecS, bidirectionalS,size_t> Graph;

    Graph kernelGraph;

    typedef pair<size_t,size_t> kernelEdges;

    typedef graph_traits<Graph>::vertex_descriptor kernelVertex;

    kernelVertex select1,select2,logical1,material,ari,sou;

    size_t N = 6;

    sou = add_vertex(0,kernelGraph);
    select1 = add_vertex(1,kernelGraph);
    select2 = add_vertex(1,kernelGraph);
    logical1 = add_vertex(2,kernelGraph);
    material = add_vertex(3,kernelGraph);
    ari = add_vertex(4,kernelGraph);

    add_edge(sou,select1,kernelGraph);
    add_edge(sou,select2,kernelGraph);
    add_edge(select1,logical1,kernelGraph);
    add_edge(select2,logical1,kernelGraph);
    add_edge(logical1,material,kernelGraph);
    add_edge(material,ari,kernelGraph);

    //Make order for the kernels
    typedef std::list<kernelVertex> MakeOrder;
    MakeOrder make_order;
    topological_sort(kernelGraph, std::front_inserter(make_order));

//    std::cout << "make ordering: ";
//    for (MakeOrder::iterator i = make_order.begin();
//         i != make_order.end(); ++i)
//        std::cout <<KernelArray[kernelGraph[*i]].name << " ";
//    std::cout << std::endl;
//
//
//    exit(0);

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

    cout<<"maxdist : "<<maxdist+2<<endl;


    //Compiled execution
    compilekernels();

    int concurrentTime = 0;
    Graph::vertex_iterator  iv, iend;
    short prevTime = 0;
    vector<cl_event> relatedEvents,temp;
    for (boost::tie(iv, iend) = vertices(kernelGraph); iv != iend; ++iv){
        std::cout << "time_slot[" << KernelArray[kernelGraph[*iv]].name << "] = " << time[*iv] << std::endl;

        //enqueue the kernel and get a event as return argument
        vector<string> args,params;


        //Starting events
        if(time[*iv] == 0)
            relatedEvents.push_back(*queueExecution(device[0][0],KernelArray[kernelGraph[*iv]].name,args,params));

        if(prevTime != time[*iv]){
            temp = relatedEvents;
            relatedEvents.clear();
            relatedEvents.push_back(*queueExecution(device[0][0],KernelArray[kernelGraph[*iv]].name,args,params,100,&temp[0]));
        }
    }
    executeGraph(device[0][0]);
}

