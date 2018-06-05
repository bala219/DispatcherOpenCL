__kernel void _kernel_bitonic_sort(__global int* _elements, const uint _stages) {

	uint _mId = get_global_id(0);
    __local int _mLocalArr[ARR_SIZE];

	_mLocalArr[_mId] = _elements[_mId];
	_mLocalArr[_mId + ARR_SIZE/2] = _elements[_mId + ARR_SIZE/2];
	barrier(CLK_LOCAL_MEM_FENCE);

	// PHASE 1. Making bitonic sequences
	for (uint _stage = 1; _stage <= _stages; _stage++) {

		uint _m_signo = (_mId >> (_stage - 1)) & 1;
		for (uint _passOfStage = _stage; _passOfStage > 0; _passOfStage--) {

			uint _targetElements = 1 << (_passOfStage-1);
			//uint g = 1 << _passOfStage;

			uint _index1 = (_mId >> (_passOfStage-1));
			_index1 = _index1 << _passOfStage;
			_index1 = _index1 + (_mId & (_targetElements - 1));

			uint _index2 = _index1 + _targetElements;

			if ((_mLocalArr[_index1] > _mLocalArr[_index2]) ^ (_m_signo)) {
				int aux = _mLocalArr[_index1];
				_mLocalArr[_index1] = _mLocalArr[_index2];
				_mLocalArr[_index2] = aux;
			}

            //printf("\nSTAGE :: %d\tPASS_OF_STAGE :: %d \t(ID :: %d \tTARGET_ELEMENTS :: %d , \tSIGNO :: %d) \tINDEX1 :: %d \tINDEX2 :: %d \tARRAY[%d] :: %d \t\tARRAY[%d] :: %d", _stage, _passOfStage, _mId, _targetElements, _m_signo, _index1, _index2, _index1, _mLocalArr[_index1], _index2, _mLocalArr[_index2]);

			barrier(CLK_LOCAL_MEM_FENCE);

			/*if(_index2 == ARR_SIZE-1) {
	            printf("\n---------------------------------------------------------------------");

			}
			if(_passOfStage == 1) {
			    printf("\n---------------------------------------------------------------------");
			}*/
		}
	}
	_elements[_mId] = _mLocalArr[_mId];
	_elements[_mId + ARR_SIZE/2] = _mLocalArr[_mId + ARR_SIZE/2];
}

/*__kernel void _kernel_bitonic_sort(__global int* _elements, const uint _stages) {

    unit _mId = get_global_id(0);
    __local int _mLocalArr[ARR_SIZE];

    _mLocalArr[_mId] = _elements[_mId];
    _mLocalArr[_mId + ARR_SIZE/2] = _elements[_mId + ARR_SIZE/2];

    // For Stages
    for(uint _stage = 1; _stage <= _stages; _stages) {

        //Order Of flow of data
        uint _flowDir =

        // For Pass Of Stage
        for(uint _passOfStage = _stage; _passOfStage > 0; _passOfStage++) {


        }
    }
}/**/