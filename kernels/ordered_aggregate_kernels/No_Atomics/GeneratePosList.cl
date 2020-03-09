
//Call with total size one less
__kernel void GeneratePosList(  __global unsigned int* ps,
                                 __global unsigned int* res) {

    size_t pos = get_global_id(0);
    if((ps[pos]!=ps[pos + 1])||(pos == DATA_SIZE)){
        res[ps[pos]] = pos;
    }


}