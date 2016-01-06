#include "main.h"

using namespace std;
using namespace sf;

extern RenderWindow window;
extern World world;

Poster::Poster(string new_sector_id, string new_id, string type){

    sector_id = new_sector_id;
    id = new_id;
    properties = getProperties(type);

    sprite.setTexture(getProperties("PHYSICAL POSTER")->sheet, true);
    IntRect sprite_bounds = sprite.getTextureRect();
    sprite.setOrigin(sprite_bounds.width/2, sprite_bounds.height/2);

    poster.setTexture(properties->sheet, true);
    IntRect poster_bounds = poster.getTextureRect();
    poster.setOrigin(poster_bounds.width/2, poster_bounds.height/2);
}

void Poster::attach(string new_parent_id, Vector2f new_mount_position){

    parent_id = new_parent_id;
    mount_position = new_mount_position;
}

void Poster::run(double dt){

    if(parent_id == ""){return;}

    shared_ptr<Entity> parent = getSector(sector_id)->getEnt(parent_id);

    double Q = parent->sprite.getRotation() * (M_PI/180);
    Vector2f position = parent->sprite.getPosition() + Vector2f(mount_position.x*cos(Q)-mount_position.y*sin(Q),mount_position.x*sin(Q)+mount_position.y*cos(Q));
    sprite.setPosition(position);

    FloatRect parent_bounds = parent->sprite.getLocalBounds();
    map<string, double> displacement;
    displacement["top"] = mount_position.y - (-parent_bounds.height/2);
    displacement["bottom"] = mount_position.y - (parent_bounds.height/2);
    displacement["right"] = mount_position.x - (parent_bounds.width/2);
    displacement["left"] = mount_position.x - (-parent_bounds.width/2);

    double closest_disp = -1;
    string closest_side = "";
    for(map<string, double>::iterator parser = displacement.begin(); parser != displacement.end(); parser++){
        if(closest_disp < 0 || abs(parser->second) < closest_disp){
            closest_disp = abs(parser->second);
            closest_side = parser->first;
        }
    }

    if(closest_side == "bottom"){
        sprite.setRotation(parent->sprite.getRotation());
    }
    if(closest_side == "top"){
        sprite.setRotation(parent->sprite.getRotation() + 180);
    }
    if(closest_side == "right"){
        sprite.setRotation(parent->sprite.getRotation() - 90);
    }
    if(closest_side == "left"){
        sprite.setRotation(parent->sprite.getRotation() + 90);
    }

    sprite.setColor(Color(getSector(sector_id)->ambient_light+light_level,getSector(sector_id)->ambient_light+light_level,getSector(sector_id)->ambient_light+light_level));
    light_level = 0;

    if(light_override!=Color(0,0,0))
    {
        sprite.setColor(light_override);
        light_override = Color(0,0,0);
    }
}


void Poster::light(double amount)
{
    light_level += amount;
    if(light_level > 200){light_level = 200;}
}

void Poster::overrideLight(int r,int g,int b)
{
    light_override = Color(r,g,b);
}

void Poster::draw(){

    if(parent_id == ""){return;}

    window.draw(sprite);
}

string makeNewPoster(string sector_id, string id, string type){

    getSector(sector_id)->posters[id] = make_shared<Poster>(sector_id, id, type);
    return id;
}

string makeNewPoster(string sector_id, string id, string type, string parent_id, Vector2f mount_position){

    getSector(sector_id)->posters[id] = make_shared<Poster>(sector_id, id, type);
    getSector(sector_id)->posters[id]->attach(parent_id, mount_position);
    return id;
}
