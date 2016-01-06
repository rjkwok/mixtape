#ifndef WEC_OBJ
#define WEC_OBJ

#include "main.h"

using namespace std;
using namespace sf;

class Object{

public:

    Object();
    Object(string new_id, Vector2f new_coords, double new_rotation, string type);

    string id;
    shared_ptr<Properties> properties;
    Sprite sprite;
};

#endif // WEC_OBJ
