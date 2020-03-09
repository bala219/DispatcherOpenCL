#include "include/headers.h"
#include "include/Environment.h"
#include "include/data_api.h"
#include "include/kernel_api.h"
#include "include/runtime_api.h"
#include <math.h>

void test_aggregates(int size, int no_of_repeats, int variant, int device_num);
void initialize_aggregates();

enum device_num {
    cpu = 1,
    gpu = 0
};

enum agg_variant{
    atomic = 0,
    branched_inc = 1,
    branched_dec = 2
};

void run_aggregate(){

    initialize_aggregates();
    int data_size = 100;
    int iterations = 1;

    for(int j=1;j<=100;j++) {
//        for (int k = 0; k < 3; k++) {
//            for (int l = 0; l < 2; l++) {
                int average_time = 0;
                for (int i = 0; i < iterations; i++) {
                    test_aggregates(data_size, j, 0, 0);
                    average_time += nanoSeconds;
                }
//                cout << j << "\t" << k << "\t" << l << "\t" << average_time << "\n";
//            }
//        }
    }
}

struct aggregate_variant{
    string name;
    string src;
//    int global_size;
    int local_size;
}av[3];

void initialize_aggregates(){

    // Atomic aggregate
    av[0].name = "atomic_agg";
    av[0].src = "__kernel void atomic_agg(  __global unsigned int* ps,\n"
            "                           __global unsigned int* res)\n"
            "\n"
            "{\n"
            "    atomic_add(&res[ps[get_global_id(0)]],1);\n"
            "}";
//    av[0].global_size = 1;
    av[0].local_size = 1;

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
            "        atomic_add(&res[PS[pos]],local_aggregate);\n"
            "        local_aggregate=0;\n"
            "    }\n"
            "    pos++;\n"
            "}\n"
            "atomic_add(&res[PS[pos]],local_aggregate);\n"
            "\n"
            "}";
//    av[1].global_size = 32;
    av[1].local_size = 32;

    //Branched position decrement
    av[2].name = "branched_pos_aggregate";
    av[2].src = "void kernel branched_pos_aggregate(\n"
            "        global const int* PS,\n"
            "        global const int* BM,\n"
            "        global int* res\n"
            ") {\n"
            "\n"
            "size_t pos = 32 * get_global_id(0);\n"
            "unsigned int prev_pos = pos;\n"
            "for (unsigned int i=0; i<32; ++i)\n"
            "{\n"
            "\n"
            "if(BM[pos]){\n"
            "unsigned int local_aggregate = pos - prev_pos;\n"
            "atomic_add(&res[PS[pos]],local_aggregate);\n"
            "prev_pos=pos;\n"
            "}\n"
            "pos++;\n"
            "}\n"
            "atomic_add(&res[PS[pos]], pos - prev_pos);\n"
            "}";
//    av[2].global_size = 32;
    av[2].local_size = 32;
}



void test_aggregates(int size, int no_of_repeats, int variant, int device_num) {

    setup_environment();
    int *bm, *ps;

    bm = new int[size];
    ps = new int[size];

    bm[0] = 1;
//    cout<<" no of repeats: "<<no_of_repeats<<"\n";
    //Generate the bitmap directly
//    cout<<"bitmap\n";
    for(int i=1;i<size;i+=no_of_repeats){
            for(int j=0;(j<(no_of_repeats-1))&&(i+j<size);j++){
                bm[i+j] = 0;
            }
            if(i+no_of_repeats-1 < size)
                bm[i+no_of_repeats-1] = 1;
    }

//    cout<<"Bitmap results: \n";
//    for(int i=0;i<size;i++){
//        cout<<bm[i]<<"\n";
//    }
//    return;
    ps[0] = 0;
//    cout<<"prefix-sum\n";
    for (int i = 1; i < size; i++) {
        ps[i] = ps[i - 1] + bm[i];
//        cout<<ps[i]<<"\n";
    }

    unsigned int* res = (unsigned int*)calloc(ps[size - 1] + 1,sizeof(unsigned int));
    for(int i=0;i<ps[size - 1] + 1;i++)
        res[i]=0;

    string kernel_name = av[variant].name;

    string kernel_source = av[variant].src;

    cl_device_id GPU;

    GPU = device[device_num][0];

    add_data("PS", ps, GPU, size);
    if (variant != 0)
        add_data("BM", bm, GPU, size);
    add_data("res", res, GPU, ps[size - 1]+1);

    add_kernel(kernel_name, GPU, kernel_source);

    vector<string> arguments;
    vector<string> param;

    arguments.push_back("PS");
    if (variant != 0)
        arguments.push_back("BM");
    arguments.push_back("res");
    //Get default local size value

    execute(GPU, kernel_name, arguments, param, size / av[variant].local_size, av[variant].local_size);

    res = get_data("res",GPU,ps[size - 1]+1);
    cout<<"start\n";
    for(int i=0; i<size;i++){
        cout<<res[i]<<"\n";
    }
    cout<<"end\n";

//    cout<<"\n_start__\n";
//    print_data("res",GPU,ps[size - 1]+1);
//    cout<<"\n_end__\n";
//    delete[] col;
    delete[] bm;
    delete[] ps;
}