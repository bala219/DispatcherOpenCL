__kernel void partial_agg_compute(__global const unsigned int *_sorted_element,
                                  __global unsigned int *_index_arr,
                                  __global unsigned int *_offset_arr,
                                  __global unsigned int *agg_size,
                                  unsigned int total_size) {

    short local_pointer = get_global_id(0) * ITERATOR;

    for(int i = (int) get_global_id(0)* ITERATOR; (i < ((get_global_id(0)* ITERATOR) + ITERATOR ))&&(i<total_size ); i++){

        _index_arr[local_pointer]++;
        local_pointer += (_sorted_element[i]!=_sorted_element[i+1]);
    }

    _offset_arr[get_global_id(0)] = local_pointer;
//                                    +(_sorted_element[(get_global_id(0)+1)* ITERATOR]
//             !=_sorted_element[(get_global_id(0)+1)* ITERATOR - 1]);

    local_pointer -= (get_global_id(0) * ITERATOR);
    atomic_add(agg_size,local_pointer);
}