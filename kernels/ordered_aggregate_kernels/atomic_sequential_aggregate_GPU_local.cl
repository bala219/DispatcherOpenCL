__kernel void atomic_sequential_aggregate_GPU(  __global unsigned int* ps,
                                 __global unsigned int* res){

    /*
     * Initialize local memory and required parameter
     */

//    size_t pos = get_local_id(0);
//    for(int i =0;i<ITERATOR;i++)
//        localRes[pos+i]=0;

    barrier(CL_KERNEL_LOCAL_MEM_SIZE);

    unsigned int firstPS,lastPS;
    firstPS = ps[get_group_id(0)*get_local_size(0)*ITERATOR];
    lastPS = ps[((get_group_id(0)+1)*get_local_size(0)*ITERATOR) - 1];

    pos = get_global_id(0);

    for(short i =0;i<DATA_SIZE/get_global_size(0);i++){

        atomic_add(&res[ps[pos]],1);
        pos+=(get_global_size(0));
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    /*
     * Push results to global memory from local
     */

    //Fill in first and last position
    if((get_local_id(0) == 0)){

        atomic_add(&res[ps[get_global_id(0)*ITERATOR]],localRes[get_local_id(0)]);
    }
    if((get_local_id(0)==get_local_size(0)-1)&&(lastPS!=firstPS)){

        atomic_add(&res[ps[get_global_id(0)*ITERATOR + ITERATOR - 1]],localRes[get_local_id(0) + ITERATOR -1]);
    }

    //check if the last and first position are not next or equal to each other
    if(lastPS - firstPS > 1){

        //Copy all other values
        event_t event1,event2;

        int num_of_elements = lastPS - firstPS - 1; //since 0 initialized removes 1 inherent value and two atomics are being used in the top.
        // One more removed to make the right set

        event1 = async_work_group_copy(&res[firstPS + 1],&localRes[1],num_of_elements);
        wait_group_events(1, &event1);
    }
}