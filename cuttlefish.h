//
// Created by gurumurt on 7/13/18.
//

#ifndef OPENCLDISPATCHER_CUTTLEFISH_H
#define OPENCLDISPATCHER_CUTTLEFISH_H

#endif //OPENCLDISPATCHER_CUTTLEFISH_H

/*
 * Cuttlefish: A adaptive placement primitive that uses reinforcement learning strategy for selecting variant of a operator.
 *             It uses thompson sampling to select the fastest variant and executes the operator.
 *
 *
 * Three functions available within the tuner
 * Choose: selects a variant
 * Execute: Executes the given variant
 * Observe: Records the performance metrics we want.
 *
 * Tuner(reward) --chooses--> operator --execute--> metrics --observe--> reward
 *
 * Tuner(operator.metric){
 *
 *      operator.code = choose(variant)
 *      response_time = execute(operator)
 *      operator.metric = observe(response_time)
 * }
 *
 */

#include "include/headers.h"
#include "include/Environment.h"
#include "include/data_api.h"
#include "include/kernel_api.h"
#include "include/runtime_api.h"
#include "include/Importkernel.h"
#include <math.h>
#include <chrono>


/*
 * Gets operator and variant information.
 * returns Execution time
 */