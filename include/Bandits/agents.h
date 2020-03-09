//
// Created by gurumurt on 10/15/18.
//

#ifndef OPENCLDISPATCHER_AGENTS_H
#define OPENCLDISPATCHER_AGENTS_H

#endif //OPENCLDISPATCHER_AGENTS_H
#include <iostream>
#include <math.h>
#include "bandit.h"
#include "policy.h"

class agent{

public:
    //Policy policy
    int k,t;
    double gamma;
    short prior;
    int lastAction;
    double *valueEstimates, *actionAttempts;
    policy p;
    MultiArmBandit mab;

    agent(){

        this->valueEstimates = (double*)calloc(k,sizeof(double));
        this->actionAttempts = (double*)calloc(k,sizeof(double));
        for(short i=0;i<k;i++){

            valueEstimates[i] = 1.0;
        }

        this->t = 0;
        this->lastAction = -1;
    }

    agent(MultiArmBandit bandit, policy p, short prior =0, int gamma= nullptr)//bandit, policy
    {
        this->mab = bandit;
        this->p = p;
        this->k = mab.k;
        this->prior = prior;
        this->gamma = gamma;
    }

    void reset(){

        agent();
    }

    double choose(){
        int action = p.choose();
        lastAction = action;
        return action;
    }

    void observe(double reward){

        actionAttempts[lastAction] +=1;

        double g;
        if(gamma==0){

            g = 1 /actionAttempts[lastAction];
        }
        else
            g = gamma;
        double q = valueEstimates[lastAction];
        valueEstimates[lastAction] +=g*(reward - q);
        t += 1;
    }

    double* getValueEstimate(){
        return valueEstimates;
    }
};

class GradientAgent : public agent{


public:
    int alpha;
    bool baseline;
    double avgReward;

    GradientAgent(int alpha=0,bool baseline = true){

        this->alpha = alpha;
        this->baseline = baseline;
        avgReward = 0;
    }

    void observe(double reward){
        actionAttempts[lastAction]+=1;
        if(baseline){
            double diff = reward - avgReward;
            double actionAttemptSum=0;
            double valueEstimateSum=0;

            for(short i =0;i<k;i++){
                actionAttemptSum += actionAttempts[i];
                valueEstimateSum += exp(valueEstimates[i]);
            }

            avgReward +=1/(actionAttemptSum*diff);

            double pi[k];
            for(int i=0;i<k;i++){
                pi[i] = exp(valueEstimates[i])/valueEstimateSum;
            }

            double lastValueEstimate = valueEstimates[lastAction];
            lastValueEstimate += alpha*(reward - avgReward)*(1-pi[lastAction]);

            double diffValue = alpha*(reward - avgReward);
            for(int i=0;i<k;i++){
                valueEstimates[i] -= diffValue*pi[i];
            }
            valueEstimates[lastAction] = lastValueEstimate;
            t+=1;
        }
    }

    void reset(){
        agent::reset();
        avgReward = 0;
    }
};