//
// Created by gurumurt on 10/15/18.
//

#ifndef OPENCLDISPATCHER_ENVIRONMENT_H
#define OPENCLDISPATCHER_ENVIRONMENT_H

#endif //OPENCLDISPATCHER_ENVIRONMENT_H

#include "bandit.h"
#include "policy.h"

class VowpalEnvironment{

    MultiArmBandit bandit;
    agent ag;
    string label;
    unsigned int  *scores, *optimal;
    unsigned int actionRecord;

    VowpalEnvironment(){

    }

    VowpalEnvironment(MultiArmBandit mab, agent a, string label="test"){

        bandit = mab;
        ag = a;
        this->label = label;
    }

    void reset(){
        bandit.reset();
        ag.reset();
    }

    void run(int expr, int trail=10){

        double binnedContexts[11] = {0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0};

        scores = (unsigned int*) calloc(trail,sizeof(unsigned int));
        optimal = (unsigned int*) calloc(trail,sizeof(unsigned int));
        actionRecord = 0;

        unsigned int* computedCost = (unsigned int*) calloc(trail,sizeof(unsigned int));
        for(int i=0;i<expr;i++){
            reset();
            for(int j=0;j<trail;j++){
                double context; //context taken randomly from binnedcontexts
                short randIndex = rand() %11;
                context = binnedContexts[randIndex];
                int choice; //agent.predict?
                //testing only for one agent

            }
        }

        //dictionary actionRecord with all agents
        //for (experiment) { for(trails) ..

/*
        //action_record = [dict() for x in range(len(self.agents))]
                for _ in range(experiments):
            self.reset()
            for t in range(trials):
                computed_costs={} #Saves computations per trial
                context= random.choice(binned_contexts) #random.random()
                for i, agent in enumerate(self.agents):
                    choice = agent.predict("| " + str(context) )
                    if not isinstance(choice, int):
                        tally = sum(choice)
                        if not tally == 1: #TECHNICAL DEBT ALERT...!!!!!!!!! I'm rounding here one of the numbers so the sum of probabilities adds up to 1. Since these are small numbers it should not make a difference.
                            #print (t, choice)
                            for j, ch in enumerate(choice):
                                if (ch +1 - tally >=0):
                                    choice[j]=choice[j]+1-tally
                                    break
                            #print(t, choice)
                        action = np.random.choice(range(0,self.arms),p=choice)
                        max_pos = choice.index(max(choice))
                        if not isinstance(max_pos, int):
                            opt= action in max_pos
                        else:
                            opt= action == max_pos
                        probability = choice[action]
                    else:
                        opt=True
                        probability = 1
                        action = choice-1

                    if not str(round(context, 1)) in action_record[i]:
                        action_record[i][str(round(context, 1))]={}
                        action_record[i][str(round(context, 1))][str(action)]=int(1)
                    elif not str(action) in action_record[i][str(round(context, 1))]:
                        action_record[i][str(round(context, 1))][str(action)]=int(1)
                    else:
                        action_record[i][str(round(context, 1))][str(action)]=int(action_record[i][str(round(context, 1))][str(action)])+1

                    if action in computed_costs:
                        reward = computed_costs[action]
                    else:
                        original_cmd = 'cd /home/campero/Desktop/current/bala/DBPV/Dispatcher/openCLDispatcher/ && ./openCLDispatcher '
                        variant = action
                        cmd = original_cmd + str(variant) + " "+str(context)
                        output = subprocess.check_output(cmd, shell=True)
                        output = str(output)[2:]  # removing b'
                        output = output[:-1]  # removing last '
                        output = int(output)
                        reward = 1.0 / output
                        computed_costs[action] = reward
                    agent.learn(
                        str(variant+1) + ":" + str(reward) + ":" + str(probability) + " | " + str(context) )
                    scores[t, i] += reward
                    if opt:
                        optimal[t, i] += 1


                if t%10==0:
                    print("It:",t)
        file = open(test_name+'.txt', 'a')
        file.write("*********************************************************************\n")
        file.write("Action Frequency Table\n")
        file.write("*********************************************************************\n")
        for i, ag in enumerate(action_record):
            file.write(str(self.agent_labels[i])+"\n")
            file.write("Context (binned), Action, Frequency (all the rest are zeros)\n")
            for it in sorted(ag):
                for it2 in sorted(ag[it]):
                    file.write(str(it)+","+str(it2)+","+str(ag[it][it2])+"\n")

        file.write("*********************************************************************\n")
        file.write("Beliefs Table\n")
        file.write("*********************************************************************\n")
        for i, ag in enumerate(self.agents):
            file.write(str(self.agent_labels[i])+"\n")
            file.write("Context (binned), Belief per Action (or optimal action)\n")
            for cntxt in binned_contexts:
                choice = ag.predict("| " + str(cntxt)+"\n")
                if isinstance(choice, int):
                    choice = choice -1
                file.write(str(cntxt)+","+str(choice)+"\n")

        file.write("*********************************************************************\n")
        file.write("Average Scores\n")
        file.write("*********************************************************************\n")
        average_scores = scores.mean(0)
        for i, ag in enumerate(self.agents):
            file.write(str(self.agent_labels[i])+"\n")
            file.write("Average Score\n")
            file.write(str(average_scores[i])+"\n")

        file.write("*********************************************************************\n")
        file.write("Average Time (nanoseconds)\n")
        file.write("*********************************************************************\n")
        average_scores = scores.mean(0)
        for i, ag in enumerate(self.agents):
            file.write(str(self.agent_labels[i])+"\n")
            file.write("Average Time (nanoseconds)\n")
            file.write(str(1/average_scores[i])+"\n")
        file.close()
        return scores/experiments, optimal/experiments
*/

    }
};