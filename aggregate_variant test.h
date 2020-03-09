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
#include "include/Importkernel.h"
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

    // Atomic aggregate
    av[0].name = "branched_aggregate_local";

    av[0].src = readKernelFile("kernels/local_branched_aggregate.cl");

    av[0].local_size = 4;

    //Branched local increment
    av[1].name = "branched_aggregate";
    av[1].src = "void kernel branched_aggregate(\n"
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

    av[1].local_size = 32;

    //Branched position decrement
    av[2].name = "branched_pos_aggregate";
    av[2].src = "void kernel branched_pos_aggregate(\n"
            "        global const int* PS,\n"
            "        global const int* BM,\n"
            "        global int* res\n"
            ") {\n"
            "\n"
            "size_t pos = 32*get_global_id(0);\n"
            "size_t prev_pos = pos;\n"
            "for (unsigned int i=0; i<32; ++i)\n"
            "{\n"
            "\n"
            "if(BM[pos]){\n"
            "unsigned int local_aggregate = pos - prev_pos;\n"
//            "res[PS[pos]-1] = local_aggregate;\n"
          "atomic_add(&res[PS[pos]-1],local_aggregate);\n"
            "prev_pos=pos;\n"
            "}\n"
            "pos++;\n"
            "}\n"
            "atomic_add(&res[PS[pos-1]], pos - prev_pos);\n"
            "}";
//    av[2].global_size = 32;
    av[2].local_size = 32;

    // Atomic aggregate
    av[3].name = "atomic_agg_var";
    av[3].src = "__kernel void atomic_agg_var(  __global unsigned int* ps,\n"
            "                           __global unsigned int* res)\n"
            "\n"
            "{\n"
            "    atomic_add(&res[ps[get_global_id(0)]],1);\n"
            "    atomic_add(&res[ps[get_global_id(0)+1]],1);\n"
            "    atomic_add(&res[ps[get_global_id(0)+2]],1);\n"
            "    atomic_add(&res[ps[get_global_id(0)+3]],1);\n"
            "}";
//    av[0].global_size = 1;
    av[3].local_size = 4;

    //Predicated grouped aggregation
    av[4].name = "predicated_pos_aggregate";
    av[4].src = "void kernel predicated_pos_aggregate(\n"
            "        global const int* PS,\n"
            "        global const int* BM,\n"
            "        global int* res\n"
            ") {\n"
            "\n"
            "size_t pos = 32 * get_global_id(0);\n"
            "local unsigned int local_aggregate = 0;\n"
            "for (unsigned int i=0; i<32; ++i)\n"
            "{\n"
            "local_aggregate++;\n"
            "res[PS[pos]-1] = BM[pos] * local_aggregate;\n"
            "local_aggregate= !BM[pos] * local_aggregate;\n"
            "pos++;\n"
            "}\n"
            "barrier(CLK_LOCAL_MEM_FENCE);"
            "atomic_add(&res[PS[pos-1]],local_aggregate);"
            "}\n";
//    av[2].global_size = 32;
    av[4].local_size = 32;

    av[5].name = "branched_aggregate_barrier";
    av[5].src = "void kernel branched_aggregate_barrier(\n"
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
            "        res[PS[pos]-1] = local_aggregate;\n"
//          "        atomic_add(&res[PS[pos]-1],local_aggregate);\n"
            "        local_aggregate=0;\n"
            "    }\n"
            "    pos++;\n"
            "}\n"
            "barrier(CLK_LOCAL_MEM_FENCE);"
            "atomic_add(&res[PS[pos-1]],local_aggregate);\n"
            "}";

// vary local size upto the optimal value
//    av[1].global_size = 32;
    av[5].local_size = 32;

    av[6].name = "branched_local_aggregate";
    av[6].src = "void kernel branched_local_aggregate(\n"
            "        global const int* PS,\n"
            "        global const int* BM,\n"
            "        global int* res\n"
            "){\n"
            "\n"
            "\n"
            "size_t pos = 32 * get_global_id(0);\n"
            "unsigned int local_aggregate = 0;\n"
            "printf(\"pos:%d\",pos);\n"
            "__local int l_PS[32], l_RES[32];\n"
            "int lp = 0;\n"
            "\n"
            "l_PS[lp] = PS[pos];\n"
            "l_RES[lp] = 0;\n"
            "for(int i = 0; i<32 ; i++){\n"
            "    l_RES[lp]++;\n"
            "    if(BM[pos]){\n"
            "        lp++;\n"
            "        l_PS[lp] = PS[pos];\n"
            "        l_RES[lp] = 0;\n"
            "    }\n"
            "    pos++;\n"
            "}\n"
            "for(int i = 0; i< lp;i++){\n"
            "    printf(\"pos:%d ps:%d res:%d PS:%d lp:%d\\n\",get_global_id(0),l_PS[i],l_RES[i],PS[32*get_global_id(0)],lp);\n"
//            "atomic_add(&res[l_PS[i]], l_RES[i]);\n"
            "    }\n"
            "}";

    av[6].local_size = 32;
}

int *bm, *ps;
unsigned int  *cpu_res, *gpu_res;

void unique_percent_generate(int size = 1024, int unique=2){

    //Creating percentage unique values
    for(int i=1;i<size-unique;i++){
        bm[i]=0;
    }
    for(int i=size-unique;i<size;i++){
        bm[i]=1;
    }

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
    if (variant != 1)
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
    if (variant != 2 && variant != 3)
        add_data("BM", bm, d, size);
    add_data("res", res, d, ps[size - 1]+1);

//        add_variant_kernel(variant, dev);
//        flags_variant[variant][dev]=false;
//    }
    string kernel_name = av[variant].name;
    string kernel_source = av[variant].src;
    add_kernel(kernel_name, d, kernel_source);

    arguments.push_back("PS");
    if (variant!=2 && variant!=3)
        arguments.push_back("BM");
    arguments.push_back("res");
    if(variant == 0)
        execute(d, kernel_name, arguments, param, size/av[0].local_size, av[0].local_size,1);
    else
        execute(d, kernel_name, arguments, param, size/av[0].local_size, av[0].local_size);

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

void compute_aggregate(int variant = 1, int data_size = 1024){

    //Adding iteration here
//    prepare_data(data_size, 10);

    unsigned int* res1 = execute_variant(variant,1,data_size,gpu_res);
    gpu_time = nanoSeconds;
//    cout<<nanoSeconds<<"\n";
//    cout<<"CPU variant"<<endl;
    unsigned int* res2 = execute_variant(variant,0,data_size,cpu_res);
    cpu_time = nanoSeconds;

    aggregate_value = res1[ps[data_size - 1]+1];
//    cout<<"aggregate: "<<aggregate_value<<"\n";
//     compare=memcmp(&res1[2],&res2[2],sizeof(unsigned int)*(ps[data_size - 1]-2));
    compare = 0;
//    clear_data(arguments,device[0][0]);
//    clear_data(arguments,device[1][0]);
//
//    arguments.clear();
//    param.clear();
}


//i -> percentage
//j -> variant

void test_local_variant(){
    initialize_aggregates();
    setup_environment();
    int data_size = 64;
    int iterate = 1;
    cout<<"Testing local aggregate\n";

    prepare_data(data_size,32,1);
    compute_aggregate(0,data_size);
//    cout<<gpu_time<<"\n";
}


void extended_data_set_test(){

    initialize_aggregates();
    setup_environment();
    int data_size = 2048;
    int iterate = 1;
    cout<<"\n\nUnique\n\n";
    for(int i = 2;i<=data_size;i*=2){
        prepare_data(data_size, 16,0);
        for(int j = 0;j<1;j++){
            double cpu_average = 0;
            double gpu_average = 0;
            for(int z=0;z<iterate;z++){
                compute_aggregate(j,data_size);
                cpu_average += cpu_time;
                gpu_average += gpu_time;
            }
            cout<<ps_size<<"\t"<<i<<"\t"<<j<<"\t"<<cpu_average/iterate<<"\t"<<gpu_average/iterate<<"\t"<<compare<<"\n";
        }
    }
}


void execute_repeats(){

    initialize_aggregates();
    setup_environment();
    int data_size = 100000;
    int iterate = 1;
    int steps = 1000;
    cout<<"\n\nUnique\n\n";
        for(int i = steps;i<data_size;i+=steps){
        prepare_data(data_size, 2,1);
        for(int j = 6;j<7;j++){
//            if(j==0||j==2||j==3)
//                continue;
            double cpu_average = 0;
            double gpu_average = 0;
            for(int z=0;z<iterate;z++){
                compute_aggregate(j,data_size);
                cpu_average += cpu_time;
                gpu_average += gpu_time;
            }
            cout<<ps_size<<"\t"<<i<<"\t"<<j<<"\t"<<cpu_average/iterate<<"\t"<<gpu_average/iterate<<"\t"<<compare<<"\n";
        }
    }

}
