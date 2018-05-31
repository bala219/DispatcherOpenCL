#define ARR_SIZE 16

__kernel void _GPU_kernel_bitonic_sort(__global int* _elements, const uint _stages) {

	uint _mId = get_global_id(0);
    local int _mLocalArr[ARR_SIZE];

	_mLocalArr[_mId] = _elements[_mId];
	_mLocalArr[_mId + ARR_SIZE/2] = _elements[_mId + ARR_SIZE/2];
	barrier(CLK_LOCAL_MEM_FENCE);

	// PHASE 1. Making bitonic sequences
	for (uint _stage = 1; _stage <= _stages; _stage++) {
		uint _m_signo = (_mId >> (_stage - 1)) & 1;
		for (uint _passOfStage = _stage; _passOfStage > 0; _passOfStage--) {

			uint _targetElements = 1 << (_passOfStage-1);
			//uint g = 1 << _passOfStage;

			uint _index = (_mId >> (_passOfStage-1));
			_index = _index << _passOfStage;
			_index = _index + (_mId & (_targetElements - 1));

			uint index2 = _index + _targetElements;

			if ((_mLocalArr[_index] > _mLocalArr[index2]) ^ (_m_signo)) {
				int aux = _mLocalArr[_index];
				_mLocalArr[_index] = _mLocalArr[index2];
				_mLocalArr[index2] = aux;
			}

            printf("\n(ID :: %d \t TARGET ELEMENTS :: %d , \t SIGNO :: %d) \t INDEX :: %d \t INDEX2 :: %d\t ARRAY :: %d", _mId, _targetElements, _m_signo, _index, index2, _mLocalArr[_mId]);

			barrier(CLK_LOCAL_MEM_FENCE);
		}

		printf("\n---------------------------------------------------------------------");
	}
	_elements[_mId] = _mLocalArr[_mId];
	_elements[_mId + ARR_SIZE/2] = _mLocalArr[_mId + ARR_SIZE/2];
}