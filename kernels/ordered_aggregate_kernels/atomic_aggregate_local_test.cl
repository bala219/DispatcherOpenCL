__kernel void atomic_aggregate_local_test(  __global unsigned int* ps,
                           __global unsigned int* res,
                           __local unsigned int* localRes){

        unsigned int firstPS = ps[get_group_id(0)*get_local_size(0)];
        atomic_add(&localRes[ps[get_global_id(0)] - firstPS],1);
}