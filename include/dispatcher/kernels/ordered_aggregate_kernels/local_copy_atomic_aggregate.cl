__kernel void local_copy_atomic_aggregate( const __global unsigned int* ps,
                                __global unsigned int* res){

    __local unsigned int local_ps[32];
    event_t e;

    async_work_group_copy(&local_ps[get_local_id(0)], &ps[get_global_id(0)],1,e);
    wait_group_events(1, &e);

    atomic_add(&res[ps[get_global_id(0)]],1);
}