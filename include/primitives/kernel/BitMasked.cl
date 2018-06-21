__kernel void _kernel_bit_masked(__global const uint *_elements,
                                 __global uint *_bit_masked) {

    uint mIndex, mI;
    uint mGlobalId = get_global_id(0);

    mIndex = mGlobalId * ITERATOR;

    for(int j = 0; j < ITERATOR; j++) {
        mI = j + mIndex;

        if(mI == 0) {
            _bit_masked[mI] = 1;
        } else {
            _bit_masked[mI] = (_elements[mI] == _elements[mI - 1]) ? 0 : 1;
        }

        //printf("\n%d. GLOBAL ID :: %d \t ELEMENT[%d] :: %d \t BIT_MASKED[%d] :: %d ", mI, mGlobalId, mI, _elements[mI], mI, _bit_masked[mI]);
        barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
    }
}