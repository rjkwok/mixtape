#include "main.h"

using namespace std;
using namespace sf;


extern World world;
//
//Scrap::Scrap(string new_sector_id, string e_id, double x, double y){
//
//    constructEntity(new_sector_id, e_id, x, y, "Unknown");
//
//    setSlots(1);
//
//    double neg_x = 1;
//    double neg_y = 1;
//    if(rand() % 100<50){neg_x=-1;}
//    if(rand() % 100<50){neg_y=-1;}
//
//    double x_excitement = neg_x*(rand()%5)*getMass();
//    double y_excitement = neg_y*(rand()%5)*getMass();
//
//    p = p + Vector2f(x_excitement,y_excitement);
//
//    rot_p = (rand()%15)*neg_x;
//}
//
//bool Scrap::setItem(string item_id){
//
//    if(addToContents(item_id,1)){
//
//        properties = getProperties(getSector(sector_id)->items[contents[1]]->properties->name);
//        sprite.setTexture(properties->sheet, true);
//        FloatRect testRect = sprite.getLocalBounds();
//        sprite.setOrigin(testRect.width/2,testRect.height/2);
//        sprite.setPosition(getPosition().x,getPosition().y);
//        collision_radius = hypot(testRect.width/2,testRect.height/2);
//
//        return true;
//    }
//    else{
//
//        return false;
//    }
//}
//
//void Scrap::run(double dt){
//
//
//
//    runPhysicsAndGraphics(dt);
//}
//
//bool Scrap::salvage(string e_id){
//
//    //attempts to add an item to the entity specified with e_id, then deletes self
//    shared_ptr<Item> item = getSector(sector_id)->items[contents[1]];
//    if(item->putInStorage(e_id)){
//        getSector(sector_id)->trash(id);
//        return true;
//    }
//
//    return false;
//
//}
//
//string makeNewScrap(string sector_id, string new_id, double x, double y)
//{
//    shared_ptr<Scrap> temp = make_shared<Scrap>(sector_id,new_id,x,y);
//
//    getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(temp);
//    getSector(sector_id)->scrap[new_id] = weak_ptr<Scrap>(temp);
//
//    return new_id;
//}

