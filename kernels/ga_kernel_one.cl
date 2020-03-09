

// Do normal local aggregation with the result array
void kernel ga_kerel_one(
        global const int* PS,
        global const int* BM,
        global int* res1,
        global int* res2,
        global int* res3
) {

size_t pos = 32 * get_global_id(0);
unsigned int local_aggregate = 0;
for (unsigned int i=0; i<32; ++i)
{
local_aggregate++;
if(BM[pos]){
atomic_add(&res[PS[pos]-1],local_aggregate);
local_aggregate=0;
}
pos++;
}
res2[get_global_id(0)] = local_aggregate;
res3[get_global_id(0)] = PS[pos-1];

