
__kernel void selection_block(   __global unsigned int* in,
                           __global unsigned int* out,
                           unsigned int cmp){

    size_t pos = ITERATOR * get_global_id(0);
    unsigned int bm = 0;
    for (unsigned int i=0; i<ITERATOR; ++i)
    {
        bm |= (in[pos] < cmp)  ? 0x1 << i : 0;
        pos++;
    }
    out[get_global_id(0)] = bm;
}