__kernel void atomic_aggregate(  __global unsigned int* ps,
                           __global unsigned int* res){

        atomic_add(&res[ps[get_global_id(0)]],1);
}