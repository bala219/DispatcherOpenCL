//
// Created by tompi on 6/3/18.
//

/*#include "../header/Aggregation.h"

void Aggregation::Aggregate(cl_device_id _DEVICE, int _sorted_elements[], uint _element_size, uint _no_distinct_val) {

    // Make array of double the size of distinct value
    uint * _m_res_arr = new uint[_no_distinct_val * 2];

    // Add data to the respective devices
    add_data("sorted_elements", _sorted_elements, _DEVICE, _element_size);
    add_data("res_arr", _m_res_arr, _DEVICE, (_no_distinct_val * 2));

    lookup_data_buffer();

    print_data("sorted_elements", _DEVICE, _element_size);
}/**/