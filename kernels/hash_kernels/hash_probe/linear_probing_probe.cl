__kernel void linear_probing_probe(__global unsigned int *input,
                             __global unsigned int *slots,
                             __global unsigned int *hash_table,
                             __global unsigned int *result) {

    uint TID = get_global_id(0)*THREAD_SIZE;
    for(int i=0;i<THREAD_SIZE;i++){
        uint curr_val = TID + i;
        short slot = slots[curr_val];
        short initial_slot = slot;

        do{
            if(hash_table[slot + TID ] == input[curr_val]){
                result[curr_val] = slot + TID;
                break;
            }
            slot = (++slot) % TABLE_SIZE;
        } while(slot !=initial_slot);
    }

}