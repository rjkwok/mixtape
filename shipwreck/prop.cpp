#include "main.h"

using namespace std;
using namespace sf;

extern World world;
extern RenderWindow window;

Prop::Prop(){}
Prop::Prop(string new_sector_id, string e_id, double x, double y, string type){

    constructEntity(new_sector_id,e_id,x,y,type);
    prop = true;
    bound_to_sector = true;
}

void Prop::run(double dt){

    animator.update(dt, sprite);

    p = Vector2f(0,0);
    rot_p = 0;

    temperature -= 1.5*dt;
    if(temperature < 0){ temperature = 0; }

    double r_percent = 1;
    double g_percent = 1;
    double b_percent = 1;

    if(temperature > 273.15){

        if(temperature-273.15 > 200){
            if(properties->classification != "PLAYER"){
                getSector(sector_id)->trash(id);
                createEffect(sector_id, getPosition(), getVelocity(), "Smoke");
            }

        }
        if((temperature-273.15)>125){
            double percent = (temperature-398.15)/75;
            if(percent > 1){percent = 1;}
            castLight(sector_id, getPosition(), 0.8, (155*percent) + (sin(world.date.second)*(8*percent)));
        }
        if((temperature-273.15)>125 && abs(1-sin(world.date.second*2))<0.01){

             double neg_x = 1;
            double neg_y = 1;
            if(rand() % 100<50){neg_x=-1;}
            if(rand() % 100<50){neg_y=-1;}
            Vector2f excitement(neg_x*(rand()%50),neg_y*(rand()%50));

            createEffect(sector_id, getPosition(), excitement, "Embers");
        }

        double percent = (temperature-273.15)/400;
        if(percent > 1){percent = 1;}

        g_percent -= percent*0.75;
        b_percent -= percent;
    }


    sprite.setColor(Color(r_percent*(getSector(sector_id)->ambient_light+light_level),g_percent*(getSector(sector_id)->ambient_light+light_level),b_percent*(getSector(sector_id)->ambient_light+light_level)));
    light_level = 0;

    if(light_override!=Color(0,0,0))
    {
        sprite.setColor(light_override);
        light_override = Color(0,0,0);
    }

    if(properties->name == "Huge Asteroid 1" || properties->name == "Big Asteroid 1" || properties->name == "Medium Asteroid 1"){
        sprite.setColor(Color(155,155,155,255));
    }

    sprite.setPosition(coords.x,coords.y);

}

FloorProp::FloorProp(){}
FloorProp::FloorProp(string new_sector_id, string e_id, double x, double y, double x_tiles, double y_tiles, string type){

    constructEntity(new_sector_id,e_id,x,y,type);
    prop = true;
    bound_to_sector = true;

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

void FloorProp::run(double dt){

    animator.update(dt, sprite);

    p = Vector2f(0,0);
    rot_p = 0;

    applyGrip(dt);
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

}
string makeNewProp(string sector_id, string ent_id, double x, double y, string type){

    getSector(sector_id)->ents[ent_id] = shared_ptr<Entity>(make_shared<Prop>(sector_id,ent_id,x,y,type));
    return ent_id;
}

string makeNewProp(string sector_id, string ent_id, double x, double y, double rotation, string type){

    getSector(sector_id)->ents[ent_id] = shared_ptr<Entity>(make_shared<Prop>(sector_id,ent_id,x,y,type));
    getSector(sector_id)->ents[ent_id]->setRotation(rotation);
    return ent_id;
}

string makeNewFloorProp(string sector_id, string ent_id, double x, double y, double x_tiles, double y_tiles, string type){

    getSector(sector_id)->floors[ent_id] = shared_ptr<Floor>(make_shared<FloorProp>(sector_id,ent_id,x,y,x_tiles,y_tiles,type));
    return ent_id;
}

string makeNewFloorProp(string sector_id, string ent_id, double x, double y, double x_tiles, double y_tiles, double rotation, string type){

    getSector(sector_id)->floors[ent_id] = shared_ptr<Floor>(make_shared<FloorProp>(sector_id,ent_id,x,y,x_tiles,y_tiles,type));
    getSector(sector_id)->floors[ent_id]->setRotation(rotation);
    return ent_id;
}
