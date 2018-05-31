//
// Created by gurumurt on 3/19/18.
//

#ifndef OPENCLTEST_HEADERS_H
#define OPENCLTEST_HEADERS_H

#endif //OPENCLTEST_HEADERS_H

#pragma once

#include <iostream>
#include <fstream>
//#include <vector>
#include <bitset>
//#include <boost/chrono.hpp>
#include <iomanip>
#include <map>
#include <vector>
#include <list>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

enum RETURN_TYPE {
    INT,
    FLOAT,
    DOUBLE,
    CHAR
};

using namespace std;