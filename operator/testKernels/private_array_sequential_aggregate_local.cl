void kernel private_array_sequential_aggregate_local(
        global int* PS,
        global int* res
){

    unsigned int tid = get_global_id(0);
    unsigned int local_thread_size = get_local_size(0);
    unsigned int tlid = get_local_id(0);
    __local unsigned int cacheMem[LOCAL_DATA_SIZE*ITERATOR];


    size_t localPos = tlid*ITERATOR;
    for(size_t i=0;i<ITERATOR;i++)
        cacheMem[localPos + i] = 0;
    barrier(CLK_LOCAL_MEM_FENCE);

    size_t pos = get_global_id(0) * ITERATOR;

    int private_pos=0;
    unsigned int private_res[ITERATOR];
    private_res[private_pos] = 1;
    for (unsigned int i = 0;(i < ITERATOR-1)&&(pos + i<DATA_SIZE); ++i){

        if(PS[pos + i] != PS[pos + i + 1]){
          private_pos++;
          private_res[private_pos] = 1;
        }
        else
          private_res[private_pos]++;
    }

    //Push private value results into the local array

    unsigned int cache_mem_first_pos = PS[get_group_id(0)*ITERATOR*get_local_size(0)];
    unsigned int cache_mem_last_pos = PS[((get_group_id(0)+1)*LOCAL_DATA_SIZE*ITERATOR)-1];

    unsigned int private_first_pos = PS[get_global_id(0)*ITERATOR];
    unsigned int private_last_pos = PS[((get_global_id(0)+1)*ITERATOR)-1];

    //First location atomics
    atomic_add(&cacheMem[private_first_pos - cache_mem_first_pos],private_res[0]);

    //Other results simply copied before last atomic
    for(unsigned int i = 1;i< private_last_pos - private_first_pos && i<DATA_SIZE;i++)
                   cacheMem[private_first_pos - cache_mem_first_pos + i] = private_res[i];

    //Last location atomics
    if(private_last_pos != private_first_pos)
        atomic_add(&cacheMem[private_last_pos - cache_mem_first_pos],private_res[private_last_pos - private_first_pos]);

    barrier(CLK_LOCAL_MEM_FENCE);

    if(get_local_id(0) == 0){
        //Perform atomics for first position
        atomic_add(&res[PS[get_global_id(0)]],cacheMem[0]);
        for(unsigned int i=PS[pos] + 1;i< cache_mem_last_pos && i<DATA_SIZE;i++)
               res[i]  = cacheMem[i-PS[pos]];

        if(cache_mem_last_pos != PS[get_global_id(0)])
            atomic_add(&res[cache_mem_last_pos],cacheMem[cache_mem_last_pos - cache_mem_first_pos]);
    }
}