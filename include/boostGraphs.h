#include "headers.h"
#include "globals.h"

#include <iostream> // std::cout
#include <utility> // std::pair
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/range/irange.hpp>
#include <boost/graph/topological_sort.hpp>

using namespace boost;
using namespace std;

class kernelBundle{

public:
    string name;
};

class dataBundle{

public:
    string name;
};


void kernelCompile(){

    typedef adjacency_list<vecS, vecS, bidirectionalS,kernelBundle> Graph;
    typedef std::pair<kernelBundle,kernelBundle> dataEdge;

    kernelBundle d1,d2,d3,d4,d5,d6,source1,source2,source3;
    vector<kernelBundle> kernelsToCompile;


    source1.name = "Source data";
    source2.name = "Source data";
    source3.name = "Source data";
    d1.name = "selection data 1";
    d2.name = "selection data 2";
    d3.name = "selection data 3";
    d4.name = "logical result";
    d5.name = "final logical result";
    d6.name = "Sink data";
    kernelsToCompile.push_back(source1);
    kernelsToCompile.push_back(source2);
    kernelsToCompile.push_back(source3);
    kernelsToCompile.push_back(d1);
    kernelsToCompile.push_back(d2);
    kernelsToCompile.push_back(d3);
    kernelsToCompile.push_back(d4);
    kernelsToCompile.push_back(d5);
    kernelsToCompile.push_back(d6);


    typedef pair<kernelBundle,kernelBundle> dataEdges;

    Graph g;
    size_t N = 9;
    add_edge(add_vertex(kernelsToCompile[0],g),add_vertex(kernelsToCompile[3],g),g);
    add_edge(add_vertex(kernelsToCompile[1],g),add_vertex(kernelsToCompile[4],g),g);
    add_edge(add_vertex(kernelsToCompile[2],g),add_vertex(kernelsToCompile[5],g),g);
    add_edge(add_vertex(kernelsToCompile[3],g),add_vertex(kernelsToCompile[6],g),g);
    add_edge(add_vertex(kernelsToCompile[4],g),add_vertex(kernelsToCompile[6],g),g);
    add_edge(add_vertex(kernelsToCompile[5],g),add_vertex(kernelsToCompile[7],g),g);
    add_edge(add_vertex(kernelsToCompile[7],g),add_vertex(kernelsToCompile[8],g),g);


    typedef graph_traits<Graph>::vertex_descriptor Vertex;

    typedef std::list<Vertex> MakeOrder;
    MakeOrder make_order;
    boost::topological_sort(g, std::front_inserter(make_order));

    std::cout << "make ordering: ";
    for (MakeOrder::reverse_iterator i = make_order.rbegin();
         i != make_order.rend(); ++i)
        std::cout << g[*i].name << "\n";
    std::cout << std::endl;


    //Making parallel order
    std::vector<int> time(N, 0);
    for (MakeOrder::iterator i = make_order.begin(); i != make_order.end(); ++i) {
        if (in_degree (*i, g) > 0) {
            Graph::in_edge_iterator j, j_end;
            int maxdist = 0;
            for (boost::tie(j, j_end) = in_edges(*i, g); j != j_end; ++j)
                maxdist = std::max(time[source(*j, g)], maxdist);
            time[*i]=maxdist+1;
        }
    }

    std::cout << "parallel make ordering, " << std::endl
              << "  vertices with same group number" << std::endl
              << "  can be made in parallel" << std::endl << std::endl;

    Graph::vertex_iterator  iv, iend;
    for (boost::tie(iv, iend) = vertices(g); iv != iend; ++iv)
        std::cout << "time_slot[" << g[*iv].name << "] = "<< time[*iv] << std::endl;


}

void fileCompile(){

    enum files_e { dax_h, yow_h, boz_h, zow_h, foo_cpp,
        foo_o, bar_cpp, bar_o, libfoobar_a,
        zig_cpp, zig_o, zag_cpp, zag_o,
        libzigzag_a, killerapp, N };

    cout<<"N : "<<N<<endl;

    const char* name[] = { "dax.h", "yow.h", "boz.h", "zow.h", "foo.cpp",
                           "foo.o", "bar.cpp", "bar.o", "libfoobar.a",
                           "zig.cpp", "zig.o", "zag.cpp", "zag.o",
                           "libzigzag.a", "killerapp" };

    typedef std::pair<int, int> Edge;
    Edge used_by[] = {
            Edge(dax_h, foo_cpp), Edge(dax_h, bar_cpp), Edge(dax_h, yow_h),
            Edge(yow_h, bar_cpp), Edge(yow_h, zag_cpp),
            Edge(boz_h, bar_cpp), Edge(boz_h, zig_cpp), Edge(boz_h, zag_cpp),
            Edge(zow_h, foo_cpp),
            Edge(foo_cpp, foo_o),
            Edge(foo_o, libfoobar_a),
            Edge(bar_cpp, bar_o),
            Edge(bar_o, libfoobar_a),
            Edge(libfoobar_a, libzigzag_a),
            Edge(zig_cpp, zig_o),
            Edge(zig_o, libzigzag_a),
            Edge(zag_cpp, zag_o),
            Edge(zag_o, libzigzag_a),
            Edge(libzigzag_a, killerapp)
    };

    using namespace boost;
    typedef adjacency_list<vecS, vecS, bidirectionalS,
            property<vertex_color_t, default_color_type>
    > Graph;
    Graph g(used_by, used_by + sizeof(used_by) / sizeof(Edge), N);
    typedef graph_traits<Graph>::vertex_descriptor Vertex;

    typedef std::list<Vertex> MakeOrder;
    MakeOrder make_order;
        boost::topological_sort(g, std::front_inserter(make_order));

    std::cout << "make ordering: ";
    for (MakeOrder::iterator i = make_order.begin();
         i != make_order.end(); ++i)
        std::cout << name[*i] << " ";
    std::cout << std::endl;

    std::vector<int> time(N, 0);
    for (MakeOrder::iterator i = make_order.begin(); i != make_order.end(); ++i) {
        if (in_degree (*i, g) > 0) {
            Graph::in_edge_iterator j, j_end;
            int maxdist = 0;
            for (boost::tie(j, j_end) = in_edges(*i, g); j != j_end; ++j)
                maxdist = std::max(time[source(*j, g)], maxdist);
            time[*i]=maxdist+1;
        }
    }

    std::cout << "parallel make ordering, " << std::endl
              << "  vertices with same group number" << std::endl
              << "  can be made in parallel" << std::endl << std::endl;
    //tie i;
    Graph::vertex_iterator  iv, iend;
    for (boost::tie(iv, iend) = vertices(g); iv != iend; ++iv)
        std::cout << "time_slot[" << name[*iv] << "] = " << time[*iv] << std::endl;

}