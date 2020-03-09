#ifndef REDUCE_AFTER_GROUPBY
#define REDUCE_AFTER_GROUPBY

#define MAX_NUM_BLOCK (512)

__global__
void mapBeforeGather_kernel( Record* d_Rin, int rLen, int* d_loc, int* d_temp ){

	int bx = blockIdx.x;
	int tx = threadIdx.x;
	int gridSize = blockDim.x*gridDim.x;

	for( int idx = bx*blockDim.x + tx; idx < rLen; idx += gridSize ){

		d_loc[idx] = d_Rin[idx].x;
		d_temp[idx] = d_Rin[idx].y;
	}
}

__global__
void parallelAggregate( Record* d_S, int* d_startPos, int* d_aggResults, int blockOffset, int numGroups, int rLen  ){

	int tx = threadIdx.x;
	int numThread = blockDim.x;
	int idx = blockOffset + blockIdx.x;
	int start = d_startPos[idx];
	int end = (idx == (numGroups - 1))?(rLen):(d_startPos[idx + 1]);
	extern __shared__ int s_data[];
	int currentVal;

	//initialization the shared memory
	s_data[tx] = 0;

	for( int i = start + tx; i < end; i = i + numThread ){

		s_data[tx] += d_S[i].y;
	}

	__syncthreads();

	int delta = 2;
	while( delta <= numThread )
	{
		int offset = delta*tx;
		if( offset < numThread )
		{
			s_data[offset] = (s_data[offset]) + (s_data[offset + delta/2]);
		}

		delta = delta*2;
		__syncthreads();
	}

	__syncthreads();

	if( tx == 0 )
	{
		d_aggResults[idx] = s_data[0];

	}

}

void gatherBeforeAgg( Record* d_Rin, int rLen, Record* d_Ragg, Record* d_S, int numThread, int numBlock )
{
	int* d_loc;
	GPUMALLOC( (void**)&d_loc, sizeof(int)*rLen ) ;

	//map for the x
	unsigned int timer = 0;

	//cudaThreadSynchronize();
	//startTimer( &timer );
	int* d_temp;
	GPUMALLOC( (void**)&d_temp, sizeof(int)*rLen ) ;
	mapBeforeGather_kernel<<<numBlock, numThread>>>( d_Rin, rLen, d_loc, d_temp );
	cudaThreadSynchronize();
	//endTimer( "map", &timer );

	CUDA_SAFE_CALL( cudaFree(d_temp) );

	//gather
	//unsigned int timer = 0;
	startTimer(&timer);
	gatherImpl( d_Ragg, rLen, d_loc, d_S,rLen, numThread, numBlock);
	cudaThreadSynchronize();
	endTimer( "gather", &timer );
}

void aggAfterGroupByImpl(Record *d_Rin, int rLen, int* d_startPos, int numGroups, Record* d_Ragg, int* d_aggResults, int OPERATOR,
						 int numThread = 256){
	unsigned int timer = 0;

	//gather
	Record* d_S;
	GPUMALLOC( (void**)&d_S, sizeof(Record)*rLen ) ;
	gatherBeforeAgg( d_Rin, rLen, d_Ragg, d_S, 512, 256 );
	cudaThreadSynchronize();

	//parallel aggregation after gather
	//numThread = 1;
	int numChunk = ceil(((float)numGroups)/MAX_NUM_BLOCK);
	int numBlock;
	int blockOffset;

	int sharedMemSize = sizeof(int)*numThread;

	startTimer(&timer);
	for( int chunkIdx = 0; chunkIdx < numChunk; chunkIdx++ )
	{
		blockOffset = chunkIdx*MAX_NUM_BLOCK;
		if( chunkIdx == ( numChunk - 1 ) )
		{
			numBlock = numGroups - chunkIdx*MAX_NUM_BLOCK;
		}
		else
		{
			numBlock = MAX_NUM_BLOCK;
		}
		parallelAggregate<<<numBlock, numThread, sharedMemSize>>>( d_S, d_startPos, d_aggResults, OPERATOR, blockOffset, numGroups, rLen );
		cudaThreadSynchronize();
	}
	endTimer( "parallel aggregate", &timer );

	CUDA_SAFE_CALL( cudaFree( d_S ) );
}

#endif