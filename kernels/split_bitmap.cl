void kernel split_bitmap(
        global const int* input,
        global const int* output,
        global int pivot
) {
    int pos = get_global_id(0);

    output[pos] = (input[pos]>pivot);
}