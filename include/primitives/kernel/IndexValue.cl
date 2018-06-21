__kernel void _kernel_index_value(__global const int *_sorted_element,
                                __global int *_index_arr,
                                __global int *_offset_arr) {

    uint mIndex, mI;
    uint mGlobalId = get_global_id(0);

    __local uint mLocalIndexArr[ARR_SIZE];
    __local uint mLocalOffsetArr[OFFSET_SIZE];

    mIndex = mGlobalId * ITERATOR;

    for(int j = 0; j < ITERATOR; j++) {
        mI = j + mIndex;

        if(mI == 0) {
            mLocalIndexArr[mI] = 0;   //printf("\n IF \t IndexArr[%d] :: %d \t GLOBAL_ID :: %d \t ELEMENT[%d] :: %d", mI, mLocalIndexArr[mI], mGlobalId, mI, _sorted_element[mI]);
        }
        else {
            mLocalIndexArr[mI] = _sorted_element[mI] == _sorted_element[mI - 1] ? 0 + mLocalIndexArr[mI - 1] : 1 + mLocalIndexArr[mI - 1];    //printf("\n ELSE \t IndexArr[%d] :: %d \t GLOBAL_ID :: %d \t ELEMENT[%d] :: %d", mI, mLocalIndexArr[mI], mGlobalId, mI, _sorted_element[mI]);
        }

        // Stores offset values
        if(j == ITERATOR - 1) {
            atomic_add(&mLocalOffsetArr[mGlobalId], mLocalIndexArr[mI]);     //mLocalOffsetArr[mGlobalId] = mLocalIndexArr[mI];

            for(int k = mGlobalId + 1; k < OFFSET_SIZE; k++) {
                atomic_add(&mLocalOffsetArr[k], mLocalIndexArr[mI]);    //mLocalOffsetArr[k] += mLocalIndexArr[mI];
            }   //printf("\nOFFSET[%d] :: %d, %d", mGlobalId, mLocalOffsetArr[mGlobalId], mI);

            barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

            // Stores the offset values back to global memmory
            _offset_arr[mGlobalId] = mLocalOffsetArr[mGlobalId];    //printf("\nGLOBAL OFFSET[%d] :: %d, %d", mGlobalId, _offset_arr[mGlobalId], mI);
        }

        // Strores the prefix_sum value back to global memory
        _index_arr[mI] = mLocalIndexArr[mI];     //printf("\n GLOBAL IndexArr[%d] :: %d \t GLOBAL_ID :: %d \t ELEMENT[%d] :: %d", mI, _index_arr[mI], mGlobalId, mI, _sorted_element[mI]);
    }
}