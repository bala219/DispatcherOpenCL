//
// Created by gurumurt on 11/2/18.
//

#ifndef OPENCLDISPATCHER_PIPELINE_API_H
#define OPENCLDISPATCHER_PIPELINE_API_H

#endif //OPENCLDISPATCHER_PIPELINE_API_H

#include "../headers.h"
#include "../globals.h"
#include "../../include/Importkernel.h"
#include "../data_api.h"
#include "../kernel_api.h"
#include "../runtime_api.h"

#include <iostream> // std::cout
#include <utility> // std::pair

using namespace boost;
using namespace std;



void executeGraph(Graph &kernelGraph){

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

    cout<<"maxdist : "<<maxdist+2<<endl;

    compileKernels();

    vector<cl_event> relatedEvents,returnedEvents;
    Graph::vertex_iterator  iv, iend;
    short prevTime = 0;
    for (boost::tie(iv, iend) = vertices(kernelGraph); iv != iend; ++iv){

        if(time[*iv]==prevTime){
            returnedEvents.push_back(
                    *queueExecution(device[0][0],
                                    kernelArr[kernelGraph[*iv]].name,
                                    kernelArr[kernelGraph[*iv]].arguments,
                                    kernelArr[kernelGraph[*iv]].parameters,
                                    kernelArr[kernelGraph[*iv]].globalSize,&relatedEvents[0],relatedEvents.size(), 1));
            cout<< kernelArr[kernelGraph[*iv]].name <<"\t";
        }
        else{
            relatedEvents = returnedEvents;
            returnedEvents.clear();
            returnedEvents.push_back(
                    *queueExecution(device[0][0],
                                    kernelArr[kernelGraph[*iv]].name,
                                    kernelArr[kernelGraph[*iv]].arguments,
                                    kernelArr[kernelGraph[*iv]].parameters,
                                    kernelArr[kernelGraph[*iv]].globalSize,&relatedEvents[0],relatedEvents.size(),1));
            prevTime = time[*iv];
            cout<< "\n"<<kernelArr[kernelGraph[*iv]].name;
        }
    }

    executeGraph(device[0][0]);
}