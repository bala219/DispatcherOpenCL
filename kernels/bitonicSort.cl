#define ARR_SIZE 16

/*__kernel void _kernel_bitonic_sort(__global int* _elements, const uint _stages) {

	uint _mId = get_global_id(0);
    __local int _mLocalArr[ARR_SIZE];

	_mLocalArr[_mId] = _elements[_mId];
	_mLocalArr[_mId + ARR_SIZE/2] = _elements[_mId + ARR_SIZE/2];
	barrier(CLK_LOCAL_MEM_FENCE);

	// PHASE 1. Making bitonic sequences
	for (uint _stage = 1; _stage <= _stages; _stage++) {

		uint _m_signo = (_mId >> (_stage - 1)) & 1;
		for (uint _passOfStage = _stage; _passOfStage > 0; _passOfStage--) {

			uint t = 1 << (_passOfStage-1);
			//uint g = 1 << _passOfStage;

			uint _index = (_mId >> (_passOfStage-1));

			_index = _index << _passOfStage;

			_index = _index + (_mId & (t - 1));

			uint index2 = index + t;

			if ((_mLocalArr[index] > _mLocalArr[index2]) ^ (_m_signo)) {
				int aux = _mLocalArr[index];
				_mLocalArr[index] = _mLocalArr[index2];
				_mLocalArr[index2] = aux;
			}

			printf("\n(%d -  %d , %d) %d %d\t_mLocalArr[%d] :: %d\t_elements[%d] :: %d\n", _mId, t, _m_signo, index, index2, _mId, _mLocalArr[_mId], _mId, _elements[_mId]);

			barrier(CLK_LOCAL_MEM_FENCE);
		}
	}
	_elements[_mId] = _mLocalArr[_mId];
	_elements[_mId + 8] = _mLocalArr[_mId + 8];
}/**/


__kernel void _kernel_bitonic_sort(__global int* _elements, const uint _stages) {

	uint _mId = get_global_id(0);
    __local int _mLocalArr[ARR_SIZE];

	_mLocalArr[_mId] = _elements[_mId];
	_mLocalArr[_mId + ARR_SIZE/2] = _elements[_mId + ARR_SIZE/2];
	barrier(CLK_LOCAL_MEM_FENCE);

	// PHASE 1. Making bitonic sequences
	for (uint _stage = 1; _stage <= _stages; _stage++) {

		uint _m_signo = (_mId >> (_stage - 1)) & 1;
		printf("\nSTAGE :: %d\t SIGNO :: %d", _stage, _m_signo);
		for (uint _passOfStage = _stage; _passOfStage > 0; _passOfStage--) {

			uint t = 1 << (_passOfStage-1);
			//uint g = 1 << _passOfStage;

            printf("\nPASS OF STAGE :: %d\t T :: %d", _passOfStage, t);

			uint index = (_mId >> (_passOfStage-1));

			printf("\n STAGE :: %d\t INDEX 1 :: %d\t MID :: %d\t PASS OF STAGE :: %d\t", _stage, index, _mId, _passOfStage);

			index = index << _passOfStage;

			printf("\n INDEX 2 :: %d\t", index);

			index = index + (_mId & (t - 1));

			printf("\n INDEX 3 :: %d\t MID :: %d\t T :: %d\t", index, _mId, t);

			uint index2 = index + t;

			printf("\n INDEX2 :: %d\t T :: %d\t", index2, t);

			if ((_mLocalArr[index] > _mLocalArr[index2]) ^ (_m_signo)) {

			    printf("\n SIGNO :: %d\t", _m_signo);
				int aux = _mLocalArr[index];
				_mLocalArr[index] = _mLocalArr[index2];
				_mLocalArr[index2] = aux;
			}

			printf("\n(%d -  %d , %d) %d %d\t_mLocalArr[%d] :: %d\t_elements[%d] :: %d\n", _mId, t, _m_signo, index, index2, _mId, _mLocalArr[_mId], _mId, _elements[_mId]);

			barrier(CLK_LOCAL_MEM_FENCE);
		}
	}
	_elements[_mId] = _mLocalArr[_mId];
	_elements[_mId + 8] = _mLocalArr[_mId + 8];
}/**/

/*__kernel void _kernel_bitonic_sort(__global int* _elements, const uint _stages) {

    uint _mId = get_global_id(0);
    __local int _mLocalArray[ARR_SIZE];
    _mLocalArray[_mId] = _elements[_mId];

    for(int _stage = 1; _stage <= _stages+1; _stage++) { // for stages

        printf("\nTRYING OUTER LOOP");

        for(int _passOfStage = 1; _passOfStage <= _stage+1; _passOfStage++) { // for passes of stages

            printf("\nTRYING INNER LOOP");

        }
    }


    for (uint _stage = 1; _stage <= _stages; _stage++) {

    		uint signo = (_mId >> (_stage - 1)) & 1;
    		for (uint _passOfStage = _stage; _passOfStage > 0; _passOfStage--) {

    			uint t = 1 << (_passOfStage-1);
    			//uint g = 1 << _passOfStage;


    			uint index = (_mId >> (_passOfStage-1));
    			index = index << _passOfStage;
    			index = index + (_mId & (t - 1));
    			uint index2 = index + t;

    			if ((_mLocalArr[index] > _mLocalArr[index2]) ^ (signo)) {
    				float aux = _mLocalArr[index];
    				_mLocalArr[index] = _mLocalArr[index2];
    				_mLocalArr[index2] = aux;
    			}

    			printf("\n(%d -  %d , %d) %d %d\t_mLocalArr[%d] :: %d\t_elements[%d] :: %d\n", _mId, t, signo, index, index2, _mId, _mLocalArr[_mId], _mId, _elements[_mId]);

    			barrier(CLK_LOCAL_MEM_FENCE);
    		}
    	}
}/**/