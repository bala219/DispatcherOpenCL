__kernel void split_block(__global unsigned int *input,
                          __global unsigned int **output,
                          unsigned int chunk_size) {

    size_t pos = 32 * get_global_id(0);

    for (unsigned int i=0; i<32; ++i)
    {
        output[i/chunk_size][i%chunk_size] = input[i];
        pos++;
    }
}