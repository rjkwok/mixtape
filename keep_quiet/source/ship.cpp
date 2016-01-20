#include "main.h"

using namespace std;
using namespace sf;

extern map<string, Texture*> textures;
extern map<string, IntRect> texture_rects;
extern map<string, ShipProperties> ship_properties;


ShipProperties::ShipProperties(){

    max_fuel = 0;
    fuel_consumption = 0;
    max_speed = 0;
    max_gear = 0;
    parent = "";
    render_order = 0;
}

Ship::Ship(){}

Ship::Ship(string c_type_name, Vector2f c_position){

	type_name = c_type_name;

    sprite[type_name] = createSprite(ship_properties[type_name].texture_id, c_position);

    for(map<string, int>::iterator i = ship_properties[type_name].start_index.begin(); i != ship_properties[type_name].start_index.end(); i++){

    	animation[type_name][i->first] = Animation(i->second, ship_properties[type_name].end_index[i->first], ship_properties[type_name].is_looping[i->first]);
    }
    current_animation_name[type_name] = "default";
    sprite[type_name].setTextureRect(getFrame(ship_properties[type_name].start_index["default"], sprite[type_name]));

    fuel = 0;

    recalculateBounds();

}

double Ship::getMaxFuel(){

	double total = ship_properties[type_name].max_fuel;
	for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
		total += ship_properties[*i].max_fuel;
	}
	return total;
}
double Ship::getFuelConsumption(){

	double total = ship_properties[type_name].fuel_consumption;
	for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
		total += ship_properties[*i].fuel_consumption;
	}
	return total;
}
double Ship::getMaxSpeed(){

	double total = ship_properties[type_name].max_speed;
	for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
		total += ship_properties[*i].max_speed;
	}
	return total;
}
int Ship::getMaxGear(){

	int total = ship_properties[type_name].max_gear;
	for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
		total += ship_properties[*i].max_gear;
	}
	return total;
}


void Ship::recalculateBounds(){

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

void Ship::upgrade(string upgrade_name){

	upgrade_names.push_back(upgrade_name);

    sprite[upgrade_name] = createSprite(ship_properties[upgrade_name].texture_id, Vector2f(sprite[type_name].getPosition().x, sprite[type_name].getPosition().y - ((getTextureRect(ship_properties[upgrade_name].texture_id).height - getTextureRect(ship_properties[type_name].texture_id).height)/2.0)));
    for(map<string, int>::iterator i = ship_properties[upgrade_name].start_index.begin(); i != ship_properties[upgrade_name].start_index.end(); i++){

    	animation[upgrade_name][i->first] = Animation(i->second, ship_properties[upgrade_name].end_index[i->first], ship_properties[upgrade_name].is_looping[i->first]);
    }
    current_animation_name[upgrade_name] = "default";
    sprite[upgrade_name].setTextureRect(getFrame(ship_properties[upgrade_name].start_index["default"], sprite[upgrade_name]));

    recalculateBounds();

}

bool Ship::hasUpgrade(string upgrade_name){

	for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
		if(*i == upgrade_name){ return true; }
	}
	return false;
}

void Ship::update(double dt){

	double acceleration_per_gear = 10;

	//run animations
    for(map<string,string>::iterator i = current_animation_name.begin(); i != current_animation_name.end(); i++){

    	if(!animation[i->first][i->second].play(dt, sprite[i->first])){
    		//if animation has stopped playing
    		i->second = "default";
    	}
    }
    //

    //update position and its derivatives
    double acceleration_magnitude = acceleration_gear*acceleration_per_gear;
    Vector2f acceleration = acceleration_vector*acceleration_magnitude;
    Vector2f gravity = Vector2f(0,3000);
    double required_fuel = acceleration_magnitude*getFuelConsumption()*dt;
    if(required_fuel >= fuel){
    	fuel -= required_fuel;
    	velocity = velocity + (acceleration*dt);
    }
    velocity = velocity + (gravity*dt);
    position = position + (velocity*dt);
    //
    
    //update sprites relative to the (possibly) new position

    //
}

void Ship::draw(RenderWindow &window){

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
					current_bottom_layer = ship_properties[i->first].render_order;
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
				if(ship_properties[i->first].render_order < current_bottom_layer){
					current_bottom_layer = ship_properties[i->first].render_order;
					name_to_draw = i->first;
				}
			}
		}
		window.draw(sprite[name_to_draw]);
		already_drawn.insert(name_to_draw);
	}
    
}