//
// Created by gurumurt on 11/6/18.
//

#ifndef EXEC_ENGINE_COMPARISON_IMPORTKERNELSOURCE_H
#define EXEC_ENGINE_COMPARISON_IMPORTKERNELSOURCE_H

#include <fstream>
#include "globals.h"

namespace dispatcher{

    string readKernelFile(string _path) {

        //cout << "PATH :: " << _path << endl;
        string _m_output;
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
}

#endif //EXEC_ENGINE_COMPARISON_IMPORTKERNELSOURCE_H
