__kernel void final_agg_compute(__global unsigned int *_index_arr,
                      __global unsigned int *_offset_arr,
                      __global unsigned int *_res_arr) {

    //Copy values to result array
    int mIndex = get_global_id(0) * ITERATOR;
    int ValueInOffset = _offset_arr[get_global_id(0)];
    int totalValues = ValueInOffset - mIndex + 1;
    int startIndex = 0;
    int off = 0;
//    printf("%d,%d,%d,",mIndex,ValueInOffset,totalValues);

    //Compute start index
    for(int i=0;i<get_global_id(0);i++){
        startIndex+=(_offset_arr[i] - i*32);
    }

//    printf("%d,%d,%d\n",startIndex,mIndex,totalValues);
    //copy values except first and last value
    for(int i=1;i<=totalValues - 1;i++){
        _res_arr[i+startIndex] = _index_arr[mIndex + i];
    }

//    Atomic add first position
    atomic_add(&_res_arr[startIndex],_index_arr[mIndex]);

    //Atomic add last index
    atomic_add(&_res_arr[startIndex+totalValues],_index_arr[mIndex + totalValues ]);

}