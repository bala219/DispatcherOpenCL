__kernel void branched_aggregate_local(
        global const int* PS,
        global const int* BM,
        global int* res
) {

    __local int local_res_ps[32];
    __local int local_res[32];

}