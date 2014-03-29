#ifndef __GRID__
#define __GRID__

#include <vector>

class Tile;

class Grid {
	public:
		Grid(int x,int y);
	private:
		int x;
		int y;
		std::vector< std::vector< Tile* > > tiles;
};

#endif