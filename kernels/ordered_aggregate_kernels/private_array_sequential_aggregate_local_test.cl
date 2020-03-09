void kernel private_array_sequential_aggregate_local_test(
        __global unsigned int* PS,
        __global unsigned int* res,
        __local int* localRes
){

/*
 * Initialize local memory and required parameter
 */
size_t pos = get_local_id(0)*ITERATOR;

unsigned int firstPS, lastPS;
firstPS = PS[get_group_id(0) * get_local_size(0) * ITERATOR];
lastPS = PS[((get_group_id(0) + 1) * get_local_size(0) * ITERATOR) - 1];

int localFirst, localLast;
int local_res[ITERATOR];


pos = get_global_id(0) * ITERATOR;
int lc = 0;

local_res[lc] = 1;

localFirst = PS[pos];
localLast = PS[pos + ITERATOR - 1];

for (unsigned int i = 0;i<ITERATOR - 1;++i){

    if((PS[pos + i] - PS[pos + i + 1])){

        lc++;
        local_res[lc] =1;
    }

    else
        local_res[lc]++;
}

int insertPos = localFirst - firstPS;
atomic_add(&localRes[insertPos], local_res[0]);
}