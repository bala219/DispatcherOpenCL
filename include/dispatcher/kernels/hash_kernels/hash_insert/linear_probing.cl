__kernel void linear_probing(__global unsigned int *input,
                             __global unsigned int *slots,
                             __global unsigned int *hash_table) {

    uint TID = get_global_id(0)*THREAD_SIZE;

    for(int i=0;i<THREAD_SIZE;i++){

        uint curr_val = TID + i;
        short slot = slots[curr_val];
        short initial_slot = slot;

        do{
            if(!hash_table[slot + TID ]){
                hash_table[slot + TID ] = input[curr_val];
                break;
            }
            slot = (++slot) % TABLE_SIZE;
        } while(slot !=initial_slot);
    }
}