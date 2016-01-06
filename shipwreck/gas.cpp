#include "main.h"

using namespace std;
using namespace sf;


extern World world;
extern RenderWindow window;

//Cloud::Cloud(){}
//
//Cloud::Cloud(string new_sector_id, string new_id, double new_kilomoles, Vector2f new_coords, Vector2f new_velocity, string type){
//
//    sector_id = new_sector_id;
//
//    radius = 2;
//
//    if(new_kilomoles==0){kilomoles=1;}
//    id = new_id;
//    coords = new_coords;
//    properties = getProperties(type);
//    kilomoles = new_kilomoles;
//    p = new_velocity*(kilomoles*properties->get("molar_mass"));
//
//    IntRect bounds_rect(0,0,properties->get("width"),properties->get("height"));
//    sprite.setTexture(properties->sheet,false);
//    sprite.setTextureRect(bounds_rect);
//    sprite.setOrigin(bounds_rect.width/2,bounds_rect.height/2);
//    sprite.setPosition(coords.x,coords.y);
//    animator.play(0,4);
//
//    int neg = 1;
//    if(rand() % 100<50){neg=-1;}
//    spin = neg * (rand() % 40);
//}
//
//
//void Cloud::run(double dt){
//
//    animator.update(dt, sprite);
//
//    if(kilomoles>=1)
//    {
//        double neg_x = 1;
//        double neg_y = 1;
//        if(rand() % 100<50){neg_x=-1;}
//        if(rand() % 100<50){neg_y=-1;}
//
//        double x_excitement = neg_x*(rand()%25)*(kilomoles*properties->get("molar_mass"))*dt;
//        double y_excitement = neg_y*(rand()%25)*(kilomoles*properties->get("molar_mass"))*dt;
//
//        p = p + Vector2f(x_excitement,y_excitement);
//    }
//
//
//
//    Vector2f velocity = (p/(kilomoles*properties->get("molar_mass")))*dt;
//    sprite.rotate(spin*dt);
//    coords = coords + velocity;
//
//
//    sprite.setColor(Color(properties->get("r"),properties->get("g"),properties->get("b"),properties->get("a")));
//
//    sprite.setPosition(coords.x,coords.y);
//
//    if(kilomoles>=50){split();}
//}
//
//void Cloud::draw()
//{
//    window.draw(sprite);
//}
//
//void Cloud::split(){
//
//    string new_id1 = createUniqueId();
//    string new_id2 = createUniqueId();
//
//    string flow_id = properties->name;
//    Vector2f starting_pos = coords;
//    Vector2f starting_vel = (p/(kilomoles*properties->get("molar_mass")));
//
//    makeNewCloud(sector_id,new_id1,kilomoles/2,starting_pos,starting_vel,flow_id);
//    makeNewCloud(sector_id,new_id2,kilomoles/2,starting_pos,starting_vel,flow_id);
//
//    getSector(sector_id)->trash(id);
//}
//
//void Cloud::addKilomoles(double k){
//
//    kilomoles += k;
//}
//
//Vector2f Cloud::getMomentum(){
//
//    return p;
//}
//
//void Cloud::applyImpulse(double x, double y){
//
//    p = p + Vector2f(x,y);
//}
//
//bool Cloud::inRange(Vector2f point, double range){
//
//    double h = hypot(coords.x-point.x,coords.y-point.y);
//    if(h <= range+radius){return true;}
//
//    return false;
//}

void applyHeat(string sector_id, double quantity, Vector2f point, double radius){

    set<string> nearby_ents = getSector(sector_id)->collision_grid.getNearby(point, radius);
    for(set<string>::iterator parser = nearby_ents.begin(); parser != nearby_ents.end(); parser++){

        shared_ptr<Entity> ob = getSector(sector_id)->getEnt(*parser);
        Vector2f disp = point - ob->getPosition();
        if( (disp.x*disp.x) + (disp.y*disp.y) < (radius + ob->collision_radius)*(radius + ob->collision_radius)){
            ob->temperature += quantity;
        }
    }

}

//string makeNewCloud(string new_sector_id, string new_id, double new_kilomoles, Vector2f new_coords, Vector2f new_velocity, string type){
//
//    world.sectors[new_sector_id]->clouds[new_id] = make_shared<Cloud>(new_sector_id, new_id,new_kilomoles,new_coords,new_velocity,type);
//    return new_id;
//}
