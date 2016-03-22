/**
* Author: Brian Gebel
* Version: 09.17.2015
* Description:
*/

#ifndef _ALTITUDEMAPPATH_H_INCLUDED_
#define _ALTITUDEMAPPATH_H_INCLUDED_

class AltitudeMapPath {
	private:
		AltitudeMap pathedMap;
		unsigned int numRows;
		unsigned int numCols;
		unsigned int beyondMaxSum;
		std::vector<std::vector<unsigned int>> bestToPathSum;
		std::vector<std::vector<unsigned int>> bestFromPathSum;
		std::vector<std::vector<unsigned int>> bestToPathCode;
		std::vector<std::vector<unsigned int>> bestFromPathCode;

	public:
		AltitudeMapPath(const AltitudeMap &);
		BestPath findBestPath(bool);
		void bestPath(unsigned int, unsigned int);
};

#endif