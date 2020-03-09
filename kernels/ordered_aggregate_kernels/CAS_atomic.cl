__kernel void CAS_atomic(  __global unsigned int* ps,
                           __global unsigned int* res,
                           __global unsigned int* lock){

         while(atomic_xchg(lock,1) !=0);
            res[ps[get_global_id(0)]]+=1;
        *lock = 0;
}