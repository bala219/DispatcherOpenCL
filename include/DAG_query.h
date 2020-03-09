//
// Created by gurumurt on 4/11/18.
//

#ifndef OPENCLDISPATCHER_DAG_QUERY_H
#define OPENCLDISPATCHER_DAG_QUERY_H

#endif //OPENCLDISPATCHER_DAG_QUERY_H

#include "headers.h"
#include "globals.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

//struct node {
//
//    string kernelName;
//    short noOfInput;
//};
//
//node source, sink;
//
//multimap<node*,node*> edges;
//
//void printGraph(){
//
//    for(pair<node*,node*> edge : edges){
//        cout<<edge.first->kernelName<<"----->"<<edge.second->kernelName<<endl;
//    }
//}
//
//void testDAG(){
//
//    node selectA,selectB,andAB;
//
//    source.kernelName = "Data source";
//    sink.kernelName = "Data sink";
//
//    selectA.kernelName = "Selection";
//    selectB.kernelName = "Selection";
//    andAB.kernelName = "LogicalAND";
//
//    edges.insert(make_pair(&source,&selectA));
//    edges.insert(make_pair(&source,&selectB));
//    edges.insert(make_pair(&selectA,&andAB));
//    edges.insert(make_pair(&selectB,&andAB));
//    edges.insert(make_pair(&andAB,&sink));
//    printGraph();
//}

using namespace boost;

//Name of vertices
enum{
    sources,
    sinks,
    selection,
    logical
};

void testBoostGraph(){

    typedef adjacency_list<vecS,vecS,bidirectionalS> graph;
    int num_vertices = 5;

    typedef std::pair<int, int> edge;
    edge edge_array[] =
            {
                    edge(sources,selection),
                    edge(sources,selection),
                    edge(selection,logical),
                    edge(selection,logical),
                    edge(logical,sinks)
            };
    int num_edges = sizeof(edge_array)/sizeof(edge_array[0]);

    graph g(num_vertices);

    for(int i=0; i<num_edges;i++)
        add_edge(edge_array[i].first,edge_array[i].second,g);
}