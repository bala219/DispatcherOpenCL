void kernel branched_aggregate(
        global const int* PS,
        global int* res
) {
int pos = get_group_id(0);
atomic_add(&res[PS[pos]],1);
}