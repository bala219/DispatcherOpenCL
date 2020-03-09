__kernel void _kernel_index_value(__global const unsigned int *_sorted_element,
                                  __global unsigned int *_index_arr,
                                  __global unsigned int *_offset_arr) {


//    for(int i=0;i<OFFSET_SIZE;i++){
//        _offset_arr[i]=0;
//    }
//
//    barrier(CLK_GLOBAL_MEM_FENCE);

    uint mIndex, mI, mLastOffsetVal, mPrevIndexVal;
    uint mGlobalId = get_global_id(0);

    mIndex = mGlobalId * ITERATOR;


    // Stores Index Array Value

    if(mIndex!=0){

        _index_arr[mIndex] = (_sorted_element[mIndex] == _sorted_element[mIndex - 1])?0:1;
    }
    else
        _index_arr[mIndex] = 0;

    for(int j = 1; j < ITERATOR; j++) {

        mI = j + mIndex;

        mPrevIndexVal = (mI == mIndex) ? 0 : _index_arr[mI - 1];
        _index_arr[mI] = (mI == 0) ? mI : !(_sorted_element[mI] == _sorted_element[mI - 1])  + mPrevIndexVal;
        mLastOffsetVal = _index_arr[mI];
    }

//    printf("%d : last val\n",mLastOffsetVal);

    for(uint k = mGlobalId; k < OFFSET_SIZE; k++) {
        atomic_add(&_offset_arr[k], mLastOffsetVal);
    }


}