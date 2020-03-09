__kernel void branched_aggregate_local(
        global const int* PS,
        global const int* BM,
        global int* res,
        __local int* local_ps,
        __local int* local_res_ps,
        __local int* local_res
) {

size_t local_offset = get_local_id(0)*4;
size_t global_offset = get_global_id(0)*4;
event_t event;
async_work_group_copy(&local_ps[get_local_id(0)],&PS[global_offset],32,event);
//async_work_group_copy(&local_ps[local_offset],&PS[global_offset],4,event);

wait_group_events(1, &event);

//Start aggregation here

//PS: Last entry - first entry gives number of unique values : not possible in OpenCL
//int local_res_ps[local_ps[local_offset + 31] - local_ps[local_offset] + 1];
//int local_res[local_ps[local_offset + 31] - local_ps[local_offset] + 1];
//Compute local aggregates

int lc = 0;
local_res_ps[lc] = local_ps[local_offset];
local_res[lc] = 1;
for (unsigned int i = local_offset+1;i<local_offset+4; ++i){

    if((local_ps[i] - local_ps[i - 1])){
        lc++;
        local_res_ps[lc] = local_ps[i];
        local_res[lc] =1;
    }

    else
        local_res[lc]++;
}

//Store the local results back
for(int i = 0; i<=lc; i++){
    atomic_add(&res[local_res_ps[i]],local_res[i]);
}

}