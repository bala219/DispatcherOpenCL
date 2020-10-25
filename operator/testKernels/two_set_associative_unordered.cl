void kernel two_set_associative_unordered(
        global unsigned int* PS,
        global unsigned int* res
){

    unsigned int local_res=1;
    size_t pos = (size_t)(get_global_id(0) * ITERATOR);
	int PS_cache[ITERATOR];
	int payload_cache[ITERATOR];
	int payload_two_set_switch[ITERATOR/2];
	int next_pos = 0;

	for(int i =0,j=0;i<ITERATOR;i++){
		PS_cache[i] = 0;
		payload_cache[i] = 0;

		//if(i%2==0)
		//{
		//    payload_two_set_switch[j++] = 0;
		//}
	}

	for(int i =0;i<ITERATOR/2;i++){
		payload_two_set_switch[i] = 0;
	}

    for (unsigned int i = pos;(i < pos+ITERATOR)&&( i< pos+DATA_SIZE); i++){

		//store the value within the given space using hashing algorithm
		int hash_pos = PS[i]%(ITERATOR/2);

		if(PS_cache[hash_pos]==PS[i])
		{
		    payload_cache[hash_pos]++;
		}
		else if(PS_cache[hash_pos+1]==PS[i])
        {
        	payload_cache[hash_pos+1]++;
        }
		else
		{
            next_pos = payload_two_set_switch[hash_pos];
            //next_pos = i%2;
            atomic_add(&res[PS_cache[hash_pos+next_pos]],payload_cache[hash_pos+next_pos]);

            PS_cache[hash_pos+next_pos] = PS[i];
            payload_cache[hash_pos+next_pos]=1;
            payload_two_set_switch[hash_pos]=(next_pos==0)?1:0;
		}

    }

	//add the remaining results into global memory
	for(int i =0;(i<ITERATOR);i++){
		if(payload_cache[i])
			atomic_add(&res[PS_cache[i]],payload_cache[i]);
	}
}
