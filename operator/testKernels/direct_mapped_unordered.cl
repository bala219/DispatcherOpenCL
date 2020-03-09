void kernel direct_mapped_unordered(
        global unsigned int* PS,
        global unsigned int* res
){

    unsigned int local_res=1;
    size_t pos = (size_t)(get_global_id(0) * ITERATOR);
	int PS_cache[ITERATOR];
	int payload_cache[ITERATOR];
	
	for(int i =0;i<ITERATOR;i++){

		PS_cache[i] = 0;
		payload_cache[i] = 0;
	}
	
    for (unsigned int i = 0;(i < ITERATOR)&&(pos + i<DATA_SIZE); ++i){
		
		//store the value within the given space using hashing algorithm
		int hash_pos = PS[i]%ITERATOR;
		//increment the corresponding payload
		//if no values present
		if(PS_cache[hash_pos]==0){
		
			PS_cache[hash_pos] = PS[pos+i];
			payload_cache[hash_pos]++;
		}
		else if(PS_cache[hash_pos]==PS[pos+i])
			payload_cache[hash_pos]++;		
		//if the group name within the input is different than the current one - evict and do atomic on 
		else{
			
				atomic_add(&res[PS_cache[hash_pos]],payload_cache[hash_pos]);		
				PS_cache[hash_pos] = PS[pos+i];
				payload_cache[hash_pos]=1;
		}
		
    }
	
	//add the remaining results into global memory
	for(int i =0;(i<ITERATOR)&&(pos + i<DATA_SIZE);i++){
		if(payload_cache[i])
			atomic_add(&res[PS_cache[i]],payload_cache[i]);
	}
}
