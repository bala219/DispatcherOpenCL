__kernel void _kernel_aggregate(__global const int* _prefix_sum,
                                __global int* _res_arr) {

    uint mIndex, mPSIndex, mI;
    uint mGlobalId = get_global_id(0);
    __local uint mLocalResArr[UNIQUE_ELEMENT];

    mIndex = mGlobalId * ITERATOR;

    for(int j = 0; j < ITERATOR; j++) {
        mI = j + mIndex;
        mPSIndex = _prefix_sum[mI];
        atomic_add(&mLocalResArr[mPSIndex], 1);

        //printf("\nINDEX :: %d \t PS[%d] :: %d \t RES_ARR[%d] :: %d", mIndex, mIndex, _prefix_sum[mIndex], mPSIndex, _res_arr[mPSIndex]);
        //barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

        _res_arr[mPSIndex] = mLocalResArr[mPSIndex];

    }
}