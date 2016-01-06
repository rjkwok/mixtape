#include "main.h"

using namespace std;
using namespace sf;


extern World world;
extern RenderWindow window;

Floor::Floor(){}

Floor::Floor(string new_sector_id, string f_id, double x , double y, double x_tiles, double y_tiles, string type)
{
    constructEntity(new_sector_id,f_id,x,y,type);

    tiles_x = x_tiles;
    tiles_y = y_tiles;

    double tile_width = properties->get("width");
    double tile_height = properties->get("height");

    double width = tile_width*x_tiles;
    double height = tile_height*y_tiles;

    Sprite tile = sprite;
    sprite.setTextureRect(IntRect(0,0,width,height));
    sprite.setOrigin(width/2,height/2);
    sprite.setPosition(x,y);

    Vector2f start_position((x - (width/2)) + (tile_width/2), (y - (height/2)) + (tile_height/2));

    for(int y_index = 0; y_index < y_tiles; y_index++){
        for(int x_index = 0; x_index < x_tiles; x_index++){
            tile.setPosition(start_position.x + (tile_width*x_index), start_position.y + (tile_height*y_index));
            tile_positions.push_back(tile.getPosition() - sprite.getPosition());
            tile_light_levels.push_back(0);
            tiles.push_back(tile);
        }
    }

    collision_radius = hypot(width/2, height/2);
    tile_radius = hypot(tile_width/2,tile_height/2);
    k=10;
}

double Floor::getMyMass(){

    return properties->get("mass")*tiles.size();
}

void Floor::flagBonded(){
    if(bonded){
        for(set<string>::iterator parser = getSector(sector_id)->bonds[bond]->members.begin(); parser != getSector(sector_id)->bonds[bond]->members.end(); parser++){
            getSector(sector_id)->getEnt(*parser)->bonded_to_floor = true;
            if(getSector(sector_id)->getEnt(*parser)->intersects(id)){
                getSector(sector_id)->getEnt(*parser)->floor_bonds.insert(bond);
            }

        }
    }
}

void Floor::displace(double x, double y){

    if(bonded)
    {
        shared_ptr<Bond> my_bond = getSector(sector_id)->bonds[bond];
        my_bond->center = my_bond->center + Vector2f(x,y);
        double Q = my_bond->rotation*(M_PI/180); //rotation in radians

        for(set<string>::iterator i = my_bond->members.begin(); i != my_bond->members.end(); i++)
        {
            if(shared_ptr<Entity> e = getSector(sector_id)->getEnt(*i)){

                Vector2f newPosition = my_bond->center + Vector2f(my_bond->positions[e->id].x*cos(Q)-my_bond->positions[e->id].y*sin(Q),my_bond->positions[e->id].x*sin(Q)+my_bond->positions[e->id].y*cos(Q));
                e->setPosition(newPosition.x,newPosition.y);
                if(getSector(sector_id)->floors.count(*i)!=0){
                    for(set<string>::iterator parser = getSector(sector_id)->floors[*i]->passengers.begin(); parser != getSector(sector_id)->floors[*i]->passengers.end(); parser++){
                        if(my_bond->members.count(*parser)==0){
                            getSector(sector_id)->getEnt(*parser)->displace(x,y);
                        }
                    }
                }
            }
        }

        return;
    }
    else{
        coords = coords + Vector2f(x,y);
        sprite.setPosition(coords.x,coords.y);
        for(set<string>::iterator parser = passengers.begin(); parser != passengers.end(); parser++){
            getSector(sector_id)->getEnt(*parser)->displace(x,y);
        }
    }

}

void Floor::run(double dt)
{
    runPhysicsAndGraphics(dt);
    

    double Q = sprite.getRotation() * (M_PI/180);

    vector<Vector2f>::iterator tile_position = tile_positions.begin();
    vector<double>::iterator tile_light = tile_light_levels.begin();
    for(vector<Sprite>::iterator parser = tiles.begin(); parser != tiles.end(); parser++){

        Vector2f new_position = sprite.getPosition() + Vector2f(tile_position->x*cos(Q)-tile_position->y*sin(Q),tile_position->x*sin(Q)+tile_position->y*cos(Q));
        parser->setPosition(new_position);
        parser->setRotation(sprite.getRotation());

        parser->setColor(Color(getSector(sector_id)->ambient_light + *tile_light,getSector(sector_id)->ambient_light + *tile_light,getSector(sector_id)->ambient_light + *tile_light,255));
        *tile_light = 0;

        tile_position++;
        tile_light++;
    }

    //applyGrip(dt);
}

void Floor::draw(){

    double Q = sprite.getRotation() * (M_PI/180);

    vector<Vector2f>::iterator tile_position = tile_positions.begin();
    for(vector<Sprite>::iterator parser = tiles.begin(); parser != tiles.end(); parser++){

        Vector2f new_position = sprite.getPosition() + Vector2f(tile_position->x*cos(Q)-tile_position->y*sin(Q),tile_position->x*sin(Q)+tile_position->y*cos(Q));
        parser->setPosition(new_position);
        parser->setRotation(sprite.getRotation());

        tile_position++;
        window.draw(*parser);
    }
}

void Floor::determinePassengers()
{
    passengers.clear();

    set<string> nearby_ids = getSector(sector_id)->collision_grid.getNearby(getSector(sector_id)->getEnt(id));

    for(set<string>::iterator i = nearby_ids.begin(); i != nearby_ids.end(); i++)
    {
        shared_ptr<Entity> possible_passenger = getSector(sector_id)->getEnt(*i);

        if(not possible_passenger){continue;}

        if(possible_passenger->properties->classification=="ITEM"){continue;}
        if(possible_passenger->bound_to_sector){continue;}
        if(possible_passenger->slaved){continue;}

        if(possible_passenger->bonded and bonded and possible_passenger->bond == bond){continue;}
        if(getSector(sector_id)->people.count(*i)!=0 && getSector(sector_id)->people[*i].lock()->jetpack_on){continue;}

        if(possible_passenger->intersects(id))
        {
            passengers.insert(*i);
            possible_passenger->floors.insert(id);
            if(bonded){possible_passenger->floor_bonds.insert(bond);}
        }
    }
}

void Floor::applyGrip(double dt)
{

    for(set<string>::iterator parser = passengers.begin(); parser!=passengers.end(); parser++)
    {
        shared_ptr<Entity> passenger = getSector(sector_id)->getEnt(*parser);
        if(not passenger){
            continue;
        }
        if(passenger->bonded_to_floor){continue;}

        Vector2f relative_point = Vector2f(0,0);
        if(bonded)
        {
            shared_ptr<Bond> my_bond = getSector(sector_id)->bonds[bond];
            relative_point = passenger->getPosition()-my_bond->center;
        }
        else if(passenger->getPosition() != coords)
        {
            relative_point = passenger->getPosition()-coords;
        }
        double radius = hypot(relative_point.x,relative_point.y);


        Vector2f floor_v = getVelocity() + (getPerpendicularAxis(relative_point)*radius*(getRotationalVelocity()*(M_PI/180.0)));
        Vector2f pass_v = passenger->getVelocity();

        Vector2f relative_velocity = (floor_v - pass_v);
        
        double k = 8;
        passenger->applyImpulse(relative_velocity.x*passenger->getMyMass()*k*dt/passenger->floors.size(),relative_velocity.y*passenger->getMyMass()*k*dt/passenger->floors.size(),passenger->getPosition(),false,false);
        if(!passenger->ghost){
            applyImpulse(relative_velocity.x*passenger->getMyMass()*-k*dt/passenger->floors.size(),relative_velocity.y*passenger->getMyMass()*-k*dt/passenger->floors.size(),passenger->getPosition(),false,false);
        }
        
        double relative_spin = ((getRotationalVelocity())-(passenger->getRotationalVelocity()));
        passenger->applyRotationalImpulse(relative_spin*passenger->getMyInertia()*k*dt);

        if(abs(relative_spin) < 1){
            passenger->setRotationalVelocity(getRotationalVelocity());
        }
    }

}

string makeNewFloor(string sector_id, string ent_id, double x, double y, double x_tiles, double y_tiles, double rotation, string type){

    getSector(sector_id)->floors[ent_id] = make_shared<Floor>(sector_id,ent_id,x,y,x_tiles,y_tiles,type);
    getSector(sector_id)->floors[ent_id]->setRotation(rotation);
    return ent_id;
}
