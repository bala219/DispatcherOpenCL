//
// Created by gurumurt on 11/2/18.
//

#ifndef OPENCLDISPATCHER_NODEINFO_H
#define OPENCLDISPATCHER_NODEINFO_H

#endif //OPENCLDISPATCHER_NODEINFO_H


#include "../headers.h"
#include "../globals.h"
#include "../../include/Importkernel.h"
#include "../data_api.h"
#include "../kernel_api.h"
#include "../runtime_api.h"

#include <iostream> // std::cout
#include <utility> // std::pair

using namespace boost;
using namespace std;

enum nodes{

    selection,
    logical,
    materialize,
    arithmetics,
    aggregate,
    nodeSize
};

struct kernelObject{

    string name;
    vector<string> argument;
    vector<int> parameter;
    size_t globalSize;
};

size_t totalNodes;
static int total_arguments;

kernelObject getNode(nodes name, vector<string> arg=NULL, vector<int> para=NULL, size_t globalSize = 1024){

    kernelObject ko;

    switch(name){

        case selection :
            ko.name = "selection_block";

            if(arg.empty()){
                ko.argument.push_back("selection"+to_string(total_arguments));
                ko.argument.push_back("selectionResult");
            }
            else
                ko.argument = arg;

            ko.parameter = para;
            ko.globalSize = globalSize;

                break;

        case logical :
            ko.name = "logical_block";

            if(arg.empty()){
                ko.argument.push_back("logical"+to_string(total_arguments));
                ko.argument.push_back("logical"+to_string(total_arguments));
                ko.argument.push_back("logicalResult");
            }
            else
                ko.argument = arg;

            ko.parameter = para;
            ko.globalSize = globalSize;

            break;

        case materialize :
            ko.name = "materialize_block";

            if(arg.empty()){
                ko.argument.push_back("selection"+to_string(total_arguments));
                ko.argument.push_back("selectionResult");
            }
            else
                ko.argument = arg;

            ko.parameter = para;
            ko.globalSize = globalSize;

            break;

        case arithmetics :
            ko.name = "selection_block";

            if(arg.empty()){
                ko.argument.push_back("selection"+to_string(total_arguments));
                ko.argument.push_back("selectionResult");
            }
            else
                ko.argument = arg;

            ko.parameter = para;
            ko.globalSize = globalSize;

            break;

        case aggregate :
            ko.name = "selection_block";

            if(arg.empty()){
                ko.argument.push_back("selection"+to_string(total_arguments));
                ko.argument.push_back("selectionResult");
            }
            else
                ko.argument = arg;

            ko.parameter = para;
            ko.globalSize = globalSize;

            break;
    }
}