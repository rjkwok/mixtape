#ifndef REB_CHAR
#define REB_CHAR

#include "main.h"

using namespace std;
using namespace sf;

struct CharacterProperties{

	CharacterProperties();

	string type_name;
	string texture_id;

	map<string, int> start_index;
	map<string, int> end_index;
	map<string, bool> is_looping;
};

struct Character{

	Character();
	Character(string c_name, string c_type_name, Vector2f position);

	string name; //unique identifier
	string type_name; //defines the properties of this character ie visuals

	string structure_id; //the id of the structure this character is inside, if any
	string ship_id; //the id of the ship this character is piloting, if any
	Sprite sprite; //all character position and rotation info is stored as this single sprite
	double dy; //y rate of change (velocity on y-axis only)
	double ddy; //rate of change of y rate of change (acceleration on y-axis only)

	string current_animation_name;
	map<string, Animation> animation;

	void update(double dt, Terrain &terrain);
	void moveWithShip(map<string, Ship*> &ships);
	void draw(RenderWindow &window);
};


#endif