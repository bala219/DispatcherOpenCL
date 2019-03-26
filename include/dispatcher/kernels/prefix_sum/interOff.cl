__kernel void interOff(__global unsigned int *_index_arr,
                      __global unsigned int *_offset_arr) {

    for (size_t i = 0; i < get_global_id(0); i++) {
        _offset_arr[get_global_id(0)] += _index_arr[i * ITERATOR + ITERATOR - 1];
    }
}




//// Using Upsweep and /down-Sweep within a single work group
//__kernel void interOff(__global unsigned int *_index_arr,
//                       __global unsigned int *_offset_arr) {
//
//
//    //Fill offset array
//    _offset_arr[get_global_id(0)] = _index_arr[(get_global_id(0)+1)*ITERATOR - 1];
//    _offset_arr[get_global_id(0)+get_global_size(0)] = _index_arr[(get_global_id(0)+get_global_size(0)+1)*ITERATOR - 1];
//
//    //Upsweep
//    for(size_t i= 1; i<=OFFSET_SIZE/2;i++){
//        //Select baseline for threads
//        if(get_global_id(0)<=OFFSET_SIZE/2*i){
//            printf("%zd\t",get_global_id(0));
//            //Thread write location TID + 2*level
////            size_t TID = 2*get_global_id(0) + 1;
////            _offset_arr[TID] = _offset_arr[TID -1] + _offset_arr[TID];
//        }
//        barrier(CLK_GLOBAL_MEM_FENCE);
//        printf("\n");
//    }
//
//    //Downsweep
//
//    //Initialize root with Zero: Root --> last value
//    _offset_arr[OFFSET_SIZE - 1] = 0;
//    for(size_t i=OFFSET_SIZE/2;i>=1;i--){
//
//        if(get_global_id(0)%i==0){
//
//            uint current_val = _offset_arr[get_global_id(0)];
//            size_t left_child;
//            left_child = get_global_id(0)/2;
//
//            //New Right Child Value
//            _offset_arr[get_global_id(0)]
//                    = _offset_arr[left_child] + current_val;
//
//            //New left child value
//            _offset_arr[left_child] = current_val;
//        }
//        barrier(CLK_GLOBAL_MEM_FENCE);
//    }
//
//}
