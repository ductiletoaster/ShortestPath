/**
* Author: Brian Gebel
* Version: 09.13.2011
* Description:
*/

#ifndef _ALTITUDECOMMON_H_INCLUDED_
#define _ALTITUDECOMMON_H_INCLUDED_

#include <vector>
#include <stdexcept>

#define AM_MUTLITHREADING_OFF 0
#define AM_MUTLITHREADING_ON 1

static const unsigned int AM_MAXSIZE = 256;
static const unsigned int AM_DEFAULTLENGTH = 1;
static const unsigned int AM_DEFAULTWIDTH = 1;
static const unsigned int AM_DEFAULTHEIGHT = 1;

static const unsigned int AM_MINTHREADS = 1;
static const unsigned int AM_MAXTHREADS = 256;

struct BestPath {
	unsigned int prefix;
	unsigned int suffix;
	unsigned int sum;
	BestPath();
};

struct Boundaries {
	unsigned int length;
	unsigned int width;
	unsigned int height;
	Boundaries();
	Boundaries(unsigned int, unsigned int, unsigned int);
};

struct Coordinates {
	unsigned int x;
	unsigned int y;
	unsigned int z;
	Coordinates();
	Coordinates(unsigned int, unsigned int);
	Coordinates(unsigned int, unsigned int, unsigned int);
};

#endif