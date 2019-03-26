void kernel branched_aggregate_reversed(
        global const int* PS,
        global const int* BM,
        global int* res
) {

size_t pos = 32 * get_global_id(0);
unsigned int prev_pos = pos;
for (unsigned int i=0; i<32; ++i)
{
if(!BM[pos]){
unsigned int local_aggregate = pos - prev_pos + 1;
unsigned int local_aggregate_check = PS[pos] - PS[prev_pos] + 1;
atomic_add(&res[local_aggregate_check - local_aggregate],local_aggregate);
prev_pos=pos;
}
pos++;
}

unsigned int local_aggregate = pos - prev_pos;
unsigned int local_aggregate_check = PS[pos] - PS[prev_pos];
atomic_add(&res[local_aggregate_check - local_aggregate], pos - prev_pos);

}