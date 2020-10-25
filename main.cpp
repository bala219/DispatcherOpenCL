//
// Created by gurumurt on 3/19/18.
//

#include "include/headers.h"
#include "include/Environment.h"
#include "include/data_api.h"
#include "include/kernel_api.h"
#include "include/runtime_api.h"
#include "BitonicSorting.h"
#include "include/kernelGraphs.h"
#include "include/boostGraphs.h"
#include "include/kernelTestGraph.h"
#include "operator/evaluate_atomics.h"
#include "operator/aggregateTests.h"

cl_mem result;

void test_addition();
//void agg();

void test_aggregates(int size, int no_of_repeats, int variant, int device_num);
void test_inverse_grouped_aggregation();
void test_intra_warp_ps();
void initialize_aggregates();

void split_primitive(){

    int input[32];
    int output[32];

    for(int i=0;i<32;i++){
        input[i] = i;
        output[i] = 2;
    }

    unsigned int pivot = input[rand() % (int)(32+1)];

    cout<<"pivot: "<<pivot<<"\n";

    setup_environment();
    cl_device_id CPU = device[0][0];
    add_data("input", input, CPU,32);
    add_data("output", output, CPU,32);

    vector<string> arguments;
    vector<unsigned int> param;

    arguments.push_back("input");
    arguments.push_back("output");
    param.push_back(pivot);

    string kernel_name = "split_bitmap";
    string kernel_src = "void kernel split_bitmap(\n"
            "        global unsigned int* input,\n"
            "        global unsigned int* output,\n"
            "        unsigned int pivot\n"
            ") {\n"
            "    int pos = get_global_id(0);\n"
            "\n"
            "    output[pos] = (input[pos]>pivot);\n"
            "}";

    add_kernel(kernel_name,CPU,kernel_src);
    execute(CPU,kernel_name,arguments,param,32,1);

}

//uint inp[(int)pow(2,16)];
//
//void test_bitonic_sort(int size){
//
//    for(int i = size;i>0;i--){
//        inp[size - i] = i;
//    }
//
//    BitonicSort(device[1][0],inp,size);
//}


void test_hashing(){
//    test_hash();
}


void testing_NUMA(){

    setup_environment();
    print_environment();

    //Partition device based on affinity

    //Understand the possible sub device opportunities available
    cl_device_affinity_domain dev_aff;
    size_t returnSize = 0;
    cl_uint max_sub_device=0;

    clGetDeviceInfo(device[1][0],CL_DEVICE_PARTITION_MAX_SUB_DEVICES,NULL,NULL,&returnSize);
    clGetDeviceInfo(device[1][0],CL_DEVICE_PARTITION_MAX_SUB_DEVICES,returnSize,&max_sub_device,NULL);

    cout<<returnSize<<endl<<max_sub_device<<endl<<endl;


    clGetDeviceInfo(device[1][0],CL_DEVICE_MAX_COMPUTE_UNITS,NULL,NULL,&returnSize);
    clGetDeviceInfo(device[1][0],CL_DEVICE_MAX_COMPUTE_UNITS,returnSize,&max_sub_device,NULL);

    cout<<returnSize<<endl<<max_sub_device<<endl;


    clGetDeviceInfo(device[1][0],CL_DEVICE_PARTITION_AFFINITY_DOMAIN,NULL,NULL,&returnSize);
    clGetDeviceInfo(device[1][0],CL_DEVICE_PARTITION_AFFINITY_DOMAIN,returnSize,&dev_aff,NULL);

    cout<<returnSize<<endl<<dev_aff<<endl;
}

void testVariants();

//
//void testJoin(){
//
//    test_hash();
//
//}

int main(int argc, char* argv[]) {

//argc means number of arguments whereas argv is argument itself
//    std::ofstream output("error.txt");
//    std::streambuf* p_cerrbuffer=std::cerr.rdbuf();
//    std::cerr.rdbuf(output.rdbuf()); // redirecting to a file
    setup_environment();
    //print_environment();

    //Testing TPCH with barrier atomics
    //TPCHqueriesTest();


    short p = 10;
    short dev_id = 0;
    size_t ds = pow(2,p);

    short option =  argc<2?1:std::stoi(argv[1]);
    //cout<<option;
    short end_dis = 8; //8
    short dataDis = 0; //0
    short start_v = 0;
    short end_v = 8; //5

    if(option == 2){

        end_dis = 14;
        dataDis = 9;
        start_v=0;
        end_v = 5;
    }
    //executeGraph(reinterpret_cast<cl_device_id>(dev_id));
    start_v=5;
    end_v=7;
    dataDis=0;
    end_dis= 10;
    //cout<<argv[0];
    //cout<<argc;
    cout<<"Data distribution\tData Size\tkernel name\tresult size\tresult\ttime\tbaseline"<<endl;
    for(;dataDis <= end_dis;dataDis++) {// 0 -- 8 => ORDERED | 9 -- 14 => UNORDERED
        for (short p = 10; p <= 11; p++) {
            ds = pow(2, p);
            //prepare_input(dataDis, ds, pow(2,p-2));
            prepare_input();
            for (short v = start_v; v <= end_v; v++) { //6
                cout
                        << dataDis
                        << "\t"
                        << ds
                        << "\t";
               test_atomic_operation(v, dev_id, dataDis, ds);
                //if(v==0 || v==5)
                    //test_barrier_reduce(dev_id, dataDis, ds);
                //else
                   cout<<"0"<<endl;
            }
        }
    }

}