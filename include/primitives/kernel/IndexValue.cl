__kernel void _kernel_index_value(__global const int *_sorted_element,
                                __global int *_index_arr,
                                __global int *_offset_arr) {

    __local uint mIndex, mI, mLastOffsetVal;
    uint mGlobalId = get_global_id(0);

    mIndex = mGlobalId * ITERATOR;

    //printf("\nGLOBAL SIZE = %d \t LOCAL SIZE = %d", mGlobalId, get_local_id(0) );

    // For Index Array Value Calculation
    for(int j = 0; j < ITERATOR; j++) {

        mI = j + mIndex;

        //printf("\nGLOBAL_ID :: %d \t MINDEX :: %d \t J :: %d \t MI :: %d", mGlobalId, mIndex, j, mI);
        _index_arr[mI] = (mI == 0) ? mI : !(_sorted_element[mI] == _sorted_element[mI - 1])  + _index_arr[mI - 1];
        mLastOffsetVal = _index_arr[mI];
    }

    // Stores the Offset Values
    atomic_add(&_offset_arr[mGlobalId], mLastOffsetVal);
    for(int k = mGlobalId + 1; k < OFFSET_SIZE; k++) {
        atomic_add(&_offset_arr[k], mLastOffsetVal);
    }
}