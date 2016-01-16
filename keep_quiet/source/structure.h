#ifndef REB_STRU
#define REB_STRU

#include "main.h"

using namespace std;
using namespace sf;

struct StructureProperties{

	StructureProperties();

	string type_name;
	string parent;
	int render_order;

	int max_workers; //a constant is used instead of this value when the structure is in "construction" state
	int max_ammunition;
	int max_fuel;
	int power_contribution;
	int construction_contribution;
	int supply_contribution;
	double fuel_consumption;
	double construction_cost;

	string texture_id;
	string icon_id;

	map<string, int> frame_names;

	IntRect getFrame(string frame_name);

};

struct Worker{

	//entity that can fill production slots in structures
	//controls a representation of itself, a rebel npc, that moves and animates according to worker's current task

	Worker();

	string tasked_structure_id;
};

struct Structure{

	//something that is initiated as an incomplete construction that must have workers tasked to it
	//evolves into (possible into an inherited class) upon completion, that may provide or draw power, construction rate, or food supply
	//may require workers to be tasked for production to take place after completion as well
	//some structures may have to buffer fuel or ammunition and require an interface for that to be loaded

	Structure();
	Structure(string c_type_name, Vector2f position);

	//defines a local copy of structure properties
	string type_name;
	//

	//variables that track the state of the structure
	bool contributing; //used by base when tallying all contributions to determine whether or not to tally contributions from this structure
	string construction_name; //identifies what upgrade is in construction
	double construction_progress;
	vector<Worker*> tasked_workers;
	//

	//buffered stuff
	int ammunition; 
	int fuel;
	//

	map<string, Sprite> sprite;
	vector<string> upgrade_names;

	void update(double dt, int total_construction, int surplus_power);
	void draw(RenderWindow &window);

	int getMaxWorkers();
	int getMaxFuel();
	int getMaxAmmunition();
	int getPowerContribution();
	int getSupplyContribution();
	int getConstructionContribution();
	double getFuelConsumption();

	void upgrade(string upgrade_name);
};

#endif