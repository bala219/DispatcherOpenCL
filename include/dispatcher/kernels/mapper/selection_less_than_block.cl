
__kernel void selection_less_than_block(   __global unsigned int* data,
                           __global unsigned int* bitmap,
                           unsigned int c1)
{ size_t pos = 32 * get_global_id(0);
    unsigned int bm = 0;
    for (unsigned int i=0; i<32; ++i)
    {
        bm |= (data[pos] < c1)  ? 0x1 << i : 0;
        pos++;
    }
    bitmap[get_global_id(0)] = bm;
}