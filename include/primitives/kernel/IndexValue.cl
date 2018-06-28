__kernel void _kernel_index_value(__global const int *_sorted_element,
                                __global int *_index_arr,
                                __global int *_offset_arr) {

    __local uint mIndex, mI, mLastOffsetVal, mPrevIndexVal;
    uint mGlobalId = get_global_id(0);

    mIndex = mGlobalId * ITERATOR;

    // Stores Index Array Value
    for(int j = 0; j < ITERATOR; j++) {

        mI = j + mIndex;

        mPrevIndexVal = (mI == mIndex) ? 0 : _index_arr[mI - 1];
        _index_arr[mI] = (mI == 0) ? mI : !(_sorted_element[mI] == _sorted_element[mI - 1])  + mPrevIndexVal;
        mLastOffsetVal = _index_arr[mI];
    }

    // Stores the Offset Array Values
    atomic_add(&_offset_arr[mGlobalId], mLastOffsetVal);
    for(int k = mGlobalId + 1; k < OFFSET_SIZE; k++) {
        atomic_add(&_offset_arr[k], mLastOffsetVal);
    }
}