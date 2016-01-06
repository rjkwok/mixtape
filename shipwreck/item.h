#ifndef WEC_ITEM
#define WEC_ITEM

#include "main.h"

using namespace std;
using namespace sf;

class Item{

public:

    Item();
    Item(string new_sector_id, string item_id, string type);

    string id; //internal identification for processor
    shared_ptr<Properties> properties; //name, parts that make it up, visual, about

    bool putInStorage(string new_storage_id);
   // bool putInWorld(Vector2f new_coords);

    string sector_id;
    string storage_id = "";
};

//void makeNewItemInWorld(string sector_id, string type, string item_id, Vector2f new_coords);
void makeNewItemInStorage(string sector_id, string item_id, string type, string storage_id);


#endif // WEC_ITEM
