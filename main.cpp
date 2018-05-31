//
// Created by gurumurt on 3/19/18.
//

#include "include/Environment.h"
#include "include/BitonicSort.h"

//cl_mem result;

//void test_addition();
void env_setup();

int main(int argc, char *argv[]) {

    //cross_device();
    //test_addition();
    env_setup();
}

void env_setup() {

    setup_environment();
    print_environment();

    // Array of elements
    int _m_size = 16;
    int _m_arr[_m_size];
    for (int i = 0; i < _m_size; i++) {
        _m_arr[i] = rand() % _m_size;
    }

    // Get Device ID
    cl_device_id CPU, GPU;

    GPU = device[0][0];
    //CPU = device[0][1];

    // Call the Bitonic Sorting
    bitonicSort(GPU, _m_arr, _m_size);

}

/*void test_addition(){

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
//    CPU = device[0][1];

    //Add data to the respective devices
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
                           "\t\n"
                           "\t// Get the index of the work item\n"
                           "\tint index =  get_global_id(0);\n"
                           "\tC[index] = A[index] + B[index];\n"
                           "}";
    string kernel_name = "add";
    add_kernel(kernel_name, GPU, kernel_source);

    vector<string> arguments;
    arguments.push_back("a");
    arguments.push_back("b");
    arguments.push_back("c");

    vector<int> param;
    execute(GPU, kernel_name,arguments,100,param);

    print_data("c",GPU,100);
}/**/