void kernel private_array_sequential_aggregate_local(
        __global int* PS,
        __global int* res,
        __local int* localRes
){

/*
 * Initialize local memory and required parameter
 */
size_t pos = get_local_id(0)*ITERATOR;

for(int i = 0;i<ITERATOR;i++)
    localRes[pos+i]=0;

barrier(CLK_LOCAL_MEM_FENCE);

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

for(int i = 1;i<localLast - localFirst;i++){
localRes[insertPos + i] = local_res[i];
}

if(localLast - localFirst){

    int insertPos = localLast - firstPS;
    atomic_add(&localRes[insertPos], local_res[lc]);
}

barrier(CLK_LOCAL_MEM_FENCE);

/*
 * Push results to global memory from local
 */

//Fill in first and last position
if(get_local_id(0)==0){

    atomic_add(&res[firstPS],localRes[0]);

}

else if((get_local_id(0)==get_local_size(0)-1)&&(lastPS-firstPS)){
    atomic_add(&res[PS[pos + ITERATOR - 1]],localRes[lastPS - firstPS]);
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