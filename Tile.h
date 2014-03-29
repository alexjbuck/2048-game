#ifndef __TILE__
#define __TILE__

class Grid;

class Tile {
	public:
		Tile(Grid* grid,int x,int y);
		Tile(Grid* grid,int x,int y,int val);
	private:
		int x;
		int y;
		int value;
		Grid* grid;
};

#endif