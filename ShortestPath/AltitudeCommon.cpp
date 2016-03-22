#include "AltitudeCommon.h"

BestPath::BestPath() {
	prefix = 0;
	suffix = 0;
	sum = 0;
}

Boundaries::Boundaries() {
	length = 1;
	width = 1;
	length = 1;
}

Boundaries::Boundaries(unsigned int l, unsigned int w, unsigned int h) {
	length = l;
	width = w;
	length = h;
}

Coordinates::Coordinates() {
	x = 0;
	y = 0;
	z = 0;
}

Coordinates::Coordinates(unsigned int xCoord, unsigned int yCoord) {
	x = xCoord;
	y = yCoord;
	z = 0;
}

Coordinates::Coordinates(unsigned int xCoord, unsigned int yCoord, unsigned int zCoord) {
	x = xCoord;
	y = yCoord;
	z = zCoord;
}
