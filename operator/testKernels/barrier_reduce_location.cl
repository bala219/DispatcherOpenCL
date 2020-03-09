__kernel void barrier_reduce_location(__global int* PS, __global int* result){

    unsigned int i = get_global_id(0);

    if(PS[i] != PS[i+1])
        result[PS[i]] = i;
}