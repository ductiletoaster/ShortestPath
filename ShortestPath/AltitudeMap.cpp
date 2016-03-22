#include "AltitudeCommon.h"
#include "AltitudeMap.h"

AltitudeMap::AltitudeMap() {
	// Creates smallest default map
	init(AM_DEFAULTLENGTH, AM_DEFAULTWIDTH, AM_DEFAULTHEIGHT);
}

AltitudeMap::AltitudeMap(unsigned int length) {
	// Creates a cuboid map with uniform base and height
	init(length, length, length);
}

AltitudeMap::AltitudeMap(unsigned int length, unsigned int height) {
	// Creates a cuboid map with square base
	init(length, length, height);
}

AltitudeMap::AltitudeMap(unsigned int length, unsigned int width, unsigned int height) {
	// Creates a cuboid map with rectangular base
	init(length, width, height);
}

void AltitudeMap::init(unsigned int length, unsigned int width, unsigned int height) {
	index = 0;
	bound.length = length;
	bound.width = width;
	bound.height = height;

	// Size vector
	Coordinates newCoord = Coordinates();
	coord.assign(bound.length * bound.width, newCoord);
	
	// Fill vector with 0 height -- Could use iterators here
	for (unsigned int row = 0; row < bound.length; row++) {
		for (unsigned int col = 0; col < bound.width; col++) {
			coord[row * bound.length + col] = Coordinates(row, col);
		}
	}
}

Coordinates& AltitudeMap::operator()(unsigned int row, unsigned int col) {
	if (row > bound.length - 1 || col > bound.width - 1)
		throw std::invalid_argument("coordinate out of bounds");
	return coord[row * bound.length + col]; // Row col order
}

int AltitudeMap::length() {
	return bound.length;
}

int AltitudeMap::width() {
	return bound.width;
}

int AltitudeMap::height() {
	return bound.height;
}