#include <Tile.h>
#include <Grid.h>

Tile::Tile(Grid* grid,int x,int y) {
	Tile::Tile(grid,x,y,2);
}

Tile::Tile(Grid* grid,int x,int y, int val) {
	this->x = x;
	this->y = y;
	this->value = val;
	this->grid = grid;
}
