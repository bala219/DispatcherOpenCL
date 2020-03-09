__kernel void barrier_reduce_atomics(__global int* PS, __global int* res,__local unsigned int* cacheMem){

    unsigned int tid = get_global_id(0);
    unsigned int local_thread_size = get_local_size(0);
    unsigned int global_size = get_global_size(0);

    unsigned int tlid = get_local_id(0);

    cacheMem[tlid] = PS[tid]==0?1:PS[tid];
    barrier(CLK_LOCAL_MEM_FENCE);

    //local intra-work group reduce
    for(int s=local_thread_size >> 1;s > 0; s >>=1){

        if(tlid < s){
            cacheMem[tlid] += cacheMem[tlid + s];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if(get_local_id(0) == 0)
        atomic_add(&res[PS[tid]], cacheMem[tlid]);
}