__kernel void selection_interleaved(
        __global unsigned int* in,
        __global unsigned int* out,
        unsigned int cmp) {

    size_t ip_pos = get_global_id(0);

    size_t result_pos = (get_group_id(0) * get_local_size(0)) + (get_local_id(0));
    if (in[ip_pos] < cmp) atomic_or(&out[result_pos / 32], ((uint) 0x1) << (get_local_id(0) % 32));
}