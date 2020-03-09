__kernel void scatterPrimitive(__global unsigned int *input,
                                __global unsigned int *position,
                                __global unsigned int *output) {

    output[get_global_id(0)] = input[position[get_global_id(0)]];
}