__kernel void branched_aggregate_local(  __global unsigned int* ps,
                                                  __global unsigned int* res,
                                                  __local unsigned int* localRes){


    /*
     * Initialize local memory and required parameter
     */

    size_t pos = get_local_id(0)*ITERATOR;
    for(int i =0;i<ITERATOR;i++)
        localRes[pos+i]=0;

    barrier(CLK_LOCAL_MEM_FENCE);

    unsigned int firstPS,lastPS;
    firstPS = ps[get_group_id(0)*get_local_size(0)*ITERATOR];
    lastPS = ps[((get_group_id(0)+1)*get_local_size(0)*ITERATOR) - 1];

    /*
     * Variant processing step for aggregation
     */

    int local_res = 1;
    pos =  get_global_id(0) * ITERATOR;
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

    barrier(CLK_LOCAL_MEM_FENCE);

    /*
     * Push results to global memory from local
     */

    //Fill in first and last position
    if(get_local_id(0)==0){

        atomic_add(&res[firstPS],localRes[0]);

//        if(lastPS - firstPS > 1){
//
//            for (int i = 1; i < lastPS - firstPS; i++)
//                res[firstPS + i] += localRes[i];
//
//        }
//        return;
    }

    else if((get_local_id(0)==get_local_size(0)-1)&&(lastPS-firstPS)){

        atomic_add(&res[ps[pos + ITERATOR - 1]],localRes[lastPS - firstPS]);
    }

    if(lastPS - firstPS <= 1)
        return;

    for(int i = get_local_id(0)*ITERATOR;(i<get_local_id(0)*ITERATOR + ITERATOR)
                                         &&(i < lastPS - firstPS); i++){

        if(get_local_id(0)==0&&i==0)
            continue;

        res[firstPS+i] += localRes[i];
    }
}