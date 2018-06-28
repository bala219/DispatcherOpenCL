__kernel void _kernel_aggregate(__global const int *_index_arr,
                                __global const int *_offset,
                                __global int *_agg_res) {

    __local uint mOffsetIndex, mAggResIndex;
    uint mGlobalId = get_global_id(0);

    // For Calculating the Aggregate Of the Array
    mOffsetIndex = (mGlobalId / ITERATOR) - 1;
    mAggResIndex = (mOffsetIndex != -1) ? _offset[mOffsetIndex] + _index_arr[mGlobalId] : _index_arr[mGlobalId];
    atomic_add(&_agg_res[mAggResIndex], 1);
}