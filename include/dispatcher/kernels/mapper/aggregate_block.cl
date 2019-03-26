__kernel void aggregate_block(__global unsigned int *input, __global unsigned int* result) {

    unsigned int global_id = get_global_id(0);
    int agg = 0;
    size_t pos = 32 * get_global_id(0);
    for (unsigned int i=0;i < 32; i++){
        agg +=  input[pos + i];
    }

    atomic_add(&result[0],agg);
}