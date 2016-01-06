#include "main.h"

using namespace std;
using namespace sf;


extern World world;

Item::Item(){}

Item::Item(string new_sector_id, string item_id, string type){

    properties = getProperties(type);
    sector_id = new_sector_id;
    id = item_id;
}

bool Item::putInStorage(string new_storage_id){

    string old_storage_id = storage_id;

    if(shared_ptr<Entity> old_storage = getSector(sector_id)->getEnt(old_storage_id)){
        old_storage->removeFromContents(id);
    }

    if(shared_ptr<Entity> new_storage = getSector(sector_id)->getEnt(new_storage_id)){
        if(new_storage->addToContents(id)){
            return true;
        }
    }
    //else
    if(shared_ptr<Entity> old_storage = getSector(sector_id)->getEnt(old_storage_id)){
        old_storage->addToContents(id);
    }
    return false;

}

//bool Item::putInWorld(Vector2f new_coords){
//
//    string old_storage_id = storage_id;
//
//    if(shared_ptr<Entity> old_storage = getSector(sector_id)->getEnt(old_storage_id)){
//        old_storage->removeFromContents(id);
//    }
//
//    storage_id = makeNewScrap(sector_id,createUniqueId(),new_coords.x,new_coords.y);
//    if(getSector(sector_id)->scrap[storage_id].lock()->setItem(id)){
//
//        return true;
//    }
//    else{
//
//        if(shared_ptr<Entity> old_storage = getSector(sector_id)->getEnt(old_storage_id)){
//            old_storage->addToContents(id);
//        }
//        return false;
//    }
//}

//void makeNewItemInWorld(string sector_id, string type, string item_id, Vector2f new_coords){
//
//    if(getProperties(type)->classification=="ENTITY"){
//        makeNewEntity(sector_id, createUniqueId(), new_coords.x, new_coords.y, 0, type);
//    }
//    else if(getProperties(type)->classification=="MACHINE"){
//        makeNewMachine(sector_id, createUniqueId(), new_coords.x, new_coords.y, 0, type);
//    }
//    else{
//        registerNewItem(sector_id, item_id, type);
//        shared_ptr<Item> item = getSector(sector_id)->items[item_id];
//
//        item->putInWorld(new_coords);
//    }
//
//}

void makeNewItemInStorage(string sector_id, string item_id, string type, string storage_id){

    registerNewItem(sector_id, item_id, type);
    shared_ptr<Item> item = getSector(sector_id)->items[item_id];

    item->putInStorage(storage_id);
}


