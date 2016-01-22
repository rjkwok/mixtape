#ifndef REB_STRU
#define REB_STRU

#include "main.h"

using namespace std;
using namespace sf;

struct StructureProperties{

	StructureProperties();

	string type_name;
	
	map<int, map<int, int> > grid;
	Vector2i grid_size;
	Vector2f grid_origin;

	string backdrop_texture_id;
	string door_texture_id;
	string exterior_texture_id;

	Vector2f exterior_offset;
	Vector2f backdrop_offset;

	map<string, Vector2f> deco_position; //gives the 2d coordinate within the cell that the decoration is centered at
	map<string, string> deco_texture_id; //gives the texture id that the decoration will need to build itself from
};

struct Structure{

	Structure();
	Structure(string c_id, string c_type_name, Vector2f position);

	//defines the structure within the world
	string id;
	string type_name;
	Vector2f exterior_position;
	//

	//renders at the exterior position when outside the structure, and at the origin when inside the structure
	Sprite door; //the door provides the interface for switching between being inside and outside a structure
	//

	//renders as the outside of the structure (at the door position + relative exterior position)
	Sprite exterior_sprite;
	//

	//renders as the inside of the structure
	Terrain terrain; //all tiles default to air unless otherwise specified
	Sprite backdrop;
	map<string,Sprite> decorations; //these don't collide but are separate from the backdrop so that they can be selected if need be
	VertexArray tiles;
	//

	void drawInterior(RenderWindow &window);
	void drawExterior(RenderWindow &window);

};


#endif