__kernel void gatherPrimitive(__global unsigned int *input,
                                __global unsigned int *position,
                                __global unsigned int *output) {

    output[position[get_global_id(0)]] = input[get_global_id(0)] ;
}