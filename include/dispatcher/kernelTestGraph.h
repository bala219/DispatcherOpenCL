//
// Created by gurumurt on 11/6/18.
//

#ifndef EXEC_ENGINE_COMPARISON_KERNELTESTGRAPH_H
#define EXEC_ENGINE_COMPARISON_KERNELTESTGRAPH_H

#include "headers.h"
#include "globals.h"
#include "ImportKernelSource.h"
#include "data_api.h"
#include "kernel_api.h"
#include "runtime_api.h"
#include "graph_api.h"

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


namespace dispatcher{

    using namespace std;
    using namespace boost;

    static runtime::Database tpch;

    size_t sizes = 1024;



    void initializedataBuffers(){

        runtime::importTPCH(std::string(DATADIR)+"/tpch/sf1/",tpch);

        int* shipDate;
        int* quantity;
        int* discount;
        int* extended;

        auto s1 = std::chrono::high_resolution_clock::now();
        shipDate = tpch["lineitem"]["l_shipdate"].data<int>();
        quantity = tpch["lineitem"]["l_quantity"].data<int>();
        discount = tpch["lineitem"]["l_discount"].data<int>();
        extended = tpch["lineitem"]["l_extendedprice"].data<int>();
        sizes = tpch["lineitem"]["l_shipdate"].data_.size();
        auto s2 = std::chrono::high_resolution_clock::now();

        cout<<"Size : "<<sizes<<endl;

        int *b = (int*)calloc((int)sizes, sizeof(uint));

        cout<<"ship[0] : "<<shipDate[0]<<endl;

        add_data("shipdate",shipDate,device[0][0],sizes);
        add_data("extended",extended,device[0][0],sizes);
        add_data("selection1Result",b,device[0][0],sizes/vectorSize );
        add_data("quantity",quantity,device[0][0],sizes);
        add_data("selection2Result",b,device[0][0],sizes/vectorSize );
        add_data("discount",quantity,device[0][0],sizes);
        add_data("selection3Result",b,device[0][0],sizes/vectorSize );
        add_data("logicalResult",b,device[0][0],sizes/vectorSize );
        add_data("logical2Result",b,device[0][0],sizes/vectorSize );
        add_data("materialize1Result",b,device[0][0],sizes/vectorSize );
        add_data("materialize2Result",b,device[0][0],sizes/vectorSize );
        add_data("arithmeticResult",b,device[0][0],sizes);
        add_data("aggregateResult",b,device[0][0],1);
    }

    void initializedataBuffersQ1(){

        runtime::importTPCH(std::string(DATADIR)+"/tpch/sf1/",tpch);

        int* shipDate;
        int* quantity;
        int* discount;
        int* extended;
        int* lineStatus;
        int* returnflag;


        auto s1 = std::chrono::high_resolution_clock::now();
        shipDate = tpch["lineitem"]["l_shipdate"].data<int>();
        lineStatus = tpch["lineitem"]["l_linestatus"].data<int>();
        quantity = tpch["lineitem"]["l_quantity"].data<int>();
        discount = tpch["lineitem"]["l_discount"].data<int>();
        extended = tpch["lineitem"]["l_extendedprice"].data<int>();
        returnflag = tpch["lineitem"]["l_returnflag"].data<int>();


        sizes = tpch["lineitem"]["l_shipdate"].data_.size();
        auto s2 = std::chrono::high_resolution_clock::now();

        cout<<"Size : "<<sizes<<endl;



        int *b = (int*)calloc((int)sizes, sizeof(uint));

        add_data("shipdate",shipDate,device[0][0],sizes);
        add_data("linestatus",shipDate,device[0][0],sizes);
        add_data("quantity",shipDate,device[0][0],sizes);
        add_data("discount",shipDate,device[0][0],sizes);
        add_data("extended",shipDate,device[0][0],sizes);
        add_data("returngflag",shipDate,device[0][0],sizes);

        add_data("selection1Result",b,device[0][0],sizes/vectorSize );
        add_data("ExtendedPriceMaterialization",b,device[0][0],sizes);
        add_data("DiscountMaterialization",b,device[0][0],sizes);
        add_data("ReturnFlagMaterialization",b,device[0][0],sizes);
        add_data("LineStatusMaterialization",b,device[0][0],sizes);
        add_data("QuantityMaterialization",b,device[0][0],sizes);
    }

    void connectingKernelsWithBuffesQ1(){

        kernelObject ko;

        ko.name = executionPrimitives[0];
        ko.arguments.push_back("shipdate");
        ko.arguments.push_back("selection1Result");
        ko.parameters.push_back(2449354);
        ko.parameters.push_back(2449719);
        ko.globalSize = sizes/vectorSize ;
        kernelArr.push_back(ko);

        ko = kernelObject();
        ko.name = executionPrimitives[4];
        ko.arguments.push_back("quantity");
        ko.arguments.push_back("selection1Result");
        ko.arguments.push_back("QuantityMaterialization");
        ko.globalSize = sizes/(32*32);
        kernelArr.push_back(ko);

        ko = kernelObject();
        ko.name = executionPrimitives[4];
        ko.arguments.push_back("discount");
        ko.arguments.push_back("selection1Result");
        ko.arguments.push_back("DiscountMaterialization");
        ko.globalSize = sizes/(32*32);
        kernelArr.push_back(ko);

        ko = kernelObject();
        ko.name = executionPrimitives[4];
        ko.arguments.push_back("extended");
        ko.arguments.push_back("selection1Result");
        ko.arguments.push_back("ExtendedPriceMaterialization");
        ko.globalSize = sizes/(32*32);
        kernelArr.push_back(ko);

        ko = kernelObject();
        ko.name = executionPrimitives[4];
        ko.arguments.push_back("linestatus");
        ko.arguments.push_back("selection1Result");
        ko.arguments.push_back("LineStatusMaterialization");
        ko.globalSize = sizes/(32*32);
        kernelArr.push_back(ko);

        ko = kernelObject();
        ko.name = executionPrimitives[4];
        ko.arguments.push_back("returnflag");
        ko.arguments.push_back("selection1Result");
        ko.arguments.push_back("ReturnFlagMaterialization");
        ko.globalSize = sizes/(32*32);
        kernelArr.push_back(ko);
    }

    void connectingKernelsWithBuffes(){

        kernelObject ko;

        ko.name = executionPrimitives[0];
        ko.arguments.push_back("shipdate");
        ko.arguments.push_back("selection1Result");
        ko.parameters.push_back(2449354);
        ko.parameters.push_back(2449719);
        ko.globalSize = sizes/vectorSize ;
        kernelArr.push_back(ko);

        ko = kernelObject();
        ko.name = executionPrimitives[1];
        ko.arguments.push_back("quantity");
        ko.arguments.push_back("selection2Result");
        ko.parameters.push_back(2400);
        ko.globalSize = sizes/vectorSize ;
        kernelArr.push_back(ko);

        ko = kernelObject();
        ko.name = executionPrimitives[0];
        ko.arguments.push_back("discount");
        ko.arguments.push_back("selection3Result");
        ko.parameters.push_back(500);
        ko.parameters.push_back(700);
        ko.globalSize = sizes/vectorSize ;
        kernelArr.push_back(ko);


        ko = kernelObject();
        ko.name = executionPrimitives[3];
        ko.arguments.push_back("selection1Result");
        ko.arguments.push_back("selection2Result");
        ko.arguments.push_back("logicalResult");
        ko.globalSize = sizes/(vectorSize *vectorSize );
        kernelArr.push_back(ko);

        ko = kernelObject();
        ko.name = executionPrimitives[3];
        ko.arguments.push_back("logicalResult");
        ko.arguments.push_back("selection3Result");
        ko.arguments.push_back("logical2Result");
        ko.globalSize = sizes/(vectorSize *vectorSize );
        kernelArr.push_back(ko);

        ko = kernelObject();
        ko.name = executionPrimitives[4];
        ko.arguments.push_back("discount");
        ko.arguments.push_back("logical2Result");
        ko.arguments.push_back("materialize1Result");
        ko.globalSize = sizes/(32*32);
        kernelArr.push_back(ko);

        ko = kernelObject();
        ko.name = executionPrimitives[4];
        ko.arguments.push_back("extended");
        ko.arguments.push_back("logical2Result");
        ko.arguments.push_back("materialize2Result");
        ko.globalSize = sizes/(32*32);
        kernelArr.push_back(ko);

        ko = kernelObject();
        ko.name = executionPrimitives[5];
        ko.arguments.push_back("materialize1Result");
        ko.arguments.push_back("materialize2Result");
        ko.arguments.push_back("arithmeticResult");
        ko.globalSize = sizes/(vectorSize *vectorSize );
        kernelArr.push_back(ko);

        ko = kernelObject();
        ko.name = executionPrimitives[6];
        ko.arguments.push_back("arithmeticResult");
        ko.arguments.push_back("aggregateResult");
        ko.globalSize = sizes/(vectorSize *vectorSize );
        kernelArr.push_back(ko);
    }

    unsigned int q6Dispatcher(){

        initializedataBuffers();
        connectingKernelsWithBuffes();



        Graph kernelGraph;

        typedef pair<size_t,size_t> kernelEdges;

        typedef graph_traits<Graph>::vertex_descriptor kernelVertex;

        kernelVertex select1,select2,select3,logical1,logical2,material,material2,ari,aggre;

        size_t N = 9;

        select1 = add_vertex(0,kernelGraph);
        select2 = add_vertex(1,kernelGraph);
        select3 = add_vertex(2,kernelGraph);
        logical1 = add_vertex(3,kernelGraph);
        logical2 = add_vertex(4,kernelGraph);
        material = add_vertex(5,kernelGraph);
        material2 = add_vertex(6,kernelGraph);
        ari = add_vertex(7,kernelGraph);
        aggre = add_vertex(8,kernelGraph);

        add_edge(select1,logical1,kernelGraph);
        add_edge(select2,logical1,kernelGraph);
        add_edge(select3,logical2,kernelGraph);
        add_edge(logical1,logical2,kernelGraph);
        add_edge(logical2,material,kernelGraph);
        add_edge(logical2,material2,kernelGraph);
        add_edge(material,ari,kernelGraph);
        add_edge(material2,ari,kernelGraph);
        add_edge(ari,aggre,kernelGraph);


        executeGraphinDevice(0,kernelGraph,N);

        cout<<"OpenCL execution time "<<GraphExecutionTime<<endl;

        unsigned int *res = get_data("aggregateResult",device[0][0],1);

        return *res;

    }

    unsigned int q1Dispatcher(){

        initializedataBuffersQ1();
        connectingKernelsWithBuffesQ1();

        Graph kernelGraph;

        typedef pair<size_t,size_t> kernelEdges;

        typedef graph_traits<Graph>::vertex_descriptor kernelVertex;

        kernelVertex select1,materialize1,materialize2,materialize3,materialize4;

        size_t N = 9;

        select1 = add_vertex(0,kernelGraph);
        materialize1 = add_vertex(1,kernelGraph);
        materialize2 = add_vertex(2,kernelGraph);
        materialize3 = add_vertex(3,kernelGraph);
        materialize4 = add_vertex(4,kernelGraph);

        add_edge(select1,materialize1,kernelGraph);
        add_edge(select1,materialize2,kernelGraph);
        add_edge(select1,materialize3,kernelGraph);
        add_edge(select1,materialize4,kernelGraph);


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

        compileKernels();

        vector<cl_event> relatedEvents,returnedEvents, allEvents;
        Graph::vertex_iterator  iv, iend;
        short prevTime = 0;
        for (boost::tie(iv, iend) = vertices(kernelGraph); iv != iend; ++iv){

//            cout<<"Time - "<<time[*iv]<<" - "<<kernelArr[kernelGraph[*iv]].name<<endl;

            if(time[*iv]==prevTime){

                cl_event te = *queueExecution(device[0][0],
                                              kernelArr[kernelGraph[*iv]].name,
                                              kernelArr[kernelGraph[*iv]].arguments,
                                              kernelArr[kernelGraph[*iv]].parameters,
                                              kernelArr[kernelGraph[*iv]].globalSize,&relatedEvents[0],relatedEvents.size(), 1);
                returnedEvents.push_back(te);

                allEvents.emplace_back(te);

            }
            else{
                relatedEvents = returnedEvents;
                returnedEvents.clear();
                cl_event te =
                        *queueExecution(device[0][0],
                                        kernelArr[kernelGraph[*iv]].name,
                                        kernelArr[kernelGraph[*iv]].arguments,
                                        kernelArr[kernelGraph[*iv]].parameters,
                                        kernelArr[kernelGraph[*iv]].globalSize,&relatedEvents[0],relatedEvents.size(),1);

                returnedEvents.push_back(te);
                prevTime = time[*iv];
//                cout<< "\n"<<kernelArr[kernelGraph[*iv]].name;
                allEvents.emplace_back(te);
            }
        }

        executeGraph(device[0][0]);
        double executionTime = 0;
        for(cl_event s : allEvents){

            executionTime += dispatcher::getDuration(s);
        }

        cout<<"OpenCL execution time "<<executionTime<<endl<<"all events : "<<allEvents.size()<<endl;

        unsigned int *res = get_data("aggregateResult",device[0][0],1);

        return *res;

    }
}

#endif //EXEC_ENGINE_COMPARISON_KERNELTESTGRAPH_H
