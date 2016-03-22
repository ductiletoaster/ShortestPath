/**
 * Author: Brian Gebel
 * Version: 09.10.2015
 * Description: Allows for extremely accurate timing on windows based systems.
 */

#ifndef _WINDOWSTIMER_H_INCLUDED_
#define _WINDOWSTIMER_H_INCLUDED_

struct StopWatch {
    LARGE_INTEGER start;
    LARGE_INTEGER stop;
};

class WindowsTimer {

	private:
		StopWatch timer;
		LARGE_INTEGER frequency;
		double LIToSecs( LARGE_INTEGER & L);

	public:
		WindowsTimer();
		void startTimer( );
		void stopTimer( );
		double getElapsedTime();
};

#endif