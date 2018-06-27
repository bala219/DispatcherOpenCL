__kernel void _kernel_aggregate(__global const int *_index_arr,
                                __global const int *_offset,
                                __global int *_agg_res) {

    __local uint mIndex, mI, mAggResIndex;
    uint mGlobalId = get_global_id(0);

    mIndex = mGlobalId * ITERATOR;

    // For Calculating the Aggregate Of the Array
    for(int j = 0; j < ITERATOR; j++) {

        mI = j + mIndex;
        mAggResIndex = _index_arr[mI] + _offset[mGlobalId - 1];
        atomic_add(&_agg_res[mAggResIndex], 1);

        //mAggResIndex = (mI == 0) ? _index_arr[mI] : _index_arr[mI] + _offset[mGlobalId - 1];
        //barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
        //printf("\n GLOBAL ID :: %d \t M_INDEX :: %d \t _I :: %d \t INDEX_ARR[%d] :: %d \t ", mGlobalId, mIndex, mI, mI, _index_arr[mI]);
    }
}