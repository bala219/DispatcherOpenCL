cmake_minimum_required(VERSION 3.4)
project(openCLDispatcher)

set(CMAKE_CXX_STANDARD 14)

add_executable(openCLDispatcher

        main.cpp
        Query6.h

include/headers.h
include/globals.h
include/Environment.h
include/data_api.h
include/kernel_api.h
include/runtime_api.h
include/kernel_files/selection_block.h
include/kernel_files/logical_block.h
include/kernel_files/selection_coalesced.h
include/base_kernel.h
#include/DAG_query.h

#        aggregate_variant.h
#        "aggregate_variant test.h"
        grouped_aggregate_dual.h test_local.h include/Importkernel.h
        BitonicSorting.h prefixSumCompute.h cuttlefish.h
#        operator/ordered_aggregation.h
        hashing_test.h
        operator/operator.h
        operator/selection.h
        include/pipeline.h

        #include/Execution/Environment.h
        testBandits.h
        include/Bandits/bandit.h
        include/Bandits/agents.h
        include/Bandits/policy.h
        include/Bandits/VowpalEnvironment.h
        include/Bandits/BanditEnvironment.h
        include/kernelGraphs.h
        include/boostGraphs.h
        include/kernelTestGraph.h
        include/pipeline/pipeline_api.h
        include/primitives/primitiveKernel.h
        include/pipeline/nodeInfo.h damon.h
        FluidiCL.h
        operator/OA_graph.h operator/AggregatebenchMark.h

# including TPCH Import files into Cmake
        include/common/algebra/Operators.hpp
        include/common/algebra/Operators.cpp
        include/common/algebra/Pipeline.hpp
        include/common/algebra/Pipeline.cpp
        include/common/algebra/Types.hpp
        include/common/algebra/Types.cpp

        include/common/runtime/Import.hpp
        include/common/runtime/Import.cpp
        include/common/runtime/Database.hpp
        include/common/runtime/Database.cpp
        include/common/runtime/Barrier.hpp
        include/common/runtime/Concurrency.hpp
        include/common/runtime/Concurrency.cpp
        include/common/runtime/Hash.hpp
        include/common/runtime/Hashmap.hpp
        include/common/runtime/Hashmap.cpp
        include/common/runtime/Memory.hpp
        include/common/runtime/MemoryPool.hpp
        include/common/runtime/MemoryPool.cpp
        include/common/runtime/Mmap.hpp
        include/common/runtime/PartitionedDeque.hpp
        include/common/runtime/Profile.cpp
        include/common/runtime/Query.hpp
        include/common/runtime/SIMD.hpp
        include/common/runtime/String.hpp
        include/common/runtime/String.cpp
        include/common/runtime/Stack.hpp
        include/common/runtime/Types.hpp
        include/common/runtime/Types.cpp
        include/common/runtime/Util.hpp

        include/common/Compat.hpp
        include/common/defs.hpp
        include/common/Util.hpp

        include/graph_api.h componentModel.h operator/evaluate_atomics.h operator/aggregateTests.h)


FIND_PACKAGE(Boost 1.58 COMPONENTS system chrono)
FIND_PACKAGE(OpenCL)

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${OpenCL_INCLUDE_DIR} )

LINK_DIRECTORIES(/usr/local/cuda-8.0/include)
TARGET_LINK_LIBRARIES( openCLDispatcher LINK_PUBLIC ${Boost_LIBRARIES} ${OpenCL_LIBRARIES})
