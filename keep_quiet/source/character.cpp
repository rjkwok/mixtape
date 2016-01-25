#include "main.h"

using namespace std;
using namespace sf;

extern map<string, Texture*> textures;
extern map<string, CharacterProperties> character_properties;


CharacterProperties::CharacterProperties(){

	type_name = "";
	texture_id = "";
}

Character::Character(){}

Character::Character(string c_name, string c_type_name, Vector2f position){

	name = c_name;
	type_name = c_type_name;

	sprite = createSprite(character_properties[type_name].texture_id, position, "middle");
	for(map<string, int>::iterator i = character_properties[type_name].start_index.begin(); i != character_properties[type_name].start_index.end(); i++){

    	animation[i->first] = Animation(i->second, character_properties[type_name].end_index[i->first], character_properties[type_name].is_looping[i->first]);
    }
    current_animation_name = "default";
    sprite.setTextureRect(getFrame(character_properties[type_name].start_index["default"], sprite));

    structure_id = "";
	ship_id = "";

	dy = 0;
	ddy = 0;
}

void Character::moveWithShip(map<string, Ship*> &ships){

	if(ship_id != ""){

		sprite.setPosition(ships[ship_id]->position);
	}
}

void Character::update(double dt, Terrain &terrain){

	//run animations
	if(!animation[current_animation_name].play(dt, sprite)){
		//if animation has stopped playing
		current_animation_name = "default";
	}
    //

	if(ship_id == ""){ //if not piloting a ship

        if(distanceFromGround(sprite.getGlobalBounds(), terrain) > 0){ //if the character is in the air, enact a gravitational acceleration
            ddy = 3000; 
        }
        else{ //if the player is on the ground, downwards acceleration and velocity should be set to 0
            ddy = 0;
            if(dy > 0){
                dy = 0;
            }
        }
        dy += (ddy*dt); //apply y acceleration
        sprite.move(0, dy*dt); //apply y velocity
	}
    
    //if the sprite has crossed either edge of the map, teleport it to the other side accordingly
	if(ceil(sprite.getPosition().x) > (terrain.max_x*terrain.tile_size)){ sprite.setPosition(0,sprite.getPosition().y); }
	if(floor(sprite.getPosition().x) < 0){ sprite.setPosition((terrain.max_x*terrain.tile_size),sprite.getPosition().y); }
	//
}

void Character::draw(RenderWindow &window){

	if(ship_id == ""){
		window.draw(sprite);
	}
}