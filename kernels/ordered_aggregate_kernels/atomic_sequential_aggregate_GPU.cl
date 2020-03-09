__kernel void atomic_sequential_aggregate_GPU(  __global unsigned int* ps,
                                 __global unsigned int* res){

    size_t pos = get_global_id(0);

    for(short i =0;i<DATA_SIZE/get_global_size(0);i++){

        atomic_add(&res[ps[pos]],1);
        pos+=(get_global_size(0));
    }
}