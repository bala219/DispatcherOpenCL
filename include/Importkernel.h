
//
// Created by tompi on 5/31/18.
//

#ifndef OPENCLDISPATCHER_IMPORTKERNEL_H
#define OPENCLDISPATCHER_IMPORTKERNEL_H

#endif //OPENCLDISPATCHER_IMPORTKERNEL_H

#pragma once

#include <fstream>
#include "globals.h"
#include "headers.h"

string readKernelFile(string _path) {

    //cout << "PATH :: " << _path << endl;
    string _m_output;

    ifstream _m_myfile(_path);

//    cout<<_path<<endl;


    if(!_m_myfile) { // failed to open file
        cout << "File failed to open : "<<_path << endl;
    }
    else {
        //cout << "File successfully to opened" << endl;

        stringstream _m_buffer;
        _m_buffer << _m_myfile.rdbuf();
        _m_output = _m_buffer.str();
        _m_myfile.close();
    }
    return _m_output;
}
