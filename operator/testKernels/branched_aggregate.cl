void kernel branched_aggregate(
        global unsigned int* PS,
        global unsigned int* res
){

    unsigned int local_res=1;
    size_t pos = (size_t)(get_global_id(0) * ITERATOR);

    for (unsigned int i = 0;(i < ITERATOR-1)&&(pos + i<DATA_SIZE); ++i){

        if(PS[pos + i] != PS[pos + i + 1]){
            atomic_add(&res[PS[pos + i]],local_res);
            local_res=1;
        }
        else
            local_res++;
    }

    size_t final_position = (size_t)(pos + ITERATOR - 1);
    size_t finalPS = PS[final_position];
    unsigned int old_value = atomic_add(&res[finalPS],local_res);
}
