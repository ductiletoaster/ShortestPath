/**
* Author: Brian Gebel
* Version: 09.13.2011
* Description: Creates an integer matrix representing an altitude map
*/

#ifndef _ALTITUDEMAP_H_INCLUDED_
#define _ALTITUDEMAP_H_INCLUDED_

class AltitudeMap {
	private:
		unsigned int index;
		Boundaries bound;
		std::vector<Coordinates> coord;
		void init(unsigned int, unsigned int, unsigned int);
		
	public:
		AltitudeMap();
		AltitudeMap(unsigned int);
		AltitudeMap(unsigned int, unsigned int);
		AltitudeMap(unsigned int, unsigned int, unsigned int);
		Coordinates& operator()(unsigned int, unsigned int);
		int length();
		int width();
		int height();
};

#endif