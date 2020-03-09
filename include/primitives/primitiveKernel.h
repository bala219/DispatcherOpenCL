//
// Created by gurumurt on 11/2/18.
//

#ifndef OPENCLDISPATCHER_PRIMITIVEKERNEL_H
#define OPENCLDISPATCHER_PRIMITIVEKERNEL_H

#endif //OPENCLDISPATCHER_PRIMITIVEKERNEL_H

#include "../headers.h"
#include "../globals.h"
#include "../../include/Importkernel.h"
#include "../data_api.h"
#include "../kernel_api.h"
#include "../runtime_api.h"

#include <iostream> // std::cout
#include <utility> // std::pair

class primitiveKernel{

public:
    string name, src;
    static int num_of_args;
    static int num_of_param;
    int iterationSize;
    vector<string> arguments;
    vector<int> parameters;
    size_t globalSize;

    void readSource(string path){
        src = readKernelFile(path);
    }

};

class selection : public primitiveKernel{

    selection(vector<string> args, vector<int> param,size_t globalSize=1024, size_t blockSize = 32, string type="block"){
        this->name = "selection_"+type;
        this->readSource("../kernels/mapper/"+name+".cl");

        if(args.empty()){
            this->arguments.push_back("selection"+to_string(num_of_args++));
            this->arguments.push_back("selectionResult");
        }
        else{
            arguments = args;
        }

        if(param.empty()){
            parameters.push_back(32);
        }
        else{
            parameters = param;
        }

        this->globalSize = globalSize/blockSize;
    }
};

class logical : public primitiveKernel{

    logical(vector<string> args, vector<int> param,size_t globalSize=1024, size_t blockSize = 32, short num_of_incoming=2, string type="block"){

        this->name = "logical_"+type;
        this->readSource("../kernels/mapper/"+name+".cl");

        if(args.empty()){
            for(short i=0; i< num_of_incoming; i++)
                this->arguments.push_back("logical"+to_string(num_of_args++));

            this->arguments.push_back("logicalResult");
        }
        else{
            arguments = args;
        }

        parameters.clear();
        this->globalSize = globalSize/blockSize;
    }
};

class materialize : public primitiveKernel{

    materialize(vector<string> args, vector<int> param, size_t globalSize=1024, size_t blockSize = 32, short num_of_incoming=2, string type="block"){

        this->name = "materialize_"+type;
        this->readSource("../kernels/mapper/"+name+".cl");

        if(args.empty()){
            for(short i=0; i< num_of_incoming; i++)
                this->arguments.push_back("materialize"+to_string(num_of_args++));

            this->arguments.push_back("materializeResult");
        }

        else{
            arguments = args;
        }

        parameters.clear();
        this->globalSize = globalSize/blockSize;
    }
};

class arithmetics : public primitiveKernel{

    arithmetics(vector<string> args, vector<int> param,size_t globalSize=1024, size_t blockSize = 32,short num_of_incoming=2, string type="block"){

        this->name = "arithmetic_"+type;
        this->readSource("../kernels/mapper/"+name+".cl");

        if(args.empty()){
            for(short i=0; i< num_of_incoming; i++)
                this->arguments.push_back("arithmetics"+to_string(num_of_args++));
            this->arguments.push_back("arithmeticsResult");
        }

        else{
            arguments = args;
        }

        parameters.clear();
        this->globalSize = globalSize/blockSize;
    }
};

class aggregate : public primitiveKernel{

    aggregate(vector<string> args, vector<int> param,size_t globalSize=1024, size_t blockSize = 32,short num_of_incoming=2, string type="block"){

        this->name = "aggregate_"+type;
        this->readSource("../kernels/mapper/"+name+".cl");

        if(args.empty()){
            for(short i=0; i< num_of_incoming; i++)
                this->arguments.push_back("aggregate"+to_string(num_of_args++));
            this->arguments.push_back("aggregateResult");
        }

        else{
            arguments = args;
        }

        parameters.clear();
        this->globalSize = globalSize/blockSize;
    }
};