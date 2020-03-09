//
// Created by gurumurt on 10/15/18.
//

#ifndef OPENCLDISPATCHER_BANDIT_H
#define OPENCLDISPATCHER_BANDIT_H

#endif //OPENCLDISPATCHER_BANDIT_H

#include <iostream>
#include <math.h>
#include <random>
#include "../../include/headers.h"


struct pullReturn{

    bool optimal;
    double CurrentReward;
};

class MultiArmBandit{

public:

    int k;
    double* actionValues;
    int optimal;
    double reward;

    MultiArmBandit(int k){
        this->k = k;
        actionValues = (double*)calloc(k,sizeof(double));
        optimal = 0;
    }

    MultiArmBandit(){
        MultiArmBandit(1);
    }

    void reset(){
        actionValues = (double*)calloc(k,sizeof(double));
        optimal =0;
    }

    pullReturn pull(int action){

    }

};

class GaussianBandit : public MultiArmBandit{

public:
    int mu, sigma;

    GaussianBandit(int k, int mu=1, int sigma=1) : MultiArmBandit(k){

        this->mu = mu;
        this->sigma = sigma;
    }

    void reset(){

        MultiArmBandit::reset();

        std::default_random_engine generator;
        std::normal_distribution<double>nd(mu,sigma);

        optimal = 0;
        double largeValue = -1000000;
        for(short i=0; i<k;i++){

            actionValues[i] = nd(generator);
            if(largeValue < actionValues[i]){
                largeValue = actionValues[i];
                optimal = i;
            }
        }
    }

    pullReturn pull(int action){

        double executionTime; //=executeVariant(action);
        this->reward = 1.0/executionTime;
        pullReturn pr;
        pr.optimal = (action ==optimal);
        return pr;
    }

};

/*
 *     The Binomial distribution models the probability of an event occurring with
    p probability k times over N trials i.e. get heads on a p-coin k times on
    N flips.
    In the bandit scenario, this can be used to approximate a discrete user
    rating or "strength" of response to a single event.

class BinomialBandit: public MultiArmBandit{

public:

    int n,p,t;
    double model;
    long best;
    unsigned int *samples;
    static int cursor;

    BinomialBandit(){
        best = 100000000;
        samples = (unsigned int*) calloc(100,sizeof(unsigned int));
        cursor = 0;
    }

    BinomialBandit(int n, int p, int t) : MultiArmBandit(){

        this->n = n;
        this->p = p;
        this->t = t;
        //model = binomial result
    }

    void reset(){

        best = 100000000;
        actionValues = 0;
    }

    void pull(int action){

        //call the variant and execute it
        //reward is again the time for execution
        int recievedReward;
        reward = 0;
        if(recievedReward < best){
            best = recievedReward;
            reward =1;
        }
        else if(recievedReward < best +(0.01*best))
            reward = 1;

        //return (reward, action == optimal)
    }

    unsigned int sample(){
        if(samples[cursor] ==0)
            return (unsigned int) rand();
        else{
            return samples[cursor++];
        }
    }
};

class BernoulliBandit : public BinomialBandit{

public:
    BernoulliBandit(int k,int t){
        this->n = 1;
        this->k = k;
        this->p = NULL;
        this->t = t;
    }
};
 */