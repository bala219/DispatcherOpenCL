#pragma OPENCL EXTENSION cl_khr_initialize_memory : enable

void kernel partial_atomic_aggregate(
        global int* PS,
        global int* res
){

__local int local_ps[ITERATOR];
__local int local_res[ITERATOR];

for(int i=0;i<ITERATOR;i++){
local_ps[i]=0;
local_res[i]=0;
}

barrier(CLK_LOCAL_MEM_FENCE);

//Get the last PS value
int first_index  = get_group_id(0)*ITERATOR;
int first_ps = PS[first_index];
int local_pos = PS[get_global_id(0)] - first_ps;

//if(get_group_id(0)==0){
//printf("%zd:%d\n",get_global_id(0),local_pos);
//}

atomic_add(&local_res[local_pos],1);
local_ps[local_pos] = PS[get_global_id(0)];

barrier(CLK_LOCAL_MEM_FENCE);

//Total values
size_t last_index = PS[(get_group_id(0)+1)*ITERATOR - 1];

if((get_local_id(0)==0)||(get_local_id(0)==last_index - first_index))
    atomic_add(&res[local_ps[get_local_id(0)]],local_res[get_local_id(0)]);
else if((get_local_id(0)<last_index - first_index && get_local_id(0)>0)&&(local_res[get_local_id(0)]!=0))
    res[local_ps[get_local_id(0)]]=local_res[get_local_id(0)];

}