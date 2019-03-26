__kernel void partialPS(__global const unsigned int *_sorted_element,
                        __global unsigned int *_index_arr,
                        __global unsigned int *_offset_arr) {

    size_t pos = get_global_id(0)*ITERATOR;
    //Initialize first value

    _index_arr[pos] = (_sorted_element[pos] != _sorted_element[(pos!=0)*(pos - 1)]);

    //Partial Prefix Sum
    for(size_t i =1; i<ITERATOR&&(i+pos<DATA_SIZE);i++){
        _index_arr[i + pos] = _index_arr[i + pos -1] +
                (_sorted_element[i + pos] != _sorted_element[i + pos - 1]);
    }

//    _offset_arr[get_global_id(0)] = _index_arr[pos+ITERATOR -1];
}