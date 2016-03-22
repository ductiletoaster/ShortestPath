/**
 * Author: Brian Gebel
 * Version: 12.10.2011
 * Description:
 */

/**   
 *	For testing parallelization speeds with different memory types.
 *	The new version may prove to be faster, will test in future version.
 *
 *	OLD VERSION(USING GLOBAL DEVICE MEM)
 *	int *hostMatrix = (int*)malloc(sizeof(int)*SIZE)	
 * 	int *devMatrix;
 *	cudaMalloc((void**) &devMatrix,sizeof(int)*SIZE);
 * 	cudaMemcpy(devMatrix, hostMatrix, sizeof(int)*SIZE, cudaMemcpyHostToDevice);
 *
 *	NEW VERSION(USING CONSTANT DEVICE MEM)
 *	_constant_ int devMatrix[SIZE];
 *	cudaMemcpyToSymbol(devMatrix,hostMatrix,sizeof(int)*SIZE);
 *
 * Seems their is an issue when we have more than 2 blocks.
 * Threads may be run over the top of each other should debug
 * Also when determining the number of blocks we could easily optimise that
 */

#include <iostream>
#include <stdio.h>
#include <windows.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "parallelpath.cuh"

using namespace std;
using std::cout;
using std::endl;

__global__ void findBestPathKernel(int *deviceMatrix, int *deviceBestToDiagPathSum, int *deviceBestFromDiagPathSum, unsigned int *deviceBestToDiagPathCode, unsigned int *deviceBestFromDiagPathCode, int N) {

	// Iterate through path codes, updating bestToDiag and bestFromDiag arrays
	int toDiagPathSum, toDiagRow, toDiagCol;
	int fromDiagPathSum, fromDiagRow, fromDiagCol;

	unsigned int maxPath = ((unsigned int) 1 << N - 1);
	unsigned int currentPathCode = blockIdx.x * threadsPerBlock + threadIdx.x;
	unsigned int nextPathCode = blockDim.x * gridDim.x;

	while (currentPathCode <= maxPath) {

		unsigned int ui = currentPathCode;

		toDiagPathSum = deviceMatrix[0];
		toDiagRow = 0;
		toDiagCol = 0;

		fromDiagPathSum = deviceMatrix[(N - 1) * N + (N - 1)];
		fromDiagRow = N - 1;
		fromDiagCol = N - 1;

		for ( int i = 0 ; i < N - 1 ; i++ ) {
			if ( ui % 2 == 0) {
				toDiagCol++; // Horizontal move
				fromDiagCol--;
			} else {
				toDiagRow++;
				fromDiagRow-- ;	// Vertical move
			}

			toDiagPathSum += deviceMatrix[(toDiagRow * N) + toDiagCol];
			fromDiagPathSum += deviceMatrix[(fromDiagRow * N) + fromDiagCol];
			ui = ui >> 1;
		}
		
		if (toDiagPathSum < deviceBestToDiagPathSum[toDiagRow] ) {
			deviceBestToDiagPathSum[toDiagRow] = toDiagPathSum;
			deviceBestToDiagPathCode[toDiagRow] = currentPathCode;
		}

		if (fromDiagPathSum < deviceBestFromDiagPathSum[fromDiagRow]) {
			deviceBestFromDiagPathSum[fromDiagRow] = fromDiagPathSum;
			deviceBestFromDiagPathCode[fromDiagRow] = currentPathCode;
		}

		// Next Path Code
		currentPathCode += nextPathCode;
	}
}

void findBestPathParallel(int N) {
	// Memory Allocations
	unsigned int matrixMemSize = sizeof(int) * N * N;
	unsigned int arrayMemSize = sizeof(int) * N;
	unsigned int unsignedArrayMemSize = sizeof(unsigned int) * N;

	// Allocate Memory on the host
	int *hostMatrix;
    hostMatrix = (int*) malloc(sizeof(int) * N * N);

	// Create and initialize matrix
	for (int i = 0 ; i < N * N ; i++)
		hostMatrix[i] = 99;

	// Create path along edges of matrix
	if (N == 16) {
		setMatrixParallelPath(hostMatrix);
	} else {
		for (int i = 0 ; i < N; i++) {
			hostMatrix[i] = 1;
			hostMatrix[i * N + (N - 1)] = 1;
		}
	}

	// Create and initialize bestTodiag and bestfromDiag arrays on host
	int *hostBestToDiagPathSum;
	hostBestToDiagPathSum = (int*) malloc(sizeof(int) * N);

	int *hostBestFromDiagPathSum;
	hostBestFromDiagPathSum = (int*) malloc(sizeof(int) * N);

	unsigned int *hostBestToDiagPathCode; //used unsigned int to avoid 2's complent
	hostBestToDiagPathCode = (unsigned int*) malloc(sizeof(unsigned int) * N);

	unsigned int *hostBestFromDiagPathCode; //used unsigned int to avoid 2's complent
	hostBestFromDiagPathCode = (unsigned int*) malloc(sizeof(unsigned int) * N);

	int biggerThanMaxPathSum = 256 * N + 1;
	for ( int i = 0 ; i < N ; i++ ) {
		hostBestToDiagPathSum[i]  = biggerThanMaxPathSum ;
		hostBestFromDiagPathSum[i] = biggerThanMaxPathSum ;
	}

	// Allocate Memory on the device for matrices
	int *deviceMatrix;
    cudaMalloc((void**) &deviceMatrix, matrixMemSize);
	cudaMemcpy(deviceMatrix, hostMatrix, matrixMemSize, cudaMemcpyHostToDevice);

	// Allocate Memory on the device for arrays
	int *deviceBestToDiagPathSum;
    cudaMalloc((void**) &deviceBestToDiagPathSum, arrayMemSize);
	cudaMemcpy(deviceBestToDiagPathSum, hostBestToDiagPathSum, arrayMemSize, cudaMemcpyHostToDevice);

	int *deviceBestFromDiagPathSum;
	cudaMalloc((void**) &deviceBestFromDiagPathSum, arrayMemSize);
	cudaMemcpy(deviceBestFromDiagPathSum, hostBestFromDiagPathSum, arrayMemSize, cudaMemcpyHostToDevice);

	// Allocate Memory on the device for unsigned arrays
	unsigned int *deviceBestToDiagPathCode; //used unsigned int to avoid 2's complent
	cudaMalloc((void**) &deviceBestToDiagPathCode, unsignedArrayMemSize);
	cudaMemcpy(deviceBestToDiagPathCode, hostBestToDiagPathCode, unsignedArrayMemSize, cudaMemcpyHostToDevice);

	unsigned int *deviceBestFromDiagPathCode; //used unsigned int to avoid 2's complent
	cudaMalloc((void**) &deviceBestFromDiagPathCode, unsignedArrayMemSize);
	cudaMemcpy(deviceBestFromDiagPathCode, hostBestFromDiagPathCode, unsignedArrayMemSize, cudaMemcpyHostToDevice);

	// Execution Parameters
	int totalBlocks = (((unsigned int)1 << (N - 1)) + threadsPerBlock) / threadsPerBlock;
	if (totalBlocks > maxBlocks) totalBlocks = maxBlocks;

	// Launch a kernel on the GPU with one thread for each element.
	findBestPathKernel<<< totalBlocks, threadsPerBlock >>>(deviceMatrix, deviceBestToDiagPathSum, deviceBestFromDiagPathSum, deviceBestToDiagPathCode, deviceBestFromDiagPathCode, N);

	// Copy result from device to host
	cudaMemcpy(hostMatrix, deviceMatrix, matrixMemSize, cudaMemcpyDeviceToHost);
	cudaMemcpy(hostBestToDiagPathSum, deviceBestToDiagPathSum, arrayMemSize, cudaMemcpyDeviceToHost);
	cudaMemcpy(hostBestFromDiagPathSum, deviceBestFromDiagPathSum, arrayMemSize, cudaMemcpyDeviceToHost);
	cudaMemcpy(hostBestToDiagPathCode, deviceBestToDiagPathCode, unsignedArrayMemSize, cudaMemcpyDeviceToHost);
	cudaMemcpy(hostBestFromDiagPathCode, deviceBestFromDiagPathCode, unsignedArrayMemSize, cudaMemcpyDeviceToHost);

	// Examine bestToDiag and bestFromDiag arrays to decide best path
	int bestPathSum = biggerThanMaxPathSum;
	unsigned int bestPathCodePrefix ;
	unsigned int bestPathCodeSuffix ;

	int tempSum ;

	for (int i = 0 ; i < N ; i++) {
		tempSum = hostBestToDiagPathSum[i] + hostBestFromDiagPathSum[i] - hostMatrix[i * N + (N - 1 - i)];
		if (tempSum < bestPathSum) {
			bestPathSum = tempSum;
			bestPathCodePrefix = hostBestToDiagPathCode[i];
			bestPathCodeSuffix = hostBestFromDiagPathCode[i];
		}
	}

	hostMatrix[0] = -hostMatrix [0];
	int toDiagRow = 0;
	int toDiagCol = 0;
	unsigned ui = bestPathCodePrefix;
	
	for ( int i = 0 ; i < N - 1 ; i++ ) {
		if (ui % 2 == 0 )
			toDiagCol++ ;
		else
			toDiagRow++ ;
		if (hostMatrix[toDiagCol + (toDiagRow * N)] > 0) hostMatrix[toDiagCol + (toDiagRow * N)] = -hostMatrix[toDiagCol + (toDiagRow * N)];
		ui = ui >> 1;
	}

	hostMatrix[(N - 1) * N + (N - 1)] = -hostMatrix [(N - 1) * N + (N - 1)] ;
	int fromDiagRow = N - 1;
	int fromDiagCol = N - 1;
	ui = bestPathCodeSuffix;
	
	for ( int i = 0 ; i < N - 1 ; i++ ) {
		if (ui % 2 == 0 )
			fromDiagCol--;
		else
			fromDiagRow--;
		if (hostMatrix[fromDiagCol + (fromDiagRow * N)] > 0) hostMatrix[fromDiagCol + (fromDiagRow * N)] = -hostMatrix[fromDiagCol + (fromDiagRow * N)];
		ui = ui >> 1;
	}

	displayBestPathParallel(hostMatrix, N, bestPathSum);

	// Free up memory
	free(hostMatrix);
	free(hostBestToDiagPathSum);
	free(hostBestFromDiagPathSum);
	free(hostBestToDiagPathCode);
	free(hostBestFromDiagPathCode);
	cudaFree(deviceMatrix);
	cudaFree(deviceBestToDiagPathSum);
	cudaFree(deviceBestFromDiagPathSum);
	cudaFree(deviceBestToDiagPathCode);
	cudaFree(deviceBestFromDiagPathCode);
}

void displayBestPathParallel(int *hostMatrix, int N, int bestPathSum) {
	// Allows for colors in cmd window
	CONSOLE_SCREEN_BUFFER_INFO *ConsoleInfo = new CONSOLE_SCREEN_BUFFER_INFO();
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), ConsoleInfo);
    WORD OriginalColors = ConsoleInfo->wAttributes;

	// Builds Visual Matrix
	for (int row = 0 ; row < N ; row++) {
		for ( int col = 0 ; col < N ; col++)
			if (hostMatrix[row * N + col] < 0) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
				cout << -hostMatrix[row * N + col];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), OriginalColors);
			} else
				cout << "@";
		cout << endl;
	}
	cout << endl <<"Best Path Sum: " << bestPathSum << endl;
}

void setMatrixParallelPath(int *Matrix) {
	Matrix[0] = 1; // Row 0
	Matrix[1] = 3;
	Matrix[2] = 3;
	Matrix[3] = 3;
	Matrix[4] = 3;

	Matrix[16] = 1;
	Matrix[20] = 3;

	Matrix[32] = 1;
	Matrix[36] = 3;
	Matrix[45] = 1;

	Matrix[48] = 1;
	Matrix[52] = 3;
	Matrix[61] = 1;
	Matrix[62] = 1; 

	Matrix[64] = 1; // Row 4
	Matrix[68] = 3;
	Matrix[69] = 3;
	Matrix[70] = 3;
	Matrix[78] = 1;
	Matrix[79] = 1;

	Matrix[80] = 1;
	Matrix[81] = 1;
	Matrix[86] = 3;
	Matrix[95] = 1;

	Matrix[97] = 1;
	Matrix[102] = 3;
	Matrix[111] = 1;

	Matrix[113] = 1;
	Matrix[118] = 3;
	Matrix[127] = 1;

	Matrix[129] = 1; // Row 8
	Matrix[134] = 3;
	Matrix[135] = 2;
	Matrix[143] = 1;

	Matrix[145] = 1;
	Matrix[151] = 2;
	Matrix[152] = 2;
	Matrix[153] = 2;
	Matrix[154] = 2;
	Matrix[159] = 1;

	Matrix[161] = 1;
	Matrix[162] = 1;
	Matrix[170] = 2;
	Matrix[175] = 1;

	Matrix[178] = 1;
	Matrix[186] = 2;
	Matrix[191] = 1;

	Matrix[194] = 1; // Row 12
	Matrix[195] = 1;
	Matrix[202] = 2;
	Matrix[207] = 1;

	Matrix[218] = 2;
	Matrix[219] = 2;
	Matrix[220] = 2;
	Matrix[224] = 1;

	Matrix[236] = 2;
	Matrix[237] = 2;
	Matrix[239] = 1;

	Matrix[253] = 2;
	Matrix[254] = 2;
	Matrix[255] = 1;
}