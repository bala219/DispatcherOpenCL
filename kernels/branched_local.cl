void kernel branched_local_aggregate(
        global const int* PS,
        global const int* BM,
        global int* res
){


size_t pos = 32 * get_global_id(0);
unsigned int local_aggregate = 0;
printf("pos:%d",pos);
__local int l_PS[32], l_RES[32];
int lp = 0;

l_PS[lp] = PS[pos];
l_RES[lp] = 0;
for(int i = 0; i<32 ; i++){
    l_RES[lp]++;
    if(BM[pos]){
        lp++;
        l_PS[lp] = PS[pos];
        l_RES[lp] = 0;
    }
    pos++;
}
for(int i = 0; i< lp;i++){
    printf("pos:%d ps:%d res:%d PS:%d lp:%d\n",get_global_id(0),l_PS[i],l_RES[i],PS[32*get_global_id(0)],lp);
atomic_add(&res[l_PS[i]], l_RES[i]);
    }
}