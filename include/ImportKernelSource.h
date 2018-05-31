//
// Created by tompi on 5/31/18.
//

#ifndef OPENCLDISPATCHER_IMPORT_KERNEL_SOURCE_H
#define OPENCLDISPATCHER_IMPORT_KERNEL_SOURCE_H

#include "sstream"
#include "iostream"
#include "headers.h"

string readKernelFile(string _path, string _value) {

    //cout << "PATH :: " << _path << endl;
    string _m_output = "";
    ifstream _m_myfile(_path);

    if(!_m_myfile) { // failed to open file
        cout << "File failed to open" << endl;
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

#endif //OPENCLDISPATCHER_IMPORT_KERNEL_SOURCE_H
