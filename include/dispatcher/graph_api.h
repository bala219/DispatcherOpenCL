//
// Created by gurumurt on 1/25/19.
//

#ifndef EXEC_ENGINE_COMPARISON_TREE_API_H
#define EXEC_ENGINE_COMPARISON_TREE_API_H

#endif //EXEC_ENGINE_COMPARISON_TREE_API_H

#include "headers.h"
#include "globals.h"
#include "ImportKernelSource.h"
#include "data_api.h"
#include "kernel_api.h"
#include "runtime_api.h"

#include <iostream> // std::cout
#include <utility> // std::pair

#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <common/runtime/Database.hpp>
#include <common/runtime/Import.hpp>
#include <common/runtime/Types.hpp>
#include <boost/graph/graphviz.hpp>


//Execute graph
// Gets a graph object as parameter and executes all the nodes of the tree in the given device.

namespace dispatcher{


    //Getting kernel objects
    enum selectionOperator {
        between,
        lesser,
        greater
    };

    kernelObject getSelection(selectionOperator option, string input,string output, size_t globalSize,
                              int* parameter){

        kernelObject ko;

        switch(option){

            case selectionOperator::between :
                                                ko.name = executionPrimitives[0];
                                                ko.arguments.push_back(input);
                                                ko.arguments.push_back(output);
                                                ko.parameters.push_back(parameter[0]);
                                                ko.parameters.push_back(parameter[1]);
                                                ko.globalSize=globalSize/vectorSize;
                                                return ko;
            case selectionOperator::lesser :
                                                ko.name = executionPrimitives[1];
                                                ko.arguments.push_back(input);
                                                ko.arguments.push_back(output);
                                                ko.parameters.push_back(parameter[0]);
                                                ko.globalSize=globalSize/vectorSize;
                                                return ko;
            case selectionOperator::greater :
                                                ko.name = executionPrimitives[2];
                                                ko.arguments.push_back(input);
                                                ko.arguments.push_back(output);
                                                ko.parameters.push_back(parameter[0]);
                                                ko.globalSize=globalSize/vectorSize;
                                                return ko;
        }
    }


    kernelObject getLogical(string input,string output, size_t globalSize){

        kernelObject ko;


        ko.name = executionPrimitives[3];
        ko.arguments.push_back(input);
        ko.arguments.push_back(output);
        ko.globalSize=globalSize/vectorSize;
        return ko;
    }

    kernelObject getMaterialize(string input,string output, size_t globalSize){

        kernelObject ko;

        ko.arguments.push_back(input);
        ko.arguments.push_back(output);
        ko.globalSize=globalSize/vectorSize*vectorSize;
        return ko;
    }

    void compileKernels(size_t dev = 0){

        cl_device_id d = device[dev][0];
        for(string kern : executionPrimitives){
            string kernelSource = "include/dispatcher/kernels/mapper/"+ kern +".cl";
            string src = readKernelFile(kernelSource);
            add_kernel(kern,d,src);
        }
    }

    int executeGraphinDevice(short dev,Graph kernelGraph,int N ){

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

            compileKernels(dev);

            vector<cl_event> relatedEvents,returnedEvents, allEvents;
            Graph::vertex_iterator  iv, iend;
            short prevTime = 0;
            for (boost::tie(iv, iend) = vertices(kernelGraph); iv != iend; ++iv){

                if(time[*iv]==prevTime){

                    size_t localSize = kernelArr[kernelGraph[*iv]].globalSize==18767?7:2;
                    cl_event te = *queueExecution(device[dev][0],
                                                  kernelArr[kernelGraph[*iv]].name,
                                                  kernelArr[kernelGraph[*iv]].arguments,
                                                  kernelArr[kernelGraph[*iv]].parameters,
                                                  kernelArr[kernelGraph[*iv]].globalSize,&relatedEvents[0],relatedEvents.size(), localSize);
                    returnedEvents.push_back(te);

                    allEvents.emplace_back(te);

                }
                else{

                    size_t localSize = kernelArr[kernelGraph[*iv]].globalSize==18767?7:2;
                    relatedEvents = returnedEvents;
                    returnedEvents.clear();
                    cl_event te =
                            *queueExecution(device[dev][0],
                                            kernelArr[kernelGraph[*iv]].name,
                                            kernelArr[kernelGraph[*iv]].arguments,
                                            kernelArr[kernelGraph[*iv]].parameters,
                                            kernelArr[kernelGraph[*iv]].globalSize,&relatedEvents[0],relatedEvents.size(),localSize);

                    returnedEvents.push_back(te);
                    prevTime = time[*iv];
                    allEvents.emplace_back(te);
                }
            }

            executeGraph(device[dev][0]);
            for(cl_event s : allEvents){

                GraphExecutionTime += dispatcher::getDuration(s);
            }

            return 1;
        }

};
