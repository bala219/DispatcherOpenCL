__kernel void FinalPS(__global unsigned int *_index_arr) {


    uint offset=0;

    for (size_t i = 0; i < get_global_id(0); i++) {
        offset += _index_arr[i * ITERATOR + ITERATOR - 1];
    }


    uint mIndex = get_global_id(0) * ITERATOR;

    int add_offset = !(get_global_id(0)==0) * offset;

    for(int k=0; k<ITERATOR; k++){
        uint mI = k + mIndex;
        _index_arr[mI] = _index_arr[mI] + add_offset;
    }

}