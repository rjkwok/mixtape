#ifndef WEC_CONTAINER
#define WEC_CONTAINER

#include "main.h"

class Container: public Entity
{
    public:
    Container();
    Container(string new_sector_id, string e_id, double x, double y, string type);

   // weak_ptr<GUI_CONTAINER> gui_c;
    virtual void toggleGUI();
};

string makeNewContainer(string e_id, double x, double y, string info_key);
string makeNewContainer(string sector_key, string e_id, double x, double y, string info_key);


#endif // WEC_CONTAINER
