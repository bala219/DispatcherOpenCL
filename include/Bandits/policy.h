//
// Created by gurumurt on 10/15/18.
//

#ifndef OPENCLDISPATCHER_POLICY_H
#define OPENCLDISPATCHER_POLICY_H

#endif //OPENCLDISPATCHER_POLICY_H

#include <iostream>
#include <bitset>
#include <boost/chrono.hpp>
#include <iomanip>
#include <map>
#include <vector>
#include <list>
#include "agents.h"
using namespace std;

/// Policy prescribes the action to be taken based on the memory of an agent
class policy{

public:

    int action;

    virtual string getName()=0;
    virtual int choose()=0;
};


///The Epsilon-Greedy policy will choose a random action with probability epsilon and take the best apparent
/// approach with probability 1-epsilon. If multiple actions are tied for best choice, then a random action
/// from that subset is selected.
class EpsilonGreedy : public policy{

public:
    double epsilon;

    EpsilonGreedy(){
        this->epsilon = 0.01;
    }

    EpsilonGreedy(double eps){
        this->epsilon = eps;
    }

    string getName(){
        return "\u03B5-greedy (\u03B5={"+ to_string(epsilon) +"})";
    }

    int choose(agent a){

        if(rand() < epsilon)
            return rand();
        else{
            int action=-1;
            for(int i =0;i<a.k;i++){
                if(a.valueEstimates[i]>action){
                    action = i;
                }
            }

            //check
            short *check = (short*)calloc(a.k,short);
            short checkSize=0;
            for(short i=0;i<a.k;i++){
                if(a.valueEstimates[i]==action)
                    check[checkSize++] = i;
            }
            if(checkSize==0)
                return action;
            else{
                return check[rand() % checkSize];
            }

        }
    }

};

class GreedyPolicy : public EpsilonGreedy{

public:
    GreedyPolicy(){
        EpsilonGreedy(0);
    }

    string getname(){
        return "Greedy";
    }
};

class RandomPolicy : public EpsilonGreedy{

public:
    RandomPolicy(){
        EpsilonGreedy(1);
    }

    string getname(){
        return "Random";
    }
};

//class UCBPolicy : public policy{
//
//public:
//
//    int exploration;
//
//    string getName(){
//        return "UCB";
//    }
//
//    int choose(agent a){
//        /*
//         * exploration = np.log(agent.t+1) / agent.action_attempts
//        exploration[np.isnan(exploration)] = 0
//        exploration = np.power(exploration, 1/self.c)
//
//        q = agent.value_estimates + exploration
//        action = np.argmax(q)
//        check = np.where(q == action)[0]
//        if len(check) == 0:
//            return action
//        else:
//            return np.random.choice(check)
//
//         */
//    }
//};
//
//
//class SoftMaxPolicy : public policy{
//
//    string getName(){
//        return "SM";
//    }
//
//    int choose(agent a){
//
//        /*
//         * a = agent.value_estimates
//        pi = np.exp(a) / np.sum(np.exp(a))
//        cdf = np.cumsum(pi)
//        s = np.random.random()
//        return np.where(s < cdf)[0][0]
//         */
//    }
//};