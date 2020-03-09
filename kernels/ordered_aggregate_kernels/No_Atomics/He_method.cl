__kernel void He_method(__global unsigned int* PS, __global unsigned int* offlist, __global unsigned int* res){


    //For each of the group in prefix_sum
    unsigned int PSidx = 0;
    unsigned int global_size = get_global_size(0);
    unsigned int tid = get_global_id(0);
    unsigned int lastPS = PS[global_size - 1];

    //iterate over each of the group index
    for(; PSidx<=lastPS ; PSidx++){

        //Perform aggregation for current group
        if(PS[tid] == PSidx){

            unsigned int endTID = offlist[PSidx];Y
            unsigned int startTID = PSidx==0?0:offlist[PSidx-1];

//            printf("\n%d:%d:%d",PSidx,startTID,endTID);

            int delta=2;
            while(delta <= endTID - startTID){

                size_t offset = delta*(tid - startTID);
                if(offset < endTID - startTID){

                    if(res[offset + startTID] == 0)
                        res[offset + startTID]++;
                    else
                        res[offset + startTID] = res[offset + startTID] + res[offset  + startTID + delta/2];
                }
                delta = delta * 2;
                barrier(CLK_GLOBAL_MEM_FENCE);
            }
            barrier(CLK_GLOBAL_MEM_FENCE);
            res[PSidx] = res[startTID];

        }
        barrier(CLK_GLOBAL_MEM_FENCE);
    }

    //print results
/*    if(get_global_id(0) == 0){

        for(PSidx = 0; PSidx<=lastPS ; PSidx++){

            printf("\n%d:%d",PSidx,res[PSidx]);
        }
    } */
}