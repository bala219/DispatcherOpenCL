void kernel private_array_sequential_aggregate_GPU(
        global int* PS,
        global int* res
){

int local_ps[ITERATOR];
int local_res[ITERATOR];

size_t pos = get_global_id(0);
int lc = 0;

local_ps[lc] = PS[pos];
local_res[lc] = 1;

for (unsigned int i = 0;i < ITERATOR-1; ++i){

    if((PS[pos] - PS[pos + 1])){
        lc++;
        local_ps[lc] = PS[pos + 1];
        local_res[lc] =1;
    }
    else
        local_res[lc]++;
}

atomic_add(&res[local_ps[0]],local_res[0]);

for(int i = 1;i<lc;i++){
    atomic_add(&res[local_ps[i]],local_res[i]);
//printf("%d\n",local_res[i]);
}

//if(lc>0)
//    atomic_add(&res[local_ps[lc]],local_res[lc]);

}