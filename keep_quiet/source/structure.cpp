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

Worker::Worker(){

    tasked_structure_id = "";
}

SmartWorker::SmartWorker(){

    tasked_structure_id = "";
}

SmartWorker::SmartWorker(Sprite* c_sprite){

	sprite = c_sprite;
	tasked_structure_id = "";
}

void SmartWorker::update(double dt, map<string, Structure*> &structures){

	if(tasked_structure_id != ""){

		Vector2f target = Vector2f(structures[tasked_structure_id]->bounds.left + (structures[tasked_structure_id]->bounds.width/2.0),structures[tasked_structure_id]->bounds.top + structures[tasked_structure_id]->bounds.height);
		Vector2f direction = normalize(target - sprite->getPosition());
		double speed = 400;
		Vector2f displacement = direction * (speed * dt);
		sprite->move(displacement.x,displacement.y);
	}
	else{
		Vector2f direction = normalize(Vector2f(randSign()*randInt(30),randSign()*randInt(30)));
		double speed = 400;
		Vector2f displacement = direction * (speed * dt);
		sprite->move(displacement.x,displacement.y);
	}
}

Structure::Structure(){}

Structure::Structure(string c_type_name, Vector2f c_position){

	type_name = c_type_name;

    contributing = false;
    construction_name = type_name;
    construction_progress = 0;

    sprite[type_name] = createSprite(structure_properties[type_name].texture_id, c_position);

    for(map<string, int>::iterator i = structure_properties[type_name].start_index.begin(); i != structure_properties[type_name].start_index.end(); i++){

    	animation[type_name][i->first] = Animation(i->second, structure_properties[type_name].end_index[i->first], structure_properties[type_name].is_looping[i->first]);
    }
    current_animation_name[type_name] = "construction";
    sprite[type_name].setTextureRect(getFrame(structure_properties[type_name].start_index["construction"], sprite[type_name]));

    ammunition = 0;
    fuel = 0;

    recalculateBounds();

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

void Structure::recalculateBounds(){

	double top = sprite[type_name].getGlobalBounds().top;
	double left = sprite[type_name].getGlobalBounds().left;
	double bottom = top + sprite[type_name].getGlobalBounds().height;
	double right = left + sprite[type_name].getGlobalBounds().width;

	for(map<string,Sprite>::iterator i = sprite.begin(); i != sprite.end(); i++){
		if(i->first == type_name){ continue; } //all the values are initialized off this item so no point comparing against this item

		FloatRect rect = i->second.getGlobalBounds();
		if(rect.top < top){ top = rect.top; }
		if(rect.top + rect.height > bottom){ bottom = rect.top + rect.height; }
		if(rect.left < left){ left = rect.left; }
		if(rect.left + rect.width > right){ right = rect.left + rect.width; }
	}

	bounds = FloatRect(left, top, right-left, bottom-top);
}

void Structure::upgrade(string upgrade_name){

	dismissWorkers();

	upgrade_names.push_back(upgrade_name);

	contributing = false;
    construction_name = upgrade_name;
    construction_progress = 0;

    sprite[upgrade_name] = createSprite(structure_properties[upgrade_name].texture_id, Vector2f(sprite[type_name].getPosition().x, sprite[type_name].getPosition().y - ((getTextureRect(structure_properties[upgrade_name].texture_id).height - getTextureRect(structure_properties[type_name].texture_id).height)/2.0)));
    for(map<string, int>::iterator i = structure_properties[upgrade_name].start_index.begin(); i != structure_properties[upgrade_name].start_index.end(); i++){

    	animation[upgrade_name][i->first] = Animation(i->second, structure_properties[upgrade_name].end_index[i->first], structure_properties[upgrade_name].is_looping[i->first]);
    }
    current_animation_name[upgrade_name] = "construction";
    sprite[upgrade_name].setTextureRect(getFrame(structure_properties[upgrade_name].start_index["construction"], sprite[upgrade_name]));

    recalculateBounds();

}

bool Structure::hasUpgrade(string upgrade_name){

	for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
		if(*i == upgrade_name){ return true; }
	}
	return false;
}

void Structure::dismissWorkers(){

	for(vector<Worker*>::iterator i = tasked_workers.begin(); i != tasked_workers.end(); i++){
        Worker* w = *i;
        w->tasked_structure_id = "";
    }
	tasked_workers.clear();
}

void Structure::update(double dt, int total_construction, int surplus_power){

	//run animations
    for(map<string,string>::iterator i = current_animation_name.begin(); i != current_animation_name.end(); i++){

    	if(i->second == "construction"){ continue; } //construction animation is executed at a special fps further down in this function
    	if(!animation[i->first][i->second].play(dt, sprite[i->first])){
    		//if animation has stopped playing
    		i->second = "default";
    	}
    }
    //

    //if construction of the structure has not been completed:
    if(construction_name != ""){

    	contributing = false;

        double construction_rate = total_construction * tasked_workers.size();
        construction_progress += (construction_rate*dt);

        int total_frames = (animation[construction_name]["construction"].end_frame - animation[construction_name]["construction"].start_frame) + 1;
        double progress_per_frame = structure_properties[construction_name].construction_cost/total_frames;
        double construction_fps = (construction_rate/progress_per_frame);
        animation[construction_name]["construction"].play(dt, sprite[construction_name], construction_fps);

        if(construction_progress >= structure_properties[construction_name].construction_cost){
	        //if progress is completed set animation to default
	        current_animation_name[construction_name] = "default";

	        //also dismiss all construction workers
	        dismissWorkers();

	        //end and reset the construction process
	        construction_progress = 0;
	        construction_name = "";

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

	//draws sprite from lowest-to-highest render order via lookup (no sorted data structure)
	set<string> already_drawn;
	while(already_drawn.size() < sprite.size()){
		string name_to_draw = "";
		int current_bottom_layer = -1;
		for(map<string, Sprite>::iterator i = sprite.begin(); i != sprite.end(); i++){
			if(already_drawn.count(i->first) == 0){
				name_to_draw = i->first;
				if(i->first == type_name){ current_bottom_layer = 1;}
				else{
					current_bottom_layer = structure_properties[i->first].render_order;
				}
				break;
			}
		}
		for(map<string, Sprite>::iterator i = sprite.begin(); i != sprite.end(); i++){
			if(already_drawn.count(i->first) != 0){ continue; }
			if(i->first == type_name){
				if(1 < current_bottom_layer){
					current_bottom_layer = 1;
					name_to_draw = type_name;
				}
			}
			else{
				if(structure_properties[i->first].render_order < current_bottom_layer){
					current_bottom_layer = structure_properties[i->first].render_order;
					name_to_draw = i->first;
				}
			}
		}
		window.draw(sprite[name_to_draw]);
		already_drawn.insert(name_to_draw);
	}
    
}