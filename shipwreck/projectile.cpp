#include "main.h"

using namespace std;
using namespace sf;

extern World world;
extern RenderWindow window;

//Projectile::Projectile(){}
//
//void Projectile::run(double dt){
//
//    coords = coords + ((p/properties->get("mass"))*dt);
//    sprite.setPosition(coords);
//
//    set<string> possible_collisions = getSector(sector_id)->collision_grid.getNearby(coords, 0);
//    for(set<string>::iterator parser = possible_collisions.begin(); parser != possible_collisions.end(); parser++){
//        if(pointInSprite(getSector(sector_id)->getEnt(*parser)->sprite, coords)){
//            hit(*parser);
//            getSector(sector_id)->trash(id);
//            return;
//        }
//    }
//}
//
//void Projectile::hit(string ob_id){}
//
//void Projectile::draw(){
//
//    window.draw(sprite);
//}
//
//Rivet::Rivet(string new_sector_id, string new_id, Vector2f new_coords, Vector2f velocity, string type){
//
//    sector_id = new_sector_id;
//    id = new_id;
//    properties = getProperties(type);
//    coords = new_coords;
//    p = velocity*properties->get("mass");
//
//    sprite.setTexture(properties->sheet, true);
//    IntRect bounds = sprite.getTextureRect();
//    sprite.setOrigin(bounds.width/2, bounds.height/2);
//    sprite.setRotation(getRotationFromAxis(velocity));
//}
//
//void Rivet::hit(string ob_id){
//
//    shared_ptr<Entity> ob = getSector(sector_id)->getEnt(ob_id);
//    ob->applyImpulse(p.x, p.y, true);
//
//    for(set<string>::iterator parser = ob->floors.begin(); parser != ob->floors.end(); parser++){
//        makeNewConnector(sector_id, createUniqueId(), "RIVET", ob->id, *parser);
//    }
//}
//
//Epoxy::Epoxy(string new_sector_id, string new_id, Vector2f new_coords, Vector2f velocity, string type){
//
//    sector_id = new_sector_id;
//    id = new_id;
//    properties = getProperties(type);
//    coords = new_coords;
//    p = velocity*properties->get("mass");
//
//    sprite.setTexture(properties->sheet, true);
//    IntRect bounds = sprite.getTextureRect();
//    sprite.setOrigin(bounds.width/2, bounds.height/2);
//    sprite.setRotation(getRotationFromAxis(velocity));
//}
//
//void Epoxy::hit(string ob_id){
//
//}
//
//string makeNewProjectile(string sector_id, string id, Vector2f coords, Vector2f velocity, string type){
//
//    if(type == "Rivet"){
//
//        shared_ptr<Rivet> new_projectile = make_shared<Rivet>(sector_id, id, coords, velocity, type);
//        getSector(sector_id)->projectiles[id] = shared_ptr<Projectile>(new_projectile);
//    }
//    if(type == "Epoxy"){
//
//        shared_ptr<Epoxy> new_projectile = make_shared<Epoxy>(sector_id, id, coords, velocity, type);
//        getSector(sector_id)->projectiles[id] = shared_ptr<Projectile>(new_projectile);
//    }
//
//    return id;
//}
