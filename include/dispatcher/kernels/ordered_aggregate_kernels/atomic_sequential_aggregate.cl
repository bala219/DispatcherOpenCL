__kernel void atomic_sequential_aggregate(  __global unsigned int* ps,
                                 __global unsigned int* res){

    size_t pos = get_global_id(0) * ITERATOR;
    for(int i=0;(i < ITERATOR);i++){
        atomic_add(&res[ps[pos + i]],1);
    }

}