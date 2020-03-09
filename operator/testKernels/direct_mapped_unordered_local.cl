void kernel direct_mapped_unordered_local(
        global unsigned int* PS,
        global unsigned int* res
){

    unsigned int local_res=1;
    size_t pos = (size_t)get_global_id(0);
	__local int payload_cache[ITERATOR];

	if(get_local_memory(0)==0){

        for(int i =0;i<ITERATOR/2;i++){

        		payload_cache[i] = -1;
        		payload_cache[i + ITERATOR/2] = 0;
        }
	}
    barrier(CLK_LOCAL_MEM_FENCE);

    //Using first half for position and second half for payload
    int hash_pos = PS[pos] % (ITERATOR/2);
    int payload_pos = (ITERATOR/2) + hash_pos;
}
