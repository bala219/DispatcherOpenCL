void kernel branched_aggregate(
        global const int* PS,
//        global int* BM,
        global int* res
) {

int local_res=1;

size_t pos = get_global_id(0) * ITERATOR;

for (unsigned int i = 0;i < ITERATOR-1; ++i){

if((PS[pos + i] - PS[pos + i + 1])){
    atomic_add(&res[PS[pos + i]],local_res);
    local_res=1;
}
else
local_res++;
}

atomic_add(&res[PS[pos + ITERATOR - 1]],local_res);
}