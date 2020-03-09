void kernel intra_warp_ps(
        global const int* input
){
    int pos = get_global_id(0);
    for(int i = 0; i<32;i++){
        int val;
        if(pos>=i){
            val = input[pos - i];
            barrier(CLK_LOCAL_MEM_FENCE);
        }
        if(pos>=i){
            input[i] = val + input[i];
            barrier(CLK_LOCAL_MEM_FENCE);
        }

    }
}