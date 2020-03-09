void kernel branched_aggregate_GPU(
        global const int* PS,
        global int* res
) {

int local_res=1;

size_t pos = get_global_id(0);

for (unsigned int i = 0;i < ITERATOR-1; ++i){

    if((PS[pos] - PS[pos + 1])){
        atomic_add(&res[PS[pos]],local_res);
        local_res=1;
    }
    else
        local_res++;

    pos+=get_global_size(0);
}

atomic_add(&res[PS[pos]],local_res);
}