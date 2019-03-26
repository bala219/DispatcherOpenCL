__kernel void interOff(__global unsigned int *_index_arr,
                      __global unsigned int *_offset_arr) {

    for (size_t i = 0; i < get_global_id(0); i++) {
        _offset_arr[get_global_id(0)] += _index_arr[i * ITERATOR + ITERATOR - 1];
    }
}