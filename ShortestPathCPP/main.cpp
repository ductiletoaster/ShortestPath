/**
 * Author: Brian Gebel
 * Version: 09.13.2011
 * Description: Initializes testing for both sequnetial and parallel versions
 */
#include "main.h"

int main() {
	double elapsedTimeOne, elapsedTimeTwo;
	unsigned int expectedSum;
	BestPath bestPathOne, bestPathTwo;
	WindowsTimer timerOne, timerTwo;

	// Runs testing for MIN < N > MAX
	for (unsigned int numberOfElements = MIN; numberOfElements <= MAX; numberOfElements++) {

		// Create known path for program to find
		AltitudeMap testMap = AltitudeMap(numberOfElements);
		for (unsigned int row = 0; row < numberOfElements; row++) {
			for (unsigned int col = 0; col < numberOfElements; col++) {
				if (row == 0 || col == numberOfElements - 1) {
					testMap(row, col) = Coordinates(row, col, 1);
				}
				else {
					testMap(row, col) = Coordinates(row, col, 9);
				}
			}
		}
		AltitudeMapPath sPath = AltitudeMapPath(testMap);

		// Print test details in console
		printTestDetails(testMap);

		// Perform Test - Sequential
		timerOne.startTimer();
		bestPathOne = sPath.findBestPath(AM_MUTLITHREADING_OFF);
		timerOne.stopTimer();

		// Perform Test - Parallel
		timerTwo.startTimer();
		bestPathTwo = sPath.findBestPath(AM_MUTLITHREADING_ON);
		timerTwo.stopTimer();

		// Get result data
		elapsedTimeOne = timerOne.getElapsedTime();
		elapsedTimeTwo = timerTwo.getElapsedTime();
		expectedSum = (numberOfElements + numberOfElements) - 1;

		// Print results
		printTestResults(elapsedTimeOne, elapsedTimeTwo, expectedSum, bestPathOne.sum, bestPathTwo.sum);

		// Wait for user input before program continues
		wait("Press any key to continue.");
	}
	wait("Press any key to exit program.");
	return 0;
}

void printTestDetails(AltitudeMap map) {
	unsigned int length = map.length();
	unsigned int width = map.width();

	CONSOLE_SCREEN_BUFFER_INFO *ConsoleInfo = new CONSOLE_SCREEN_BUFFER_INFO();
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), ConsoleInfo);
	WORD OriginalColors = ConsoleInfo->wAttributes;

	std::cout << std::endl << "----------------------------------" << std::endl;
	std::cout << "Starting Sequential Test " << length << "x" << width;
	std::cout << std::endl << "----------------------------------" << std::endl;

	for (unsigned int row = 0; row < length; row++) {
		for (unsigned int col = 0; col < width; col++) {
			if (row == 0 || col == width - 1) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
				std::cout << "#";
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), OriginalColors);
			}
			else {
				std::cout << "@";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void printTestResults(double elapsedTimeOne, double elapsedTimeTwo, unsigned int expectedSum, unsigned int actualSumOne, unsigned int actualSumTwo) {
	std::cout << std::endl;
	std::cout << "Total time in seconds (S): " << std::fixed << elapsedTimeOne << std::endl;
	std::cout << "Total time in seconds (P): " << std::fixed << elapsedTimeTwo << std::endl;
	std::cout << "Actual Sum (S): " << actualSumOne << std::endl;
	std::cout << "Actual Sum (P): " << actualSumTwo << std::endl;
	std::cout << "Expected Sum: " << expectedSum << std::endl;
	std::cout << std::endl;
}

void wait(char * msg) {
    char buffer;
	std::cout << std::endl << msg << std::endl;
	std::cin.get(buffer);
}