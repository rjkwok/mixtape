#ifndef REB_SHIP
#define REB_SHIP

#include "main.h"

using namespace std;
using namespace sf;


struct ShipProperties{

	ShipProperties();

	string type_name;
	string parent;
	int render_order;
	Vector2f relative_position;

	double max_fuel;
	double fuel_consumption;
	double max_speed;
	int max_gear; //acceleration "gear" multiplies by a game-wide constant to get actual acceleration magnitude

	string texture_id;
	string icon_id;

	map<string, int> start_index;
	map<string, int> end_index;
	map<string, bool> is_looping;
};

struct Ship{

	Ship();
	Ship(string c_type_name, Vector2f position);

	//defines the base type of the ship
	string type_name;
	//

	//variables that track the state of the ship
	string pilot_id;
	double fuel;
	Vector2f position;
	Vector2f velocity;
	Vector2f acceleration_vector; //direction of acceleration (normalized) determined by user/ai input
	bool antigrav_enabled;
	int acceleration_gear; 
	//

	FloatRect bounds;
	map<string, Sprite> sprite;
	map<string, map<string, Animation> > animation;
	map<string, string> current_animation_name;
	vector<string> upgrade_names;

	void recalculateBounds();
	void controlFromInput(InputStruct input);
	void updateSpritePositions();
	void update(double dt, Terrain &terrain);
	void draw(RenderWindow &window);

	bool hasUpgrade(string upgrade_name);
	double getMaxFuel();
	double getFuelConsumption();
	double getMaxSpeed();
	int getMaxGear();

	void upgrade(string upgrade_name);
};

#endif