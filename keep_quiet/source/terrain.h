#ifndef REB_TERR
#define REB_TERR

#include "main.h"

using namespace std;
using namespace sf;

struct TileProperties{

	TileProperties();

	string type_name;
	int type_id;
	vector<int> texture_indexes;
};

struct Terrain{

	//these variables all describe properties of the "terrain" and are always used together, so they are passed together in this struct for the sake of making the code concise.

	Terrain();
	Terrain(Vector2f bottom_left_origin, double c_tile_size, int terrain_tiles_across, int terrain_tiles_up);

	map<int,map<int,int> > grid; //0 is no terrain, 1 is dirt, 2 is tunnel support, etc.
	int max_x;
    int max_y;
    Vector2f grid_ref; //terrain grid bottom left origin
    double tile_size;
    bool looping; //flags whether or not collision functions need to consider that this terrain loops back on itself

    VertexArray tiles; //one giant graphics "quilt" stitched together from all the different tiles. Only update on change to terrain.

    void updateTiles(); //restitches the vertexarray "quilt"
    void changeTile(int x, int y, int type_id);
    void setSurfaceY(double x, double y);
    double getSurfaceY(double x);
    void draw(RenderWindow &window);
	
};

#endif