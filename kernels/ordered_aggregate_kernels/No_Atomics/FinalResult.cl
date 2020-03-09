//Call with total size one less
__kernel void FinalAggregate(  __global unsigned int* breakPoint,
                                __global unsigned int* res) {

    size_t pos = get_global_id(0);

    if(!pos)
        res[pos] = breakPoint[pos] + 1;
    else
        res[pos] = breakPoint[pos] - breakPoint[pos - 1];

}