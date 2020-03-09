__kernel void _kernel_bitonic_sort_position(__global int* _elements, __global int* poslist, const uint _stages) {

    uint mIndex1, mIndex2;
    uint mGlobalId = get_global_id(0);
    uint ARR_SIZE = 512;

    __local int mLocalArr[512];
    __local int mLocalPosArr[512];

    mLocalArr[mGlobalId] = _elements[mGlobalId];
    mLocalPosArr[mGlobalId] = poslist[mGlobalId];

    mLocalArr[mGlobalId + ARR_SIZE/2] = _elements[mGlobalId + ARR_SIZE/2];
    mLocalPosArr[mGlobalId + ARR_SIZE/2] = poslist[mGlobalId + ARR_SIZE/2];

    barrier(CLK_LOCAL_MEM_FENCE);

    // PHASE 1. Making bitonic sequences
    for (uint _stage = 1; _stage <= _stages; _stage++) {

        uint _m_signo = (mGlobalId >> (_stage - 1)) % 2;
        for (uint _passOfStage = _stage; _passOfStage > 0; _passOfStage--) {

            uint _targetElements = 1 << (_passOfStage-1);

            uint mIndex1 = (mGlobalId >> (_passOfStage-1));
            mIndex1 = mIndex1 << _passOfStage;
            mIndex1 = mIndex1 + (mGlobalId & (_targetElements - 1));

            uint mIndex2 = mIndex1 + _targetElements;

            if ((mLocalArr[mIndex1] > mLocalArr[mIndex2]) ^ (_m_signo)) {
                int aux = mLocalArr[mIndex1];
                int p = mLocalPosArr[mIndex1];
                mLocalArr[mIndex1] = mLocalArr[mIndex2];
                mLocalPosArr[mIndex1] = mLocalPosArr[mIndex2];
                mLocalArr[mIndex2] = aux;
                mLocalPosArr[mIndex2] = p;
            }

            barrier(CLK_LOCAL_MEM_FENCE);

        }
    }
    _elements[mGlobalId] = mLocalArr[mGlobalId];
    poslist[mGlobalId] = mLocalPosArr[mGlobalId];

    _elements[mGlobalId + ARR_SIZE/2] = mLocalArr[mGlobalId + ARR_SIZE/2];
    poslist[mGlobalId + ARR_SIZE/2] = mLocalPosArr[mGlobalId + ARR_SIZE/2];
}