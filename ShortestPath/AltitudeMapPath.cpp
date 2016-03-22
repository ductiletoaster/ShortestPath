#include <thread>
#include <iostream>
#include "AltitudeCommon.h"
#include "AltitudeMap.h"
#include "AltitudeMapPath.h"

AltitudeMapPath::AltitudeMapPath(const AltitudeMap &map) {
	pathedMap = map;
	numRows = pathedMap.length();
	numCols = pathedMap.width();
	beyondMaxSum = (AM_MAXSIZE * numRows * numRows) + 1;
}

BestPath AltitudeMapPath::findBestPath(bool allowMultiThreading) {
	unsigned int threadCount;
	unsigned int threadIndex;

	// Figure out the number of threads we can spawn
	threadCount = std::thread::hardware_concurrency();
	if (threadCount && allowMultiThreading) {
		// Needs to be even (should be already but just in case)
		threadCount = (threadCount % 2 == 0) ? threadCount : (threadCount - 1);

		// Shared vectors
		bestToPathSum.assign(threadCount, std::vector<unsigned int>(numRows, beyondMaxSum));
		bestFromPathSum.assign(threadCount, std::vector<unsigned int>(numRows, beyondMaxSum));
		bestToPathCode.assign(threadCount, std::vector<unsigned int>(numRows, 0));
		bestFromPathCode.assign(threadCount, std::vector<unsigned int>(numRows, 0));

		// Create threads
		std::vector<std::thread> threads;
		for (threadIndex = 0; threadIndex < threadCount; threadIndex++) {
			threads.push_back(std::thread(&AltitudeMapPath::bestPath, this, threadCount, threadIndex));
		}

		// Return when all threads after they have completed
		for (auto& thread : threads) {
			thread.join();
		}
	}
	else {

		// Shared vectors
		bestToPathSum.assign(1, std::vector<unsigned int>(numRows, beyondMaxSum));
		bestFromPathSum.assign(1, std::vector<unsigned int>(numRows, beyondMaxSum));
		bestToPathCode.assign(1, std::vector<unsigned int>(numRows, 0));
		bestFromPathCode.assign(1, std::vector<unsigned int>(numRows, 0));

		// No need to create threads just call directly
		threadCount = 1;
		threadIndex = 0;
		bestPath(threadCount, threadIndex);
	}

	std::vector<unsigned int> combinedBestToPathSum(numRows, beyondMaxSum);
	std::vector<unsigned int> combinedBestFromPathSum(numRows, beyondMaxSum);
	std::vector<unsigned int> combinedBestToPathCode(numRows, 0);
	std::vector<unsigned int> combinedBestFromPathCode(numRows, 0);

	// Combined thread results
	for (threadIndex = 0; threadIndex < threadCount; threadIndex++) {
		for (unsigned int row = 0; row < numRows; row++) {
			if (bestToPathSum[threadIndex][row] < combinedBestToPathSum[row]) {
				combinedBestToPathSum[row] = bestToPathSum[threadIndex][row];
				combinedBestToPathCode[row] = bestToPathCode[threadIndex][row];
			}
			if (bestFromPathSum[threadIndex][row] < combinedBestFromPathSum[row]) {
				combinedBestFromPathSum[row] = bestFromPathSum[threadIndex][row];
				combinedBestFromPathCode[row] = bestFromPathCode[threadIndex][row];
			}
		}
	}

	// Determine the best of the returned BestPath
	unsigned int tempSum;
	Coordinates bestDiagCoord;
	BestPath path;
	path.sum = beyondMaxSum;

	unsigned int col = numCols - 1;
	for (unsigned int row = 0; row < numRows; row++) {
		bestDiagCoord = pathedMap(row, col--);
		tempSum = combinedBestToPathSum[row] + combinedBestFromPathSum[row] - bestDiagCoord.z;
		if (tempSum < path.sum) {
			path.prefix = combinedBestToPathCode[row];
			path.suffix = combinedBestFromPathCode[row];
			path.sum = tempSum;
		}
	}
	return path;
}

void AltitudeMapPath::bestPath(unsigned int threadCount, unsigned int threadIndex) {
	unsigned int tempPathCode;
	unsigned int toPathSum, toRow, toCol;
	unsigned int fromPathSum, fromRow, fromCol;

	Coordinates startCoord, endCoord, toCoord, fromCoord;

	// To and From split matrix in half along the diagonal
	unsigned int currentPathCode = threadIndex;
	unsigned int maxPathCode = ((unsigned int)1 << (numRows - 1));
	while (currentPathCode < maxPathCode) {
		tempPathCode = currentPathCode;

		// Setup to path iteration
		startCoord = pathedMap(0, 0);
		toPathSum = startCoord.z;
		toRow = 0;
		toCol = 0;

		// Setup from path iteration 
		endCoord = pathedMap(numRows - 1, numCols - 1);
		fromPathSum = endCoord.z;
		fromRow = numRows - 1;
		fromCol = numCols - 1;

		for (unsigned int index = 0; index < numRows - 1; index++) {
			if (tempPathCode % 2 == 0) {
				toCol++;
				fromCol--;
			}
			else {
				toRow++;
				fromRow--;
			}
			toCoord = pathedMap(toRow, toCol);
			toPathSum += toCoord.z;
			fromCoord = pathedMap(fromRow, fromCol);
			fromPathSum += fromCoord.z;
			tempPathCode = tempPathCode >> 1;
		}

		if (toPathSum < bestToPathSum[threadIndex][toRow]) {
			bestToPathSum[threadIndex][toRow] = toPathSum;
			bestToPathCode[threadIndex][toRow] = currentPathCode;
		}

		if (fromPathSum < bestFromPathSum[threadIndex][fromRow]) {
			bestFromPathSum[threadIndex][fromRow] = fromPathSum;
			bestFromPathCode[threadIndex][fromRow] = currentPathCode;
		}
		currentPathCode += threadCount;
	}
}
