__kernel void branched_aggregate_local_test(  __global unsigned int* ps,
                                                  __global unsigned int* res,
                                                  __local unsigned int* localRes){


    /*
     * Initialize local memory and required parameter
     */

    unsigned int firstPS,lastPS;
    firstPS = ps[get_group_id(0)*get_local_size(0)*ITERATOR];
    lastPS = ps[((get_group_id(0)+1)*get_local_size(0)*ITERATOR) - 1];

    /*
     * Variant processing step for aggregation
     */

    int local_res = 1;
    int pos =  get_global_id(0) * ITERATOR;
    for(short i = 0;i< ITERATOR-1;i++){

        if(ps[pos + i] - ps[pos + i + 1]){

            int insertPos = ps[pos +i] - firstPS;
            atomic_add(&localRes[insertPos],local_res);
            local_res = 1;
        }
        else
            local_res++;
    }

    int insertPos = ps[pos + ITERATOR - 1] - firstPS;
    atomic_add(&localRes[insertPos],local_res);
}