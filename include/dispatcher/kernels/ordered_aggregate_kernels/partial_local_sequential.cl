void kernel partial_local_sequential(
        global int* PS,
        global int* res
){

__local int local_ps[ITERATOR*ITERATOR];
__local int local_res[ITERATOR*ITERATOR];

// Initalize local memory with 0s

size_t local_pos = get_local_id(0);

for(int i=0; i<ITERATOR; i++){
local_ps[i + local_pos] = 0;
local_res[i + local_pos]= 0;
}

barrier(CLK_LOCAL_MEM_FENCE);

//Perform aggregation on work item

unsigned int l_res = 1;

for(int i = 0; i<ITERATOR - 1; i++){

    if(PS[local_pos + i] - PS[local_pos + i + 1]){
        atomic_add(&local_res[PS[local_pos + i]],l_res);
        l_res = 1;
    }
    else
        l_res++;
}

//Atomic add first and last position in the whole workgroup --> first : WI[0][0] and last : WI[ITERATOR][ITERATOR]

    //Atomic add first postion using first local ID
    if(get_local_id(0)==0){

        atomic_add(&res[local_ps[0]],local_res[0]);
}
    else if(get_local_id(0)== get_local_size(0) -1){

        atomic_add(&res[local_ps[ITERATOR - 1]],local_res[ITERATOR - 1]);
}

mem_fence(CLK_LOCAL_MEM_FENCE);

//using async workgroup copy

event_t event;

const __local int* src = &local_res[1];

int num_of_elements = PS[((get_group_id(0)+1)*32) - 2] - PS[get_group_id(0)*32];

async_work_group_copy(&res[get_group_id(0)*32],&local_res[1],num_of_elements,event);
    wait_group_events(1, &event);
}