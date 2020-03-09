__kernel void barrier_based(__global unsigned int* PS,__global unsigned int* res,unsigned int off,unsigned int ps_pos){

    //For each of the group in prefix_sum
    unsigned int tid = get_global_id(0);
    unsigned int local_thread_size = get_local_size(0);
    unsigned int global_size = get_global_size(0);
    __local unsigned int cacheMem[LOCAL_DATA_SIZE];
    unsigned int tlid = get_local_id(0);

//    cacheMem[tlid] = PS[off + tid];
    cacheMem[tlid] = 1;
    barrier(CLK_LOCAL_MEM_FENCE);

    //local intra-work group reduce
    int delta = 2;
    while(delta <= LOCAL_DATA_SIZE){

        size_t offset = delta *tlid;
        if(offset < LOCAL_DATA_SIZE)
            cacheMem[offset] = cacheMem[offset] + cacheMem[offset + delta/2];
        delta = delta*2;
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if(get_local_id(0) == 0){
        PS[off + get_group_id(0)] = cacheMem[tlid];
        res[ps_pos] = PS[off];
    }

}