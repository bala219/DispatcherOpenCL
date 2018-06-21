__kernel void _kernel_aggregate(__global const int *_index_arr,
                                __global const int *_offset,
                                __global int *_agg_res) {

    uint mIndex, mI, mAggResIndex; //, mOffsetIndex;
    uint mGlobalId = get_global_id(0);

    __local uint mLocalAggRes[RES_SIZE];

    mIndex = mGlobalId * ITERATOR;

    for(int j = 0; j < ITERATOR; j++) {
        mI = j + mIndex;

        if(mIndex > 0) { //mOffsetIndex = mGlobalId - 1;
            mAggResIndex = _offset[mGlobalId - 1] + _index_arr[mI];
            atomic_add(&mLocalAggRes[mAggResIndex], 1);   //printf("\n IF GLOBAL ID :: %d \t M_INDEX :: %d \t _I :: %d \t AGG_RES_INDEX :: %d \t INDEX_ARR[%d] :: %d \t ", mGlobalId, mIndex, mI, mAggResIndex, mI, _index_arr[mI]);
        } else {
            mAggResIndex = _index_arr[mI];
            atomic_add(&mLocalAggRes[mAggResIndex], 1);   //printf("\n ELSE GLOBAL ID :: %d \t M_INDEX :: %d \t _I :: %d \t INDEX_ARR[%d] :: %d \t ", mGlobalId, mIndex, mI, mI, _index_arr[mI]);
        }

        _agg_res[mAggResIndex] = mLocalAggRes[mAggResIndex];
    }
}