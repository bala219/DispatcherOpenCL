__kernel void _kernel_bitonic_sort(__global int* _elements, const uint _stages) {

    uint mIndex1, mIndex2;
	uint mGlobalId = get_global_id(0);
    __local int mLocalArr[ARR_SIZE];
    uint cond = -1;

	mLocalArr[mGlobalId] = _elements[mGlobalId];
	mLocalArr[mGlobalId + ARR_SIZE/2] = _elements[mGlobalId + ARR_SIZE/2];
	barrier(CLK_LOCAL_MEM_FENCE);

	// PHASE 1. Making bitonic sequences
	for (uint _stage = 1; _stage <= _stages; _stage++) {

		uint _m_signo = (mGlobalId >> (_stage - 1)) & 1;
		for (uint _passOfStage = _stage; _passOfStage > 0; _passOfStage--) {

			uint _targetElements = 1 << (_passOfStage-1);

			uint mIndex1 = (mGlobalId >> (_passOfStage-1));
			mIndex1 = mIndex1 << _passOfStage;
			mIndex1 = mIndex1 + (mGlobalId & (_targetElements - 1));
			//uint mIndex1 = ((mGlobalId >> (_passOfStage-1)) << _passOfStage) + (mGlobalId & (_targetElements - 1));

			uint mIndex2 = mIndex1 + _targetElements;

			//printf("\nGLOBAL ID :: %d\tSTAGE :: %d\tPASS :: %d\tSIGNO :: %d\tCONDITION1 :: %d\tCONDITION2 :: %d", mGlobalId, _stage, _passOfStage, (_m_signo), (mLocalArr[mIndex1] > mLocalArr[mIndex2]), (mLocalArr[mIndex1] > mLocalArr[mIndex2]) ^ (_m_signo));

			if ((mLocalArr[mIndex1] > mLocalArr[mIndex2]) ^ (_m_signo)) {
			    cond = 1;
				int aux = mLocalArr[mIndex1];
				mLocalArr[mIndex1] = mLocalArr[mIndex2];
				mLocalArr[mIndex2] = aux;
			}
			//else
			    //cond = 0;

            //printf("\nLOOP :: %d \tSTAGE :: %d\tPASS_OF_STAGE :: %d \t(ID :: %d \tTARGET_ELEMENTS :: %d , \tSIGNO :: %d) \tINDEX1 :: %d \tINDEX2 :: %d \tARRAY[%d] :: %d \t\tARRAY[%d] :: %d", cond, _stage, _passOfStage, mGlobalId, _targetElements, _m_signo, mIndex1, mIndex2, mIndex1, mLocalArr[mIndex1], mIndex2, mLocalArr[mIndex2]);

			//barrier(CLK_LOCAL_MEM_FENCE);

			/*if(mIndex2 == ARR_SIZE-1) {
	            printf("\n---------------------------------------------------------------------");
			}
			if(_passOfStage == 1) {
			    printf("\n---------------------------------------------------------------------");
			}*/
		}
	}
	_elements[mGlobalId] = mLocalArr[mGlobalId];
	_elements[mGlobalId + ARR_SIZE/2] = mLocalArr[mGlobalId + ARR_SIZE/2];
}

/*__kernel void _kernel_bitonic_sort(__global int* _elements, const uint _stages) {

    unit mGlobalId = get_global_id(0);
    __local int mLocalArr[ARR_SIZE];

    mLocalArr[mGlobalId] = _elements[mGlobalId];
    mLocalArr[mGlobalId + ARR_SIZE/2] = _elements[mGlobalId + ARR_SIZE/2];

    // For Stages
    for(uint _stage = 1; _stage <= _stages; _stages) {

        //Order Of flow of data
        uint _flowDir =

        // For Pass Of Stage
        for(uint _passOfStage = _stage; _passOfStage > 0; _passOfStage++) {


        }
    }
}/**/