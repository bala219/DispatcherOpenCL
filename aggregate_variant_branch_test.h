//
// Created by gurumurt on 6/15/18.
//

#ifndef OPENCLDISPATCHER_AGGREGATE_VARIANT_TEST_H
#define OPENCLDISPATCHER_AGGREGATE_VARIANT_TEST_H

#endif //OPENCLDISPATCHER_AGGREGATE_VARIANT_TEST_H

#include "include/headers.h"
#include "include/Environment.h"
#include "include/data_api.h"
#include "include/kernel_api.h"
#include "include/runtime_api.h"
#include <math.h>

enum device_num {
    cpu = 1,
    gpu = 0
};

enum agg_variant{
    atomic = 0,
    branched_inc = 1,
    branched_dec = 2
};

double cpu_nanosecs, gpu_nanosecs;

struct aggregate_variant{
    string name;
    string src;
//    int global_size;
    int local_size;
}av[5];

int ps_size;

void initialize_aggregates(){

    //Branched local increment
    av[0].name = "branched_aggregate";
    av[0].src = "void kernel branched_aggregate(\n"
            "        global const int* PS,\n"
            "        global const int* BM,\n"
            "        global int* res\n"
            ") {\n"
            "\n"
            "size_t pos = 32 * get_global_id(0);\n"
            "unsigned int local_aggregate = 0;\n"
            "for (unsigned int i=0; i<32; ++i)\n"
            "{\n"
            "    local_aggregate++;\n"
            "    if(BM[pos]){\n"
            "        atomic_add(&res[PS[pos]-1],local_aggregate);\n"
            "        local_aggregate=0;\n"
            "    }\n"
            "    pos++;\n"
            "}\n"
            "atomic_add(&res[PS[pos-1]],local_aggregate);\n"
            "}";


    av[0].local_size = 32;

}

int *bm, *ps;
unsigned int  *cpu_res, *gpu_res;

void unique_percent_generate(int size = 1024, int unique=2){

    //Creating percentage unique values
    for(int i=1;i<size-unique;i++){
        bm[i]=0;
    }
    for(int i=size-unique+1;i<size;i++){
        bm[i]=1;
    }
    bm[size] = 1;

}


void percent_generate(int size = 1024, int repeat= 2){

    for(int i = 1; i < size; i++){
        for(int j = 0; (j<repeat-1)&&(i<size-1);j++){
            bm[i] = 0;
            i++;
        }
        bm[i] = 1;
    }

}

void prepare_data(int size = 1024, int repeat=2,short type=0){

    bm = new int[size];
    ps = new int[size];

    bm[0] = 0;

    if(type)
        unique_percent_generate(size,repeat);
    else
        percent_generate(size,repeat);
    //Derive prefix-sum from the results
    ps[0] = 0;
    for (int i = 1; i < size; i++) {
        ps[i] = ps[i - 1] + bm[i];
    }

    ps_size=ps[size-1];
    cpu_res = (unsigned int*)calloc((unsigned int)ps[size- 1]+1,sizeof(unsigned int));
    gpu_res = (unsigned int*)calloc((unsigned int)ps[size - 1]+1,sizeof(unsigned int));

}

void add_arguments(int variant, int dev, int size, unsigned int* res){

    cl_device_id d = device[dev][0];
    add_data("PS", ps, d, size);
    if (variant != 0)
        add_data("BM", bm, d, size);
    add_data("res", res, d, ps[size - 1]+1);
}

string kernel_name;

void add_variant_kernel(int variant, int dev){

    cl_device_id d = device[dev][0];

    kernel_name = av[variant].name;
    string kernel_source = av[variant].src;

    add_kernel(kernel_name, d, kernel_source);
}

bool flags_variant[2][2] = {{true,true},{true,true}};

unsigned int* execute_variant(int variant=1, int dev=0, int size = 1024, unsigned int* res = gpu_res){

    vector<string> arguments;
    vector<int> param;

    cl_device_id d = device[dev][0];
//
//    if(flags_variant[variant][dev]) {
//        add_arguments(variant, dev, size, gpu_res);

    add_data("PS", ps, d, size);
    if (variant != 0 && variant != 3)
        add_data("BM", bm, d, size);
    add_data("res", res, d, ps[size - 1]+1);

//        add_variant_kernel(variant, dev);
//        flags_variant[variant][dev]=false;
//    }
    string kernel_name = av[variant].name;
    string kernel_source = av[variant].src;
    add_kernel(kernel_name, d, kernel_source);

    arguments.push_back("PS");
    if (variant != 0 && variant != 3)
        arguments.push_back("BM");
    arguments.push_back("res");

    execute(d, kernel_name, arguments, param, (size_t)size / av[variant].local_size, (size_t)av[variant].local_size);

    unsigned int* r = get_data("res",d,ps[size - 1]+1);

//    cout<<"result\n";
//    for(int i = 0;i<ps[size - 1]+1;i++){
//        cout<<r[i]<<"\n";
//    }

    clear_data(arguments,d);

    return r;


}

double cpu_time, gpu_time;

int compare;

int aggregate_value;

void compute_aggregate(int variant = 2, int data_size = 1024){

    //Adding iteration here

    unsigned int* res1 = execute_variant(variant,0,data_size,gpu_res);
    gpu_time = nanoSeconds;

    unsigned int* res2 = execute_variant(variant,1,data_size,cpu_res);
    cpu_time = nanoSeconds;

    aggregate_value = res1[ps[data_size - 1]+1];
//    cout<<"aggregate: "<<aggregate_value<<"\n";
    int compare=memcmp(&res1[2],&res2[2],sizeof(unsigned int)*(ps[data_size - 1]-2));

//    clear_data(arguments,device[0][0]);
//    clear_data(arguments,device[1][0]);
//
//    arguments.clear();
//    param.clear();
}


//i -> percentage
//j -> variant


void extended_data_set_test(){

    initialize_aggregates();
    setup_environment();
    print_environment();

    ulong local_size;
    clGetDeviceInfo(device[0][0],CL_DEVICE_LOCAL_MEM_SIZE,sizeof(ulong),&local_size,NULL);
    cout<<local_size;
    exit(0);
    int data_size = pow(2,20);
    int iterate = 10;
    int step = 2;
    cout<<"Groups\tVariant\tCPU\tGPU\tR\tQ\n";
    for(int q=0;q<2;q++) {
        for (int i = step; i <= data_size; i *= step) {
            prepare_data(data_size, i, q);
            for (int j = 0; j < 3; j++) {
                double cpu_average = 0;
                double gpu_average = 0;
                for (int z = 0; z < iterate; z++) {
//                    setup_environment();
                    compute_aggregate(j, data_size);
                    cpu_average += cpu_time;
                    gpu_average += gpu_time;
                }
                cout << ps_size << "\t" << j << "\t" << cpu_average / iterate << "\t"
                     << gpu_average / iterate << "\t" << compare <<"\t" << q << "\n";
            }
        }
    }
}

//q=1 ; heavey hitter dataset
void test_variants(){

    /*
     * Data test
     * Vary only the data size and see how the variants fair
     */

    initialize_aggregates();
    setup_environment();

    int data_size = (int) pow(2,10);
    int iterate = 20;
    int step = 32;

    for(int i = step;i<=data_size;i+=step){
        prepare_data(data_size, i,1);
        for(int j = 0;j<4;j++){
            double cpu_average = 0;
            double gpu_average = 0;
            for(int z=0;z<iterate;z++){
                initialize_aggregates();
                setup_environment();
                compute_aggregate(j,data_size);
                cpu_average += cpu_time;
                gpu_average += gpu_time;
            }
            cout<<ps_size<<"\t"<<i<<"\t"<<j<<"\t"<<cpu_average/iterate<<"\t"<<gpu_average/iterate<<"\t"<<compare<<"\n";
        }
    }

}