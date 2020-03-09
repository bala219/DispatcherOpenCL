__kernel void atomic_aggregate(  __global unsigned int* ps,
                           __global unsigned int* res){

//    for(size_t i = 0; i < DATA_SIZE/get_global_size(0) + 1;i++){
        atomic_add(&res[ps[get_global_id(0)]],1);
//    }

}