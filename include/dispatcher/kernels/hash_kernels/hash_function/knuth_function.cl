__kernel void knuth_function(  __global unsigned int* input,
                                 __global unsigned int* slot){

    slot[get_global_id(0)] = ((unsigned long)((unsigned int)1300000077*input[get_global_id(0)])* TABLE_SIZE)>>32;
}