#include "main.h"

using namespace std;
using namespace sf;

extern map<string, Texture*> textures;
extern map<string, IntRect> texture_rects;
extern map<string, StructureProperties> structure_properties;

StructureProperties::StructureProperties(){

	type_name = "";
	grid_size = Vector2i(0,0);
	grid_origin = Vector2f(0,0);

	backdrop_texture_id = "";
	door_texture_id = "";
	exterior_texture_id = "";

	exterior_offset = Vector2f(0,0);
	backdrop_offset = Vector2f(0,0);
}

Structure::Structure(){}

Structure::Structure(string c_id, string c_type_name, Vector2f position){

	id = c_id;
	type_name = c_type_name;
	exterior_position = position;

	StructureProperties &prop = structure_properties[type_name]; //providing access to the properties via a shorter identifier in this function for the sake of brevity

	door = createSprite(prop.door_texture_id, exterior_position, "bottom middle");
	exterior_sprite = createSprite(prop.exterior_texture_id, exterior_position + prop.exterior_offset, "bottom middle");
	backdrop = createSprite(prop.backdrop_texture_id, prop.backdrop_offset, "bottom middle");

	//load the cell's preset terrain grid
	terrain = Terrain(prop.grid_origin, 32, prop.grid_size.x, prop.grid_size.y);
	for(map<int, map<int, int> >::iterator i = prop.grid.begin(); i != prop.grid.end(); i++){
		for(map<int, int>::iterator j = i->second.begin(); j != i->second.end(); j++){
			terrain.changeTile(i->first, j->first, j->second);
		}
	}
	//

	terrain.updateTiles();

	//load the preset decorations
	for(map<string, string>::iterator i = prop.deco_texture_id.begin(); i != prop.deco_texture_id.end(); i++){
		decorations[i->first] = createSprite(i->second, prop.deco_position[i->first], "middle");
	}
	//
}

void Structure::drawInterior(RenderWindow &window){

	window.draw(backdrop);
	door.setPosition(0,0); //the door is always drawn at the origin when inside a Structure
	window.draw(door);
	for(map<string, Sprite>::iterator i = decorations.begin(); i != decorations.end(); i++){
		window.draw(i->second);
	}
	terrain.draw(window);
}

void Structure::drawExterior(RenderWindow &window){
	
	window.draw(exterior_sprite);
	door.setPosition(exterior_position); //the door marks the exterior position when outdoors
	window.draw(door);
}