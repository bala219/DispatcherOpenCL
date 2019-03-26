//
// Created by gurumurt on 11/5/18.
//

#include "benchmarks/tpch/Queries.hpp"
#include "common/runtime/Types.hpp"
#include "tbb/tbb.h"
#include "vectorwise/Operations.hpp"
#include "vectorwise/Operators.hpp"
#include "vectorwise/Primitives.hpp"
#include "vectorwise/QueryBuilder.hpp"
#include "vectorwise/VectorAllocator.hpp"
#include <iostream>

using namespace runtime;
using namespace std;

unique_ptr<QSimpleBuilder::QSimple> QSimpleBuilder::getQuery() {

    using namespace vectorwise;

    auto res = make_unique<QSimple>();
    auto& consts = *res;
    enum{ sel_a, sel_b, results};

    auto lineitem = Scan("lineitem");

    Select(Expression()
        .addOp(conf.sel_less_int32_t_col_int32_t_val(),
               Buffer(sel_a, sizeof(pos_t)),
               Column(lineitem,"l_shipdate"),
               Value(&consts.c1)
        )
        .addOp(conf.sel_less_int32_t_col_int32_t_val(),
               Buffer(sel_b, sizeof(pos_t)),
               Column(lineitem,"l_quantity"),
               Value(&consts.c3)
        )
    );

    res->rootOp = popOperator();

    return res;
}

Relation qSimple_vectorwise(Database& db, size_t nrThreads, size_t vectorSize){

    using namespace vectorwise;



}