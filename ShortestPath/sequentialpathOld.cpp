/**
 * Author: Brian Gebel
 * Version: 12.10.2011
 * Description:
 */

#include <iostream>
#include <stdio.h>
#include <windows.h>
#include "sequentialpath.h"

using namespace std;
using std::cout;
using std::endl;

void findBestPathSequential(int N) {

	// Memory Allocation
	int *Matrix;
	Matrix = new int [N * N];

	// Create and initialize matrix
	for (int row = 0 ; row < N ; row++)
		for (int col = 0 ; col < N ; col++)
			Matrix[row * N + col] = 99;

	// Create path along edges of matrix
	if (N == 16) {
		setMatrixSequentialPath(Matrix);
	} else {
		for (int i = 0 ; i < N; i++) {
			Matrix[i] = 1;
			Matrix[i * N + (N - 1)] = 1;
		}
	}

	// Create and initialize bestTodiag and bestfroMatrixDiag arrays
	int *bestToDiagPathSum;
	bestToDiagPathSum = new int [N];

	int *bestFromDiagPathSum;
	bestFromDiagPathSum = new int [N];

	unsigned int *bestToDiagPathCode; //used unsigned int to avoid 2's complement
	bestToDiagPathCode = new unsigned int [N];

	unsigned int *bestFromDiagPathCode; //used unsigned int to avoid 2's complement
	bestFromDiagPathCode = new unsigned int [N];

	int biggerThanMaxPathSum = 256 * N + 1;
	for ( int i = 0 ; i < N ; i++ ) {
		bestToDiagPathSum [i]  = biggerThanMaxPathSum;
		bestFromDiagPathSum[i] = biggerThanMaxPathSum;
	}
	
	// Iterate through path codes, updating bestToDiag and bestFromDiag arrays
	int toDiagPathSum, toDiagRow, toDiagCol;
	int fromDiagPathSum, fromDiagRow, fromDiagCol;

	unsigned int ui;

	for (unsigned int currentPathCode = 0 ; currentPathCode < ((unsigned int) 1 << (N - 1)) ; currentPathCode++) {
		ui = currentPathCode;

		toDiagPathSum = Matrix[0];
		toDiagRow = toDiagCol = 0;

		fromDiagPathSum = Matrix[(N - 1) * N + (N - 1)];
		fromDiagRow = fromDiagCol = N - 1;

		for ( int i = 0 ; i < N - 1 ; i++ ) {
			if ( ui % 2 == 0) {
				toDiagCol++; // Horizontal move
				fromDiagCol--;
			} else {
				toDiagRow++;
				fromDiagRow--;	// Vertical move
			}
			toDiagPathSum += Matrix[(toDiagRow * N) + toDiagCol];
			fromDiagPathSum += Matrix[(fromDiagRow * N) + fromDiagCol];
			ui = ui >> 1;
		}

		if (toDiagPathSum < bestToDiagPathSum[toDiagRow] ) {
			bestToDiagPathSum[toDiagRow] = toDiagPathSum;
			bestToDiagPathCode[toDiagRow] = currentPathCode;
		}

		if (fromDiagPathSum < bestFromDiagPathSum [fromDiagRow]) {
			bestFromDiagPathSum[fromDiagRow] = fromDiagPathSum;
			bestFromDiagPathCode[fromDiagRow] = currentPathCode;
		}
	}

	// Examine bestToDiag and bestFromDiag arrays to decide best path
	int bestPathSum = biggerThanMaxPathSum;
	unsigned int bestPathCodePrefix;
	unsigned int bestPathCodeSuffix;

	int tempSum;


	for (int i = 0 ; i < N ; i++) {
		tempSum = bestToDiagPathSum[i] + bestFromDiagPathSum[i] - Matrix[i * N + (N - 1 - i)];
		if (tempSum < bestPathSum) {
			bestPathSum = tempSum;
			bestPathCodePrefix = bestToDiagPathCode[i];
			bestPathCodeSuffix = bestFromDiagPathCode[i];
		}
	}

	Matrix[0] = -Matrix [0]; // Starting point - First row first column
	toDiagRow = 0;
	toDiagCol = 0;
	ui = bestPathCodePrefix;

	for ( int i = 0 ; i < N - 1 ; i++ ) {
		if (ui % 2 == 0 )
			toDiagCol++;
		else
			toDiagRow++;
		if (Matrix[(toDiagRow * N) + toDiagCol] > 0) Matrix[(toDiagRow * N) + toDiagCol] = -Matrix[(toDiagRow * N) + toDiagCol]; // Prevents Overlapping when tracing path
		ui = ui >> 1;
	}

	Matrix[(N - 1) * N + (N - 1)] = -Matrix [(N - 1) * N + (N - 1)] ; // Starting point - Last row Last column
	fromDiagRow = N - 1;
	fromDiagCol = N - 1;
	ui = bestPathCodeSuffix;
	
	for ( int i = 0 ; i < N - 1 ; i++ ) {
		if (ui % 2 == 0 )
			fromDiagCol--;
		else
			fromDiagRow--;
		if (Matrix[(fromDiagRow * N) + fromDiagCol] > 0) Matrix[(fromDiagRow * N) + fromDiagCol] = -Matrix[(fromDiagRow * N) + fromDiagCol]; // Prevents Overlapping when tracing path
		ui = ui >> 1; 
	}

	// Output best path sum and best path diagram
	displayBestPathSequential(Matrix, N, bestPathSum);

	// Free up memory
	delete [] Matrix;
	delete [] bestToDiagPathSum;
	delete [] bestFromDiagPathSum;
	delete [] bestToDiagPathCode;
	delete [] bestFromDiagPathCode;

}

void displayBestPathSequential(int *Matrix, int N, int bestPathSum) {
	// Allows for colors in cmd window
	CONSOLE_SCREEN_BUFFER_INFO *ConsoleInfo = new CONSOLE_SCREEN_BUFFER_INFO();
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), ConsoleInfo);
    WORD OriginalColors = ConsoleInfo->wAttributes;

	for (int row = 0 ; row < N ; row++) {
		for ( int col = 0 ; col < N ; col++)
			if (Matrix[row * N + col] < 0) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
				cout << -Matrix[row * N + col];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), OriginalColors);
			} else
				cout << "@";
		cout << endl;
	}
	cout << endl <<"Best Path Sum: " << bestPathSum << endl;
}

void setMatrixSequentialPath(int *Matrix) {
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