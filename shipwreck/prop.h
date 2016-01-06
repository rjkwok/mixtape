#ifndef WEC_PROP
#define WEC_PROP

#include "main.h"

using namespace std;
using namespace sf;

class Prop: public Entity{

public:

    Prop();
    Prop(string new_sector_id, string e_id, double x, double y, string type);

    virtual void run(double dt);
};

class FloorProp: public Floor{

public:

    FloorProp();
    FloorProp(string new_sector_id, string e_id, double x, double y, double x_tiles, double y_tiles, string type);

    virtual void run(double dt);
};

string makeNewProp(string sector_id, string ent_id, double x, double y, string type);
string makeNewProp(string sector_id, string ent_id, double x, double y, double rotation, string type);
string makeNewFloorProp(string sector_id, string ent_id, double x, double y, double x_tiles, double y_tiles, string type);
string makeNewFloorProp(string sector_id, string ent_id, double x, double y, double x_tiles, double y_tiles, double rotation, string type);

#endif
