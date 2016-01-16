#include "main.h"

using namespace std;
using namespace sf;

extern map<string, Texture*> textures;
extern map<string, IntRect> texture_rects;
extern map<string, StructureProperties> structure_properties;

StructureProperties::StructureProperties(){

    max_workers = 0;
    max_ammunition = 0;
    max_fuel = 0;
    power_contribution = 0;
    construction_contribution = 0;
    supply_contribution = 0;
    fuel_consumption = 0;
    construction_cost = 0;
    parent = "";
    render_order = 0;
}

IntRect StructureProperties::getFrame(string frame_name){

	int index = frame_names[frame_name];
	IntRect frame_window = getTextureRect(texture_id);
    frame_window.left = frame_window.width*(int(index * frame_window.width) % int(getTexture(texture_id)->getSize().x))/double(frame_window.width);
    frame_window.top = frame_window.height*(((index - (int(index * frame_window.width) % int(getTexture(texture_id)->getSize().x))/double(frame_window.width))*frame_window.width)/getTexture(texture_id)->getSize().x);
    return frame_window;
}

Worker::Worker(){

    tasked_structure_id = "";
}

Structure::Structure(){}

Structure::Structure(string c_type_name, Vector2f c_position){

	type_name = c_type_name;

    contributing = false;
    construction_name = type_name;
    construction_progress = 0;

    sprite[type_name] = createSprite(structure_properties[type_name].texture_id, c_position);
    sprite[type_name].setTextureRect(structure_properties[type_name].getFrame("construction"));
    
    ammunition = 0;
    fuel = 0;

}

int Structure::getMaxWorkers(){

	int total = structure_properties[type_name].max_workers;
	for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
		total += structure_properties[*i].max_workers;
	}
	return total;
}
int Structure::getMaxFuel(){

	int total = structure_properties[type_name].max_fuel;
	for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
		total += structure_properties[*i].max_fuel;
	}
	return total;
}
int Structure::getMaxAmmunition(){

	int total = structure_properties[type_name].max_ammunition;
	for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
		total += structure_properties[*i].max_ammunition;
	}
	return total;
}
int Structure::getPowerContribution(){

	int total = structure_properties[type_name].power_contribution;
	for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
		total += structure_properties[*i].power_contribution;
	}
	return total;
}
int Structure::getSupplyContribution(){

	int total = structure_properties[type_name].supply_contribution;
	for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
		total += structure_properties[*i].supply_contribution;
	}
	return total;
}
int Structure::getConstructionContribution(){

	int total = structure_properties[type_name].construction_contribution;
	for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
		total += structure_properties[*i].construction_contribution;
	}
	return total;
}
double Structure::getFuelConsumption(){

	double total = structure_properties[type_name].fuel_consumption;
	for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
		total += structure_properties[*i].fuel_consumption;
	}
	return total;
}

void Structure::upgrade(string upgrade_name){

	upgrade_names.push_back(upgrade_name);

	contributing = false;
    construction_name = upgrade_name;
    construction_progress = 0;

    sprite[upgrade_name] = createSprite(structure_properties[upgrade_name].texture_id, sprite[type_name].getPosition());
    sprite[upgrade_name].setTextureRect(structure_properties[upgrade_name].getFrame("construction"));

}

void Structure::update(double dt, int total_construction, int surplus_power){

	//display tasked/max workers above structure
    for(int index = 0; index < getMaxWorkers(); index++){

        if(index < tasked_workers.size()){
            //display stick-man as filled instead of hollow to indicate the tasked worker
        }
    }
    //

    //if construction of the structure has not been completed:
    if(construction_name != ""){

    	contributing = false;

    	//display tasked/max workers above structure
        int max_construction_workers = 4;
        for(int index = 0; index < max_construction_workers; index++){

            if(index < tasked_workers.size()){
                //display stick-man as filled instead of hollow to indicate the tasked worker
            }
        }
        //
        double construction_rate = total_construction * tasked_workers.size();
        construction_progress += (construction_rate*dt);

        if(construction_progress >= structure_properties[construction_name].construction_cost){
	        //if progress is completed set texture to complete texture
	        sprite[construction_name].setTextureRect(structure_properties[construction_name].getFrame("default"));

	        //also dismiss all construction workers
	        for(vector<Worker*>::iterator i = tasked_workers.begin(); i != tasked_workers.end(); i++){
	            Worker* w = *i;
	            w->tasked_structure_id = "";
	        }
	        construction_progress = 0;
	        construction_name = "";
	        tasked_workers.clear();
	        //
	    }
	    return; //don't bother with the rest of the function since the structure is under construction
    }

    

    //display power, construction and food contributions if non-zero. If power is required but not provided (power_contribution < 0, base power < required) or max_workers > 0 and no workers are tasked or fuel_consumption*dt > fuel, then display contributions in grey
    contributing = !(getPowerContribution() < 0 && surplus_power < 0) && !(getMaxWorkers() > 0 && tasked_workers.size() == 0) && !(getFuelConsumption()*dt > fuel);
    if(contributing){
        int index = 0; //this tracks which contributions were displayed so that the subsequent one can shift over to make room
        if(getPowerContribution() != 0){
            index++;
        }
        if(getConstructionContribution() != 0){
            index++;
        }
        if(getSupplyContribution() != 0){

        }
    }
    //
    
}

void Structure::draw(RenderWindow &window){

	window.draw(sprite[type_name]);

	//draws sprite from lowest-to-highest render order via lookup (no sorted data structure)
	set<string> already_drawn;
	while(already_drawn.size() < upgrade_names.size()){
		int index_to_draw = -1;
		int current_bottom_layer = -1;
		for(int index = 0; index < upgrade_names.size(); index++){
			if(already_drawn.count(upgrade_names[index]) == 0){
				index_to_draw = index;
				current_bottom_layer = structure_properties[upgrade_names[index]].render_order;
				break;
			}
		}
		for(int index = 0; index < upgrade_names.size(); index++){
			if(already_drawn.count(upgrade_names[index]) != 0){ continue; }
			if(structure_properties[upgrade_names[index]].render_order < current_bottom_layer){
				current_bottom_layer = structure_properties[upgrade_names[index]].render_order;
				index_to_draw = index;
			}
		}
		window.draw(sprite[upgrade_names[index_to_draw]]);
		already_drawn.insert(upgrade_names[index_to_draw]);
	}
    
}