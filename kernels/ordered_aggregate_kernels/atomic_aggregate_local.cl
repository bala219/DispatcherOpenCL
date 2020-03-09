__kernel void atomic_aggregate_local(  __global unsigned int* ps,
                                 __global unsigned int* res,
                                 __local unsigned int* localRes){


    /*
     * Initialize local memory and required parameter
     */

    size_t pos = get_local_id(0);
    localRes[pos] = 0;
    barrier(CLK_LOCAL_MEM_FENCE);

    unsigned int firstPS,lastPS;
    firstPS = ps[get_group_id(0)*get_local_size(0)];
    lastPS = ps[((get_group_id(0)+1)*get_local_size(0)) - 1];


    /*
     * Variant processing step for aggregation
     */

    int insertPos = ps[get_global_id(0)] - firstPS;
    atomic_add(&localRes[insertPos],1);
    barrier(CLK_GLOBAL_MEM_FENCE);

    /*
     * Push results to global memory from local
     */

    //Fill in first and last position
    if(get_local_id(0) == 0){
            atomic_add(&res[firstPS],localRes[get_local_id(0)]);

//        if(lastPS - firstPS > 1){
//            for (int i = 1; i < lastPS - firstPS; i++)
//                res[firstPS + i] += localRes[i];
//        }
        return;
    }

    else if(get_local_id(0) == get_local_size(0) - 1){

        if(lastPS!=firstPS)
            atomic_add(&res[lastPS],localRes[lastPS - firstPS]);
        return;
    }
    if(lastPS - firstPS <= 1)
        return;
    else if(get_local_id(0) < lastPS - firstPS)
        res[firstPS + get_local_id(0)] += localRes[get_local_id(0)];
}