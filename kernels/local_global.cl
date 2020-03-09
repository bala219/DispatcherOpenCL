void kernel local_global(
        __global const int* source,
        __local int* dest
) {
size_t local_offset = get_local_id(0)*32;
size_t global_offset = get_global_id(0)*32;
event_t event;
async_work_group_copy(&dest[local_offset],&source[global_offset],32,event);

wait_group_events(1,&event);

size_t pos = 32 * get_global_id(0);
for (unsigned int i = 0;i<32; ++i){

printf("Thread:%d--> %d , (%d,%d)\n",get_global_id(0),source[get_global_id(0)],source[i+ global_offset],dest[i + local_offset]);
}
}