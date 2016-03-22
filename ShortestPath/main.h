/**
 * Author: Brian Gebel
 * Version: 09.17.2015
 * Description: Contains definitions for main.cpp
 */

#ifndef _MAIN_H_INCLUDED_
#define _MAIN_H_INCLUDED_

#include <iostream>
#include <stdio.h>
#include <windows.h>

#include "WindowsTimer.h"
#include "AltitudeCommon.h"
#include "AltitudeMap.h"
#include "AltitudeMapPath.h"

static const unsigned int MIN = 12;
static const unsigned int MAX = 24;

int main();
void printTestDetails(AltitudeMap);
void printTestResults(double, double, unsigned int, unsigned int, unsigned int);
void wait(char *);
	
#endif
