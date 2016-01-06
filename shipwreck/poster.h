#ifndef WEC_POSTER
#define WEC_POSTER

#include "main.h"

using namespace std;
using namespace sf;

class Poster{

public:

    Poster(string new_sector_id, string new_id, string type);

    string id;
    string sector_id;

    shared_ptr<Properties> properties;

    Sprite sprite;
    Vector2f mount_position;
    string parent_id;

    Sprite poster;

    void light(double amount);
    void overrideLight(int r,int g,int b);
    double light_level = 100;
    Color light_override = Color(0,0,0);

    void attach(string new_parent_id, Vector2f new_mount_position);
    void run(double dt);
    void draw();
};

string makeNewPoster(string sector_id, string id, string type);
string makeNewPoster(string sector_id, string id, string type, string parent_id, Vector2f mount_position);

#endif // WEC_POSTER
