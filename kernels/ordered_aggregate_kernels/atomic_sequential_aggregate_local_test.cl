__kernel void atomic_sequential_aggregate_local_test(  __global unsigned int* ps,
                                                       __global unsigned int* res,
                                                       __local unsigned int* localRes){

    size_t pos = get_global_id(0) * ITERATOR;
    unsigned int firstPS = ps[get_group_id(0)*get_local_size(0)*ITERATOR];

    for(int i=0;i < ITERATOR;i++){
        unsigned int insertPos = ps[pos + i] - firstPS;
        atomic_add(&localRes[insertPos],1);
    }

}