#include "main.h"

using namespace std;
using namespace sf;

extern World world;
extern RenderWindow window;

Field::Field(){}
void Field::activateFor(string ent_id, double dt){}
void Field::draw(){

    if(bounds_type == 0){
        window.draw(bounds_r);
    }
    else if(bounds_type == 1){
        window.draw(bounds_c);
    }
}

GateField::GateField(string c_sector_id, string c_id, string l_sector_id, Vector2f l_sector_spawn, Vector2f top_left, Vector2f bottom_right, Color fill_color){

    id = c_id;
    sector_id = c_sector_id;
    type = "gate";

    bounds_type = 0;
    linked_sector_id = l_sector_id;
    linked_sector_spawn = l_sector_spawn;

    bounds_r.setSize(bottom_right-top_left);
    bounds_r.setOrigin((bottom_right.x-top_left.x)/2.0,(bottom_right.x-top_left.x)/2.0);
    bounds_r.setPosition(top_left + ((bottom_right.x-top_left.x)/2.0,(bottom_right.x-top_left.x)/2.0));
    bounds_r.setOutlineThickness(0);
    bounds_r.setFillColor(fill_color);
}
GateField::GateField(string c_sector_id, string c_id, string l_sector_id, Vector2f l_sector_spawn, Vector2f centre, double radius, Color fill_color){

    id = c_id;
    sector_id = c_sector_id;
    type = "gate";

    bounds_type = 1;
    linked_sector_id = l_sector_id;
    linked_sector_spawn = l_sector_spawn;

    bounds_c.setRadius(radius);
    bounds_c.setOrigin(radius,radius);
    bounds_c.setPosition(centre);
    bounds_c.setOutlineThickness(0);
    bounds_c.setFillColor(fill_color);
}
void GateField::activateFor(string ent_id, double dt){

    world.transfer(sector_id, ent_id, linked_sector_id, linked_sector_spawn);
}
