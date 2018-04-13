//
// Created by gurumurt on 3/19/18.
//

#include "include/headers.h"
#include "include/Environment.h"
#include "include/data_api.h"
#include "include/kernel_api.h"
#include "include/runtime_api.h"
#include "Query6.h"

cl_mem result;

void test_addition();

int main(int argc, char* argv[]) {

    cross_device();
//    test_addition();
}


void test_addition(){

    setup_environment();
    print_environment();

    cl_device_id CPU, GPU;

    int a[100], b[100],c[100];
    for(int i=0;i<100;i++){
        a[i] = i;
        b[i] = i;
        c[i] = 1;
    }

    GPU = device[0][0];
    CPU = device[1][0];

    //Add data to the respective devices
    add_data("a",a,CPU, 100);
    add_data("b",b,CPU,100);
    add_data("c",c,CPU,100);

    add_data("a",a,GPU,100);
    add_data("b",b,GPU,100);
    add_data("c",c,GPU,100);

    lookup_data_buffer();

    //Add kernels to the respective devices
    string kernel_source = "void kernel add(\n"
            "\tglobal const int* A,\n"
            "\tglobal const int* B,\n"
            "\tglobal int* C\n"
            "\t) {\n"
            "\tC[get_global_id(0)] =\n"
            "\t\tA[get_global_id(0)] + B[get_global_id(0)];\n"
            "}";
    string kernel_name = "add";
    add_kernel(kernel_name, GPU, kernel_source);
    add_kernel(kernel_name, CPU, kernel_source);

    vector<string> arguments;
    arguments.push_back("a");
    arguments.push_back("b");
    arguments.push_back("c");

    vector<int> param;
    execute(GPU, kernel_name,arguments,100,param);
    execute(CPU, kernel_name,arguments,100,param);

    print_data("c",CPU,100);
    print_data("c",GPU,100);
}