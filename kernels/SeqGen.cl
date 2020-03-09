__kernel void SeqGen(__global unsigned int *input) {

    input[get_global_id(0)] = get_global_id(0);
}