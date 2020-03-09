//
// Created by gurumurt on 10/15/18.
//

#ifndef OPENCLDISPATCHER_TESTBANDITS_H
#define OPENCLDISPATCHER_TESTBANDITS_H

#endif //OPENCLDISPATCHER_TESTBANDITS_H

#include "include/Bandits/bandit.h"
#include "include/Bandits/agents.h"
#include "include/Bandits/policy.h"
#include "include/Bandits/BanditEnvironment.h"

int no_of_arms = 6;
int trails = 10;
int experiments  = 1;


//Port for standardStochasticBandits function of Gabriel
void testBandit(){

    GaussianBandit gb(no_of_arms);
    EpsilonGreedy eGreedy;

    agent a[1];
    a[0]=agent(gb,eGreedy, 5);

    BanditEnvironment be(gb,a,1,"Testing sample");
    int counter =0;
    be.run(10,8);
}