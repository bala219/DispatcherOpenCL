void kernel branched_pos_aggregate(
        global const int* PS,
        global const int* BM,
        global int* res
) {

size_t pos = 32 * get_global_id(0);
unsigned int prev_pos = pos;
for (unsigned int i=0; i<32; ++i)
{

if(BM[pos]){
unsigned int local_aggregate = pos - prev_pos;
atomic_add(&res[PS[pos]],local_aggregate);
prev_pos=pos;
}
pos++;
}
atomic_add(&res[PS[pos]], pos - prev_pos);
}