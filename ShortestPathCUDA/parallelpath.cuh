/**
 * Author: Brian Gebel
 * Version: 12.10.2011
 * Description:
 */

#ifndef _PARALLELPATH_H_INCLUDED_
#define _PARALLELPATH_H_INCLUDED_

	// Constant Variables
	const int maxBlocks = 65535; // Max 65535
	const int threadsPerBlock = 512; // Max 512

	// Methods
	void findBestPathKernel(int *, int *, int *, unsigned int *, unsigned int *, int);
	void findBestPathParallel(int);
	void displayBestPathParallel(int *, int, int);
	void setMatrixParallelPath(int *);

#endif