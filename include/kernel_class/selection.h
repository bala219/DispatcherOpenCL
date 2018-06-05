//
// Created by gurumurt on 4/11/18.
//

#pragma once

#include "../base_kernel.h"
#include "../headers.h"
#include "../globals.h"

#ifndef OPENCLDISPATCHER_SELECTION_H
#define OPENCLDISPATCHER_SELECTION_H

#endif //OPENCLDISPATCHER_SELECTION_H

class selection1 : public base_kernel{

public:
    string op;

    selection1(){
        op = "<";
        name = "selection"+op;
        no_of_argument = 1;
        no_of_parameter = 1;
        param_type = {INTEGER};
    }

    virtual string generate_source(string op);

    void set_source(string source){
        kernel_src = source;
    }

};