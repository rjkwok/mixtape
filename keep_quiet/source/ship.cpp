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
    handling = 1.0;
    parent = "";
    render_order = 0;
    relative_position = Vector2f(0,0);
}

Ship::Ship(){}

Ship::Ship(string c_type_name, Vector2f c_position){

	type_name = c_type_name;

    sprite[type_name] = createSprite(ship_properties[type_name].texture_id, c_position + ship_properties[type_name].relative_position);

    for(map<string, int>::iterator i = ship_properties[type_name].start_index.begin(); i != ship_properties[type_name].start_index.end(); i++){

    	animation[type_name][i->first] = Animation(i->second, ship_properties[type_name].end_index[i->first], ship_properties[type_name].is_looping[i->first]);
    }
    current_animation_name[type_name] = "default";
    sprite[type_name].setTextureRect(getFrame(ship_properties[type_name].start_index["default"], sprite[type_name]));

    fuel = 0;
    acceleration_gear = 0;
    antigrav_enabled = false;
    position = c_position;
    velocity = Vector2f(0,0);
    acceleration_vector = Vector2f(1,0);
    target_vector = Vector2f(0,0);

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

	//fuel consumption is in litres, but this function returns kilolitres
	double total = ship_properties[type_name].fuel_consumption;
	for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
		total += ship_properties[*i].fuel_consumption;
	}
	return total/1000;
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
double Ship::getHandling(){

	//returns the time in seconds it takes for the ship to do a 180 change in the acceleration vector

	double total = ship_properties[type_name].handling;
	for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
		total += ship_properties[*i].handling;
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

void Ship::controlFromInput(InputStruct input){

	Vector2f direction = Vector2f(0,0);

	if(input.keys_held.count("a") != 0){
		direction = direction + Vector2f(-1,-0.1);
	}
	if(input.keys_held.count("d") != 0){
		direction = direction + Vector2f(1,-0.1);
	}
	if(input.keys_held.count("w") != 0){
		direction = direction + Vector2f(0.01,-1);
	}
	if(input.keys_held.count("s") != 0){
		direction = direction + Vector2f(0.01,1);
	}

	if(direction.x == 0 && direction.y == 0){
		target_vector = direction;
	}
	else{
		target_vector = normalize(direction);
	}
	
	if(input.keys_released.count("g") != 0){
		antigrav_enabled = !antigrav_enabled;
	}
	if(input.keys_released.count("up") != 0 && acceleration_gear != getMaxGear()){
		acceleration_gear++;
	}
	if(input.keys_released.count("down") != 0 && acceleration_gear != 0){
		acceleration_gear--;
	}
}

void Ship::update(double dt, Terrain &terrain){

	double acceleration_per_gear = 10000;  //constant throughout game, should define this as such later

	//run animations
    for(map<string,string>::iterator i = current_animation_name.begin(); i != current_animation_name.end(); i++){

    	if(!animation[i->first][i->second].play(dt, sprite[i->first])){
    		//if animation has stopped playing
    		i->second = "default";
    	}
    }
    //

    double acceleration_magnitude = acceleration_gear*acceleration_per_gear;

    if(target_vector.x != 0 || target_vector.y != 0){

    	Vector2f target_difference = target_vector - acceleration_vector;
		if(hypot(target_difference.x,target_difference.y) <= 0.1){
			target_difference = Vector2f(0,0);
		}
		if(target_difference.x != 0 || target_difference.y != 0){
			target_difference = normalize(target_difference);
		}
		acceleration_vector = acceleration_vector + (target_difference*(1.0/getHandling())*dt);

		if(acceleration_vector.x != 0 || acceleration_vector.y != 0){
			double target = getRotationFromAxis(acceleration_vector) - 90;
			if(target < 0){ target += 360; }
			if(target > 360){ target -= 360; }
			sprite[type_name].setRotation(target);
		}
    }
	
    //update position and its derivatives
    Vector2f acceleration = acceleration_vector*acceleration_magnitude;
    double required_fuel = acceleration_magnitude*getFuelConsumption()*dt;
    if(required_fuel <= fuel){ //fuel is measured in kilolitres
    	fuel -= required_fuel;

    	if(abs(velocity.x + (acceleration.x*dt)) <= getMaxSpeed()){
    		velocity.x = velocity.x + (acceleration.x*dt);
    	}
    	if(velocity.y > 0 || abs(velocity.y + (acceleration.y*dt)) < getMaxSpeed()){
    		velocity.y = velocity.y + (acceleration.y*dt);
    	}
    }

    double gravity_magnitude = 3000; //constant throughout game, should define this as such later
    double antigrav_required_fuel = gravity_magnitude*getFuelConsumption()*dt;
    if(antigrav_enabled && antigrav_required_fuel <= fuel){
    	

    	//hover above ground when antigrav is on
    	FloatRect hovering_bounds = bounds;
    	hovering_bounds.top = position.y - hovering_bounds.height;
       	hovering_bounds.width *= 1.25;
       	hovering_bounds.left = position.x - (hovering_bounds.width/2.0);
    	double current_height = distanceFromGround(hovering_bounds, terrain);
    	double hover_height = 900;

    	if(current_height < hover_height){
    		fuel -= antigrav_required_fuel;
    	velocity = velocity - Vector2f(0,gravity_magnitude*dt);
    		Vector2f hover_velocity;
    		//if(abs(velocity.x) < terrain.tile_size){
    			hover_velocity = Vector2f(0,4*(current_height-hover_height));
    		//}
    		//else{
    		//	hover_velocity = Vector2f(0, 1/(2*terrain.tile_size*(1/abs(velocity.x)))*(current_height-hover_height)); //the velocity the ship would need to correct itself up to hovering height within 1/8 a second
    		//} 
    		position = position + (hover_velocity*dt);

    		if(velocity.y > 0){
	    		Vector2f hover_velocity = Vector2f(0, 0); //the velocity the ship would need to correct itself up to hovering height within 1/8 a second
	    		Vector2f needed_acceleration = Vector2f(0,-velocity.y)*8; //the acceleration the ship needs to achieve that velocity within 1/8 a second
	    		velocity = velocity + (needed_acceleration*dt); //achieve that velocity 
	    	}
    	}
        
    	//
    }
    velocity = velocity + Vector2f(0,gravity_magnitude*dt);
    

    position = position + (velocity*dt);
    
    //update bounds position
    bounds.top = position.y - bounds.height;
    bounds.left = position.x - (bounds.width/2.0);
    //
    
    //update sprites relative to the (possibly) new position
    updateSpritePositions();
    //
}

void Ship::updateSpritePositions(){

	sprite[type_name].setPosition(position + ship_properties[type_name].relative_position);
    for(vector<string>::iterator i = upgrade_names.begin(); i != upgrade_names.end(); i++){
    	sprite[*i].setPosition(position + ship_properties[type_name].relative_position);
    }
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