__kernel void He_method(__global unsigned int* PS, __global unsigned int* offlist, __global unsigned int* res, __global unsigned int* temp){



    //For each of the group in prefix_sum
    unsigned int tid = get_global_id(0);
    unsigned int local_thread_size = get_local_size(0);
    unsigned int global_size = get_global_size(0);
    unsigned int lastPS = PS[global_size - 1];
    __local unsigned int cacheMem[LOCAL_DATA_SIZE];

    //initialize results with 1
    for(int i =0;i<LOCAL_DATA_SIZE;i++)
        temp[i] = 1;

        //iterate over each of the group index
        for(unsigned int PSidx = 0; PSidx<=lastPS ; PSidx++){

            //Perform aggregation for current group
            if(PS[tid] == PSidx){

                unsigned int startTID =  (PSidx==0)?0: offlist[PSidx - 1]+1;
                unsigned int endTID = (PSidx==lastPS)?(global_size -1) : offlist[PSidx];
                int delta = 2;

                while(delta <= endTID - startTID +1){

                    size_t offset = delta*(tid - startTID);
                    if(offset < endTID - startTID){

                            temp[offset + startTID] = temp[offset + startTID] + temp[offset  + startTID + delta/2];
                           barrier(CLK_GLOBAL_MEM_FENCE);
                    }
                    delta = delta * 2;
                    barrier(CLK_GLOBAL_MEM_FENCE);
                }
               barrier(CLK_GLOBAL_MEM_FENCE);
                if(tid == startTID)
                    printf("\n%d:%d",PSidx,temp[startTID]);
            }
        }
        barrier(CLK_GLOBAL_MEM_FENCE);

/*    //Print result
    if(get_global_id(0) == 0){

        for(int i =0;i<DATA_SIZE;i++)
            printf("\n%d:temp[%d]",temp[i],i);

        for(int i =0;i<lastPS+1;i++)
                printf("\n%d:res[%d]",res[i],i);
    } */
}


/*    //For each of the group in prefix_sum
    unsigned int tid = get_global_id(0);
    unsigned int local_thread_size = get_local_size(0);
    unsigned int global_size = get_global_size(0);
    unsigned int lastPS = PS[global_size - 1];
    __local unsigned int cacheMem[LOCAL_DATA_SIZE];

    cacheMem[tid] = 1;
    barrier(CLK_LOCAL_MEM_FENCE);
    //iterate over each of the group index
    for(unsigned int PSidx = 0; PSidx<=lastPS ; PSidx++){

        //Perform aggregation for each of the group
        if(PS[tid] == PSidx){

            unsigned int startTID =  (PSidx==0)?0: offlist[PSidx - 1]+1;
            unsigned int endTID = (PSidx==lastPS)?(global_size -1) : offlist[PSidx];
            short local_chunks = 1 + ((endTID - startTID +1)/LOCAL_DATA_SIZE);

            for(short l = 0; l<local_chunks;l++){

                if(tid >= startTID + l*LOCAL_DATA_SIZE && tid < startTID + (l+1)*LOCAL_DATA_SIZE){

                    //here comes parallel reduce
                    int delta = 2;
                    while(delta < LOCAL_DATA_SIZE){

                        size_t offset = delta*(tid - startTID);
                        cacheMem[offset + startTID] = cacheMem[offset + startTID] + cacheMem[offset  + startTID + delta/2];
                        printf("%d:%d\n",cacheMem[offset + startTID],cacheMem[offset + startTID + delta/2]);
                        delta *=2;
                        barrier(CLK_LOCAL_MEM_FENCE);
                    }
                }
                barrier(CLK_LOCAL_MEM_FENCE);
            }

            if(tid == startTID)
                printf("%d:%d\n",PS[tid],cacheMem[tid]);
            barrier(CLK_GLOBAL_MEM_FENCE);
        }
    }

    //Print result
    if(get_global_id(0) == 0)
        for(int i =0;i<lastPS+1;i++)
                printf("\n%d:res[%d]",res[i],i);
*/