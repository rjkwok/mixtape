#ifndef WEC_FIELD
#define WEC_FIELD

#include "main.h"

using namespace std;
using namespace sf;

class Field{

public:

    Field();

    string id;
    string sector_id;
    string type;

    int bounds_type = 0; //0=rectangle, 1=circle
    RectangleShape bounds_r;
    CircleShape bounds_c;

    string linked_sector_id;
    Vector2f linked_sector_spawn;

    vector<Vector2f> grid_cells;

    virtual void activateFor(string ent_id, double dt = 0);
    void draw();
};

class GateField: public Field{

public:

    GateField(string c_sector_id, string c_id, string l_sector_id, Vector2f l_sector_spawn, Vector2f top_left, Vector2f bottom_right, Color fill_color = Color(0,0,0,0)); //rectangle constructor
    GateField(string c_sector_id, string c_id, string l_sector_id, Vector2f l_sector_spawn, Vector2f centre, double radius, Color fill_color = Color(0,0,0,0)); //circle constructor



    virtual void activateFor(string ent_id, double dt = 0);
};

//a field is defined by either a circle or a rectangle as bounds (in one class)
//different subclasses of field produce different overrides of the activateFor(string ent_id) method
//fields are stored in a unique map in the sector class
//fields can be loaded/saved to file
//collision grid class handles checks for field collision, calls activateFor when collisions detected
//a collision with a field is defined as an overlap with the shape bounds
//collisions with a field do not stop movement
//field shape and colour are taken as arguments to the constructor in all subclasses

#endif //WEC_FIELD
