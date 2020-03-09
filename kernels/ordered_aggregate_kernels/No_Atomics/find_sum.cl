__kernel void find_sum.cl(__global int* input, __local int* output,__local int* local_cache){

      int global_id = get_global_id(0);
      int local_id = get_local_id(0);
      int local_size = get_local_size(0);

      //Copy values into local memory
      local_cache[local_id] = input[id];

      //Wait for all threads to catch up
      barrier(CLK_LOCAL_MEM_FENCE);

      for (int i = local_size/2; i > 0; i /= 2){
        if(local_id < i){
          local_cache[local_id] += local_cache[local_id + i];
        }
        //Wait for all threads to catch up
        barrier(CLK_LOCAL_MEM_FENCE);
      }

      if (!local_id) {
        //Can use atomic add as we're dealing with ints
        atomic_add(&output[0], local_cache[local_id]);
      }
}