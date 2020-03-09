__kernel void atomic_agg(  __global unsigned int* ps,
                           __global unsigned int* res)

{
    atomic_add(&res[ps[get_global_id(0)]],1);
//    atomic_add(&res[ps[get_global_id(0)+1]],1);
//    atomic_add(&res[ps[get_global_id(0)+2]],1);
//    atomic_add(&res[ps[get_global_id(0)+3]],1);
}


//  Atomic max kernel

__kernel void atomic_agg(  __global unsigned int* ps,
                           __global unsigned int* res)
{
    atomic_max(&res[ps[get_global_id(0)]],1);
}