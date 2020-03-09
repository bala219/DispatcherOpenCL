
//Atomic add to results the value from local results
void kernel ga_kerel_one(
        global int* res1,
        global int* res2,
        global int* res3
) {

    size_t pos = get_global_id(0);
    atomic_add(&res1[res2[pos]],res3[pos]);
}
