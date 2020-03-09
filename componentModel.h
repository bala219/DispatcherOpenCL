//
// Created by gurumurt on 3/15/19.
//

#ifndef OPENCLDISPATCHER_COMPONENTMODEL_H
#define OPENCLDISPATCHER_COMPONENTMODEL_H

#endif //OPENCLDISPATCHER_COMPONENTMODEL_H

//primitives are semantically same execution functions that provides always a same result for given input
//The model is going to follow the semantics of the volcano-model


class primitive{

public:

    virtual void open() = 0;//Initializer function that takes care of setting up environment for the process
    virtual void next() = 0; //Computes result for given N values
    virtual void close() = 0; //Does clean up of book-keeping and internal states
};

class map : public primitive {

private:
    unsigned int* input;
    unsigned int* output;

public:

    void open(void* input){
    }

    void next(){

        //Execute map on the given device
    }
};