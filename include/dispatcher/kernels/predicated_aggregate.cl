void kernel predicated_pos_aggregate(
        global const int* PS,
        global const int* BM,
        global int* res
) {

    size_t pos = 32 * get_global_id(0);
    unsigned int local_aggregate = 0;
    for (unsigned int i = 0;i<32; ++i)
    {
        local_aggregate++;
        res[PS[pos]-1] = BM[pos] *
        local_aggregate;
        local_aggregate = !BM[pos] * local_aggregate;
        pos++;
    }
    atomic_add(&res[PS[pos - 1]], local_aggregate);

}