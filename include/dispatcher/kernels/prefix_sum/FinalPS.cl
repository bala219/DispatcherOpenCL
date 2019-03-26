__kernel void FinalPS(__global unsigned int *_index_arr,
                      __global unsigned int *_offset_arr) {

    uint mIndex = get_global_id(0) * ITERATOR;

//    int add_offset = !(get_global_id(0)==0) * offset;

    for(int k=0; k<ITERATOR; k++){
        uint mI = k + mIndex;
        _index_arr[mI] = _index_arr[mI] + _offset_arr[get_global_id(0)];
    }

}