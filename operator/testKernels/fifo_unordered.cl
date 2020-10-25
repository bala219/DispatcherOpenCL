void kernel fifo_unordered(
        global unsigned int* PS,
        global unsigned int* res
){

    unsigned int local_res=1;
    size_t pos = (size_t)(get_global_id(0) * ITERATOR);
	int PS_cache[ITERATOR];
	int payload_cache[ITERATOR];
	//variables to flag hit cases and count them
	bool hit = false;
	int hitCount = 0;

    //initialize the values
	for(int i =0;i<ITERATOR;i++){

		PS_cache[i] = 0;
		payload_cache[i] = 0;
	}

    //read a block of data size not exceeding ITERATOR from global memory and perform caching operations
    for (unsigned int i = pos;(i < pos + ITERATOR)&&(i < pos + DATA_SIZE); i++){
    hit = false;

        for(unsigned int j=0; j < ITERATOR; j++ )
        {
             if(PS_cache[j] == PS[i])
                            {
                                //std::cout<<"\n Hit"<<"\t"<<PS[i];
                                hit = true;
                                hitCount++;
                                payload_cache[j]++;
                                break;
                            }
        }

         if(!hit)
                    {
                        //std::cout<<"\n Miss"<<"\t"<<PS[i];
                        atomic_add(&res[PS_cache[(i-hitCount)%ITERATOR]],payload_cache[(i-hitCount)%ITERATOR]);
                        PS_cache[(i-hitCount)%ITERATOR]=PS[i];
                        payload_cache[(i-hitCount)%ITERATOR]=1;
                    }
    }


	//add the remaining results into global memory
	for(int i =0;(i<ITERATOR);i++){
		if(payload_cache[i])
			atomic_add(&res[PS_cache[i]],payload_cache[i]);
	}
}
