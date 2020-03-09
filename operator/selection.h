//
// Created by gurumurt on 9/21/18.
//

#ifndef OPENCLDISPATCHER_SELECTION_H
#define OPENCLDISPATCHER_SELECTION_H

#endif //OPENCLDISPATCHER_SELECTION_H

#include "../include/headers.h"

class selection{

    string name = "SELECTION";

    double execution_time = 0.0;

    unsigned int* result;


    enum condition{
        EQUAL,
        LESS_THAN,
        GREATER_THAN,
        LESS_EQUAL,
        GREATER_EQUAL,
        NOT_EQUAL
    };

    unsigned int* naive_execution(unsigned int* input, condition comp_op,unsigned int comparator, size_t size){

        result = new unsigned int[size];

        for(int i = 0; i<size;i++){
                result[i] = (input[i]==comparator);
        }

        return result;
    }

    double get_cost_model(size_t size){

        //Device Based transfer time
        //Add cost model
        return size;
    }

    double get_execution_time(double current_time){

        return (execution_time + current_time) / 2;
    }

};