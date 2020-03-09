//Compares two column values and returns position of the first that satisfies the condition
void kernel nestedLoop(
	global const int* A,
	global const int* B,
	global int* C
	) {

	int pos = get_global_id(0);

	for(int i=0;i<DATA_SIZE;i++){
		if(A[pos]==B[i])
			C[pos] = i;
	}

	C[pos] = -1;
}
