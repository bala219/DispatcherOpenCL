void kernel branched_aggregate_ordered(
        global unsigned int* PS,
        global unsigned int* res
) {

    unsigned int tid = get_global_id(0);
    unsigned int local_thread_size = get_local_size(0);
    unsigned int tlid = get_local_id(0);
    __local unsigned int cacheMem[LOCAL_DATA_SIZE*ITERATOR];
    unsigned int firstPS = PS[get_group_id(0)*local_thread_size*ITERATOR];

    size_t localPos = tlid*ITERATOR;
    for(size_t i=0;i<ITERATOR;i++)
        cacheMem[localPos + i] = 0;
    barrier(CLK_LOCAL_MEM_FENCE);

    int local_res=1;
    size_t pos = get_global_id(0) * ITERATOR;
    for (unsigned int i = 0;(i < ITERATOR-1)&&(pos + i<DATA_SIZE); ++i){

        if(PS[pos + i] != PS[pos + i + 1]){
          atomic_add(&cacheMem[PS[pos + i] - firstPS],local_res);
          local_res=1;
        }
        else
          local_res++;
    }

    atomic_add(&cacheMem[PS[pos + ITERATOR-1] - firstPS],local_res);

    barrier(CLK_LOCAL_MEM_FENCE);

    unsigned int lastPS = PS[((1 + get_group_id(0))*local_thread_size*ITERATOR) - 1];
    if(get_local_id(0)==0){

        //Perform atomics for first position
        atomic_add(&res[firstPS],cacheMem[0]);

        for(unsigned int i=firstPS + 1;i< lastPS && i<DATA_SIZE;i++)
               res[i]  = cacheMem[i-firstPS];

        if(lastPS != firstPS)  //Have to add in the last value only if it also not the first value
            atomic_add(&res[lastPS],cacheMem[lastPS - firstPS]);
    }
}