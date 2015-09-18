#include <windows.h>
#include "WindowsTimer.h"

double WindowsTimer::LIToSecs(LARGE_INTEGER & L) {
	return ((double)L.QuadPart /(double)frequency.QuadPart);
}

WindowsTimer::WindowsTimer() {
	timer.start.QuadPart=0;
	timer.stop.QuadPart=0;	
	QueryPerformanceFrequency(&frequency);
}

void WindowsTimer::startTimer() {
    QueryPerformanceCounter(&timer.start);
}

void WindowsTimer::stopTimer() {
    QueryPerformanceCounter(&timer.stop);
}

double WindowsTimer::getElapsedTime() {
	LARGE_INTEGER time;
	time.QuadPart = timer.stop.QuadPart - timer.start.QuadPart;
    return LIToSecs(time) ;
}
