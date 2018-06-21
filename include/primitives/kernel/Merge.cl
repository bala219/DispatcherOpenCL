#define ITER 4

__kernel void _kernel_merge(__global int *arr_left, __global int* arr_right, __global int *arr_result, const uint size_left, const uint size_right )
{
    //setting the pointers to the left and right arrays
    int ptr=get_global_id(0)*ITER;
    printf("Loop started at=%d",ptr);
    int left_ptr=ptr;

    __private int right_ptr;
    right_ptr=0;

    do
    {
        if(arr_left[left_ptr]==arr_right[right_ptr])
        {

            printf("\n ptr %d:left[%d]=%d,right[%d]=%d",ptr,left_ptr,arr_left[left_ptr],right_ptr,arr_right[right_ptr]);
            arr_result[left_ptr]=right_ptr;
            left_ptr++;
            printf("\t left=right at %d",left_ptr);

        }
        else if (arr_left[left_ptr]<arr_right[right_ptr])
        {

            printf("\n ptr %d:left[%d]=%d,right[%d]=%d",ptr,left_ptr,arr_left[left_ptr],right_ptr,arr_right[right_ptr]);
            arr_result[left_ptr]=999;
            left_ptr++;

            printf("\t left<right at %d",-1);
        }else
        {

            printf("\n ptr %d:left[%d]=%d,right[%d]=%d",ptr,left_ptr,arr_left[left_ptr],right_ptr,arr_right[right_ptr]);
            arr_result[left_ptr]=999;
            right_ptr++;
            printf("\t left>right at %d",-1);
        }
        barrier(CLK_LOCAL_MEM_FENCE);

    } while(left_ptr<(ptr+ITER) && right_ptr<size_right);
    if(left_ptr<(ptr+ITER))
    {
        do{
            arr_result[left_ptr]=999;
            left_ptr++;
        }while(left_ptr<(ptr+ITER));
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    printf("------------------------------------------\n");
}