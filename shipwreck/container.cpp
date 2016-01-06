#include "main.h"

using namespace std;
using namespace sf;


extern World world;

Container::Container(){}

Container::Container(string new_sector_id, string e_id, double x, double y, string type)
{
    constructEntity(new_sector_id, e_id, x, y,type);
    setSlots(properties->get("slots"));
//    shared_ptr<GUI_CONTAINER> temp_gui = make_shared<GUI_CONTAINER>("gui_"+id,id,"gui_container",Large);
  //  server.guis["gui_"+id] = shared_ptr<GUI>(temp_gui);
  //  gui_c = weak_ptr<GUI_CONTAINER>(temp_gui);
}

void Container::toggleGUI()
{
    //shared_ptr<GUI_CONTAINER> g = gui_c.lock();
    //g->enabled = not g->enabled;
}

string makeNewContainer(string sector_key, string e_id, double x, double y, string type)
{
    shared_ptr<Container> temp = make_shared<Container>(sector_key, e_id,x,y,type);
    getSector(sector_key)->ents[e_id] = shared_ptr<Entity>(temp);
    getSector(sector_key)->containers[e_id] = weak_ptr<Container>(temp);

    return e_id;
}
