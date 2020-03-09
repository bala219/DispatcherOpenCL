__kernel void materialize_block(__global unsigned int *input, __global unsigned int *bitmap, __global unsigned int *result) {
    size_t pos = 32 * get_global_id(0);
    for (unsigned int i = 0; i < 32; ++i) {
        result[pos + i] = ((bitmap[get_global_id(0)] & (1 << i)) >> i) * input[pos + i];
    }
}
