#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <iterator>
#include <sstream>
#include <thread>
#include <unordered_set>
#include <dispatcher/Environment.h>
#include <dispatcher/kernelTestGraph.h>
//#include <test/TPCH.hpp>

#include "benchmarks/tpch/Queries.hpp"
#include "common/runtime/Import.hpp"
#include "profile.hpp"
#include "tbb/tbb.h"

using namespace runtime;

static void escape(void* p) { asm volatile("" : : "g"(p) : "memory"); }

size_t nrTuples(Database& db, std::vector<std::string> tables) {
   size_t sum = 0;
   for (auto& table : tables) sum += db[table].nrTuples;
   return sum;
}

/// Clears Linux page cache.
/// This function only works on Linux.
void clearOsCaches() {
   if (system("sync; echo 3 > /proc/sys/vm/drop_caches")) {
      throw std::runtime_error("Could not flush system caches: " +
                               std::string(std::strerror(errno)));
   }
}


static size_t vectorSize = 1024;

static size_t getThreads(){
    static size_t threads = 0;
    if(threads == 0){
        if (auto v = std::getenv("threads")) threads = atoi(v);
        else
            threads = std::thread::hardware_concurrency();
    }
    return threads;
}

static void configFromEnv(){
    if (auto v = std::getenv("vectorSize")) vectorSize = atoi(v);
    if (auto v = std::getenv("SIMDhash")) conf.useSimdHash = atoi(v);
    if (auto v = std::getenv("SIMDjoin")) conf.useSimdJoin = atoi(v);
    if (auto v = std::getenv("SIMDsel")) conf.useSimdSel = atoi(v);
    if (auto v = std::getenv("SIMDproj")) conf.useSimdProj = atoi(v);
}

static runtime::Database tpch;

using namespace std;

types::Integer* getColumnPointer(string tableName, string attributeName){

    runtime::Attribute& a = tpch[tableName][attributeName];
    cout<<a.data_.size()<<endl;
    auto columnPointer = a.data<types::Integer>();
    return columnPointer;
}


void columnTest(){

    auto l_ship = getColumnPointer("lineitem","l_shipdate");
    std::cout<<"Test : "<<l_ship[0]<<std::endl;
}

void tectorwise(){

    cout<<"Tectorwise\n";
    auto threads = getThreads();
    tbb::task_scheduler_init scheduler(threads);



    auto s1 = std::chrono::high_resolution_clock::now();
    auto result = q6_vectorwise(tpch, threads, vectorSize);
    auto s2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::nano> fp_ms = s2 - s1;
    std::chrono::duration<double, std::milli> ms = s2 - s1;

    std::cout<<"Execution Time : "<<fp_ms.count()<<"\t milli : "<<ms.count()<<std::endl;

//    auto& revenue = result["revenue"].typedAccess<types::Numeric<12, 4>>();
    int* revenue = result["revenue"].data<int>();
    std::cout<<"Revenue : "<<*revenue<<std::endl;

}

void typer(){
    cout<<"Typer\n";
    auto s1 = std::chrono::high_resolution_clock::now();
    auto result = q6_hyper(tpch);
    auto s2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::nano> fp_ms = s2 - s1;
    std::chrono::duration<double, std::milli> ms = s2 - s1;

    std::cout<<"Execution Time : "<<fp_ms.count()<<"\t milli : "<<ms.count()<<std::endl;

    int* revenue = result["revenue"].data<int>();
    std::cout<<"Revenue : "<<*revenue<<std::endl;

}

void dispatcherQ6(){


    cout<<"Dispatcher\n";
    dispatcher::setup_environment();
    auto s1 = std::chrono::high_resolution_clock::now();
    unsigned int result = dispatcher::q6Dispatcher();
    auto s2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::nano> fp_ms = s2 - s1;
    std::chrono::duration<double, std::milli> ms = s2 - s1;

    std::cout<<"Execution Time : "<<fp_ms.count()<<"\t milli : "<<ms.count()<<std::endl;
    std::cout<<"Revenue : "<<result<<std::endl;
}

void testQ6(){

//    clearOsCaches();
    tectorwise();
    dispatcherQ6();
    typer();
}

void hook(){

    importTPCH(std::string(DATADIR)+"/tpch/sf1/",tpch);
    testQ6();

//    std::cout<<"Changing the execution here"<<std::endl;
//    std::cout<<"Testing vectorized execution"<<std::endl;
//
//    importTPCH(std::string(DATADIR)+"/tpch/sf1/",tpch);
//
//    columnTest();
//
//    dispatcher::setup_environment();
//    dispatcher::print_environment();
//
//    auto threads = getThreads();
//    tbb::task_scheduler_init scheduler(threads);
//
//
//
//    auto s1 = std::chrono::high_resolution_clock::now();
//    auto result = q6_vectorwise(tpch, threads, vectorSize);
//    auto s2 = std::chrono::high_resolution_clock::now();
//
//    std::chrono::duration<double, std::nano> fp_ms = s2 - s1;
//
//    std::cout<<"Execution Time : "<<fp_ms.count()<<std::endl;
//
//    auto& revenue = result["revenue"].typedAccess<types::Numeric<12, 4>>();
//    std::cout<<"Revenue : "<<revenue[0]<<std::endl;

    //Reading data from database

    exit(0);
}

int main(int argc, char* argv[]) {


   hook();


   if (argc <= 2) {
      std::cerr
          << "Usage: ./" << argv[0]
          << "<number of repetitions> <path to tpch dir> [nrThreads = all] \n "
             " EnvVars: [vectorSize = 1024] [SIMDhash = 0] [SIMDjoin = 0] "
             "[SIMDsel = 0]";
      exit(1);
   }

   PerfEvents e;
   Database tpch;
   // load tpch data
   importTPCH(argv[2], tpch);

   // run queries
   auto repetitions = atoi(argv[1]);
   size_t nrThreads = std::thread::hardware_concurrency();
   size_t vectorSize = 1024;
   bool clearCaches = false;
   if (argc > 3) nrThreads = atoi(argv[3]);

   std::unordered_set<std::string> q = {"1h", "1v", "3h", "3v", "5h",  "5v",
                                        "6h", "6v", "9h", "9v", "18h", "18v"};

   if (auto v = std::getenv("vectorSize")) vectorSize = atoi(v);
   if (auto v = std::getenv("SIMDhash")) conf.useSimdHash = atoi(v);
   if (auto v = std::getenv("SIMDjoin")) conf.useSimdJoin = atoi(v);
   if (auto v = std::getenv("SIMDsel")) conf.useSimdSel = atoi(v);
   if (auto v = std::getenv("SIMDproj")) conf.useSimdProj = atoi(v);
   if (auto v = std::getenv("clearCaches")) clearCaches = atoi(v);
   if (auto v = std::getenv("q")) {
      using namespace std;
      istringstream iss((string(v)));
      q.clear();
      copy(istream_iterator<string>(iss), istream_iterator<string>(),
           insert_iterator<decltype(q)>(q, q.begin()));
   }

   tbb::task_scheduler_init scheduler(nrThreads);
   if (q.count("1h"))
      e.timeAndProfile("q1 hyper     ", nrTuples(tpch, {"lineitem"}),
                       [&]() {
                          if (clearCaches) clearOsCaches();
                          auto result = q1_hyper(tpch, nrThreads);
                          escape(&result);
                       },
                       repetitions);
   if (q.count("1v"))
      e.timeAndProfile("q1 vectorwise", nrTuples(tpch, {"lineitem"}),
                       [&]() {
                          if (clearCaches) clearOsCaches();
                          auto result =
                              q1_vectorwise(tpch, nrThreads, vectorSize);
                          escape(&result);
                       },
                       repetitions);
   if (q.count("3h"))
      e.timeAndProfile("q3 hyper     ",
                       nrTuples(tpch, {"customer", "orders", "lineitem"}),
                       [&]() {
                          if (clearCaches) clearOsCaches();
                          auto result = q3_hyper(tpch, nrThreads);
                          escape(&result);
                       },
                       repetitions);
   if (q.count("3v"))
      e.timeAndProfile(
          "q3 vectorwise", nrTuples(tpch, {"customer", "orders", "lineitem"}),
          [&]() {
             if (clearCaches) clearOsCaches();
             auto result = q3_vectorwise(tpch, nrThreads, vectorSize);
             escape(&result);
          },
          repetitions);
   if (q.count("5h"))
      e.timeAndProfile("q5 hyper     ",
                       nrTuples(tpch, {"supplier", "region", "nation",
                                       "customer", "orders", "lineitem"}),
                       [&]() {
                          if (clearCaches) clearOsCaches();
                          auto result = q5_hyper(tpch, nrThreads);
                          escape(&result);
                       },
                       repetitions);
   if (q.count("5v"))
      e.timeAndProfile("q5 vectorwise",
                       nrTuples(tpch, {"supplier", "region", "nation",
                                       "customer", "orders", "lineitem"}),
                       [&]() {
                          if (clearCaches) clearOsCaches();
                          auto result =
                              q5_vectorwise(tpch, nrThreads, vectorSize);
                          escape(&result);
                       },
                       repetitions);
   if (q.count("6h"))
      e.timeAndProfile("q6 hyper     ", tpch["lineitem"].nrTuples,
                       [&]() {
                          if (clearCaches) clearOsCaches();
                          auto result = q6_hyper(tpch, nrThreads);
                          escape(&result);
                       },
                       repetitions);
   if (q.count("6v"))
      e.timeAndProfile("q6 vectorwise", tpch["lineitem"].nrTuples,
                       [&]() {
                          if (clearCaches) clearOsCaches();
                          auto result =
                              q6_vectorwise(tpch, nrThreads, vectorSize);
                          escape(&result);
                       },
                       repetitions);
   if (q.count("9h"))
      e.timeAndProfile("q9 hyper     ",
                       nrTuples(tpch, {"nation", "supplier", "part", "partsupp",
                                       "lineitem", "orders"}),
                       [&]() {
                          if (clearCaches) clearOsCaches();
                          auto result = q9_hyper(tpch, nrThreads);
                          escape(&result);
                       },
                       repetitions);
   if (q.count("9v"))
      e.timeAndProfile("q9 vectorwise",
                       nrTuples(tpch, {"nation", "supplier", "part", "partsupp",
                                       "lineitem", "orders"}),
                       [&]() {
                          if (clearCaches) clearOsCaches();
                          auto result =
                              q9_vectorwise(tpch, nrThreads, vectorSize);
                          escape(&result);
                       },
                       repetitions);
   if (q.count("18h"))
      e.timeAndProfile(
          "q18 hyper     ",
          nrTuples(tpch, {"customer", "lineitem", "orders", "lineitem"}),
          [&]() {
             if (clearCaches) clearOsCaches();
             auto result = q18_hyper(tpch, nrThreads);
             escape(&result);
          },
          repetitions);
   if (q.count("18v"))
      e.timeAndProfile(
          "q18 vectorwise",
          nrTuples(tpch, {"customer", "lineitem", "orders", "lineitem"}),
          [&]() {
             if (clearCaches) clearOsCaches();
             auto result = q18_vectorwise(tpch, nrThreads, vectorSize);
             escape(&result);
          },
          repetitions);
   scheduler.terminate();
   return 0;
}
