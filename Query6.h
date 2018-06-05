//
// Created by gurumurt on 3/23/18.
//

#ifndef OPENCLDISPATCHER_QUERY6_H
#define OPENCLDISPATCHER_QUERY6_H

#endif //OPENCLDISPATCHER_QUERY6_H

#pragma once

#include <fstream>
#include <cstring>
#include <climits>
#include "include/headers.h"
//#include "include/kernel_store.h"
#include "include/kernel_generator/selection.h"
#include "include/kernel_class/logical.h"
#include "include/data_api.h"
#include "include/kernel_api.h"
#include "include/runtime_api.h"

static int byteArrayToInt(char* b) {
    return (b[3] & 0xFF) | (b[2] & 0xFF) << 8 | (b[1] & 0xFF) << 16
           | (b[0] & 0xFF) << 24;
}

uint32_t** readTBL(uint32_t* numPoints, uint32_t* numDimRef, uint32_t* dimMaxValue,const char* filePath, uint32_t** maxDims) {
    ifstream myfile;
    cout<<"filepath : "<<filePath<<endl;
    myfile.open(filePath, ios::in | ios::binary);
    uint32_t** store;
    vector<uint32_t>* min=new vector<uint32_t>;
    vector<uint32_t>* max=new vector<uint32_t>;

    if (myfile.is_open()) {
        cout << "Start reading: " << filePath << endl;
        //std::string line;
        int repeat = 1;
        char* bytes = (char*) malloc(4);
        myfile.read(bytes, 4);
        int numRows = byteArrayToInt(bytes);
        numRows *=repeat ;
        numRows = 1000; //This alter the total number of tuples being accessed
        *numPoints = numRows;
        myfile.read(bytes, 4);
        int numDim = byteArrayToInt(bytes);
        numDim = 4;
        *numDimRef = numDim;
        //to be adapted
        *dimMaxValue = INT_MAX;


        for (int i = 0; i < numDim; i++) {
            min->push_back(INT_MAX);
            max->push_back(0);
        }

        store = new uint32_t*[numRows];
        for (int row = 0; row < numRows; row++) {
            store[row] = new uint32_t[numDim];

            for (int dim = 0; dim < numDim; dim++) {

                if(dim >4)
                    continue;
                //                ch ar temp;
                myfile.read(bytes, 4);
                store[row][dim] = (uint32_t) byteArrayToInt(bytes);
                if ((*min)[dim] > store[row][dim])
                    (*min)[dim] = store[row][dim];
                if ((*max)[dim] < store[row][dim])
                    (*max)[dim] = store[row][dim];

            }
            //Add duplicates in the next two rows
            for(int i=0;i<repeat ;i++) {
                row++;
                store[row] = new uint32_t[numDim];
                for (int dim = 0; dim < numDim; dim++) {
                    if (dim > 4)
                        continue;
                    //                ch ar temp;
                    store[row][dim] = store[row - 1][dim];
                }
            }

        }
        cout << "Stop reading!" << endl;
        myfile.close();
        free(bytes);

        cout << "No. of dimensions: " << numDim << endl;
        cout << "No. of Rows: " << numRows << endl;
        cout << "Ranges: ";
        cout << "[ " << (*min)[0] << ", " << (*max)[0] << "]";
        for (int dim = 1 ; dim < numDim; dim++) {
            cout << ", [ " << (*min)[dim] << ", " << (*max)[dim] << "]";

        }
        cout << endl;
        cout << endl;

        (*maxDims)=&((*max)[0]);


    } else {
        cout << "Could not open file : " <<strerror(errno)<< endl;
        exit(-1);
    }

    return store;
}


/*
 * Running Q6 implementation
 */

void scalar(uint32_t** data,uint32_t tuples){

    time_t start = clock();

    uint32_t *date, *discount, *quantity, *line_status, *return_flag, *ship_instr, *extended_price;

    date = data[0];
    discount = data[1];
    quantity = data[2];
    extended_price = data[3];
    int sum = 0;
    for(int i=0;i<tuples;i++){
        sum = sum + (extended_price[i]*discount[i]*(date[i]>0&&date[i]<100000&&discount>0&&discount[i]<50&&quantity[i]<24));
    }
    time_t end = clock();
    cout <<"Scalar Execution Sum : "<<sum<<endl;
    cout<<"Scalar execution time (in Secs): "<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
}

uint32_t *date, *discount, *quantity, *line_status, *return_flag, *ship_instr, *extended_price;

void cross_device(){

    unsigned int size = 1;
    float selectivity = 0.3;
    const char *fileName = "lineitem.tbl";

    uint32_t tuples;
    uint32_t numDim;
    uint32_t dimMaxValue;
    uint32_t** data;
    uint32_t* maxDims;

    data = readTBL(&tuples, &numDim, &dimMaxValue, fileName, &maxDims);


    date = data[0];
    discount = data[1];
    quantity = data[2];
    extended_price = data[3];

    cout<<"Scalar execution"<<endl;



    scalar(data, tuples);

    /*
     * Cross device execution steps start here
     */

    setup_environment();

    cl_device_id GPU = device[0][0];
    cl_device_id CPU = device[1][0];

    int result1[tuples/32];
    int result2[tuples/32];
    int result3[tuples/32];

    //Prepare and add data to the device
    add_data("ship date",date,GPU,tuples);

    add_data("discount",discount,GPU,tuples);

    add_data("quantity",quantity,CPU,tuples);

    add_data("selection_result1",result1,GPU,tuples/32);

    add_data("selection_result2",result2,GPU,tuples/32);

    add_data("selection_result3",result3,CPU,tuples/32);

    vector<string> arguments1, arguments2, arguments3;

    arguments1.push_back("ship date");
    arguments1.push_back("selection_result1");

    arguments2.push_back("discount");
    arguments2.push_back("selection_result2");


    arguments3.push_back("quantity");
    arguments3.push_back("selection_result3");

    //Prepare selection kernel
    //Ship date and dicsount are selection between
    //quantity is selection less than

    selection_kernel between("between");
    selection_kernel less_than("less");

    logical_kernel AND("AND");

    vector<unsigned int> parameter1, parameter2, parameter3;

    parameter1.push_back(0);
    parameter1.push_back(100000);

    parameter2.push_back(0);
    parameter2.push_back(50);

    parameter3.push_back(24);

    //Compiling kernels in devices
    add_kernel(between.get_kernel_name(),GPU,between.kernel_src);
    add_kernel(less_than.get_kernel_name(),CPU,less_than.kernel_src);

    between.set_global_size(tuples);
    //Execute the kernels
    execute(GPU,between.get_kernel_name(),arguments1,between.global_size,parameter1);
//    execute(GPU,between.get_kernel_name(),arguments2,between.global_size,parameter2);

    less_than.set_global_size(tuples);
    execute(CPU,less_than.get_kernel_name(),arguments3,tuples/32,parameter3);

//    //Performing logical operation
//    parameter1.clear();
//    add_kernel(AND.get_kernel_name(),GPU,AND.get_kernel_source());
//
//    //Adding the data to argument list
//    arguments1.clear();
//    arguments1.push_back("selection_result1");
//    arguments1.push_back("selection_result2");
//
//
//    add_data("selection_result4",result1,GPU,tuples/32);
//
//    arguments1.push_back("selection_result4");
//
//    execute(GPU,AND.get_kernel_name(),arguments1,tuples,parameter1);
//    print_data("selection_result4",GPU,tuples,true);
//    print_data("selection_result2",GPU,tuples/32,true);
//    print_data("selection_result3",CPU,tuples/32,true);

}