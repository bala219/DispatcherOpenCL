__kernel void atomic_local_aggregate_ordered(  __global unsigned int* ps,
                                               __global unsigned int* res){

    unsigned int tid = get_global_id(0);
    unsigned int local_thread_size = get_local_size(0);
    unsigned int tlid = get_local_id(0);
    __local unsigned int cacheMem[LOCAL_DATA_SIZE];

    //Specific to ordered case
    unsigned int firstPS = ps[get_group_id(0)*get_local_size(0)];
    cacheMem[tlid] = 0;
    barrier(CLK_LOCAL_MEM_FENCE);

   //local memory atomics
    atomic_add(&cacheMem[ps[get_global_id(0)] - firstPS],1);
    barrier(CLK_LOCAL_MEM_FENCE);


unsigned int lastPS = ps[(get_group_id(0)+1)*get_local_size(0) - 1] ;
if(get_local_id(0) == firstPS || get_local_id(0) == lastPS )
    atomic_add(&res[ps[get_global_id(0)]],cacheMem[get_local_id(0)]);

else if(firstPS < get_local_id(0) && get_local_id(0) < lastPS)
    res[ps[get_global_id(0)]] = cacheMem[get_local_id(0)];

barrier(CLK_LOCAL_MEM_FENCE);

/*
    if(get_local_id(0) <= - firstPS){
        atomic_add(&res[ps[get_global_id(0)]],cacheMem[get_local_id(0)]);
    }

    if(get_local_id(0) == 0){

        for(short i=0;i<ps[(get_group_id(0)+1)*get_local_size(0) - 1] - firstPS;i++)
            atomic_add(&res[ps[get_global_id(0)]],cacheMem[i]);
    } */
}