//
// Created by gurumurt on 10/16/18.
//

#ifndef OPENCLDISPATCHER_BANDITENVIRONMENT_H
#define OPENCLDISPATCHER_BANDITENVIRONMENT_H

#endif //OPENCLDISPATCHER_BANDITENVIRONMENT_H

#include "bandit.h"
#include "policy.h"

struct ScoreOptimal{

    double score, optimal;
};

class BanditEnvironment{

public:
    MultiArmBandit bandit;
    agent* BanditAgent;
    short agentSize;
    string label;
    unsigned int  **scores, **optimal;
    unsigned int actionRecord;

    BanditEnvironment(MultiArmBandit bandit,agent* BanditAgent, short noOfAgents, string label){

        this->bandit = bandit;
        this->BanditAgent = BanditAgent;
        this->agentSize = noOfAgents;
        this->label = label;
    }

    void reset(){

        for(short i=0;i<agentSize;i++)
                BanditAgent[i].reset();
        bandit.reset();
    }

    void run(int trail = 100, int experiment = 4){

        scores = (unsigned int**) calloc(trail*agentSize,sizeof(unsigned int));
        optimal = (unsigned int**) calloc(trail*agentSize,sizeof(unsigned int));

        for(int i = 0; i< experiment; i++){
            reset();
            for(int j=0; j<trail; j++){
                for(int l=0;l<agentSize;i++){
                    short action = BanditAgent[i].choose();
                    pullReturn p = bandit.pull(action);
                    BanditAgent->observe(p.CurrentReward);
                    scores[j][l] += p.CurrentReward;
                    if(p.optimal)
                        optimal[j][l] +=1;
                }
            }
        }

    }
};