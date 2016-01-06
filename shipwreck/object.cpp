#include "main.h"

using namespace std;
using namespace sf;

Object::Object(){}
Object::Object(string new_id, Vector2f new_coords, double new_rotation, string type){

    id = new_id;
    properties = getProperties(type);

    sprite.setTexture(properties->sheet, false);
    IntRect bounds_rect = IntRect(0,0,properties->get("width"),properties->get("height"));
    sprite.setTextureRect(bounds_rect);
    sprite.setOrigin(bounds_rect.width/2,bounds_rect.height/2);

    sprite.setPosition(new_coords);
    sprite.setRotation(new_rotation);
}
