__kernel void zero_kernel(__global unsigned int *input) {

    input[get_global_id(0)] = 0;

}