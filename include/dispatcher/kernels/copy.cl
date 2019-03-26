__kernel void copy(   __global unsigned int* data,
                           __global unsigned int* result
                    )
{
    result[get_global_id(0)] = data[get_global_id(0)];
}