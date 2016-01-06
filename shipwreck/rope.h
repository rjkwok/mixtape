#ifndef WEC_ROPE
#define WEC_ROPE

#include "main.h"

using namespace std;
using namespace sf;

struct RopeNode
{
    RopeNode();
    RopeNode(string new_sector_id, Vector2f new_coords, string new_rope_id, int new_index, string type);

    shared_ptr<Properties> properties;
    Sprite sprite;
    string rope_id = "";
    int index = 0;

    string sector_id;
    Vector2f coords;
    Vector2f p = Vector2f(0,0);
    double light_level = 0;

    string attached_id = "";
    Vector2f attachment_coords = Vector2f(0,0);

    void light(double amount);
    void run(double dt, bool apply_to_ob);
    void applyImpulse(double x, double y);

    Vector2f getPosition();
    Vector2f getVelocity();
    Vector2f getMomentum();
};


class Rope
{
    public:

    Rope();
    Rope(string new_sector_id,string name, string type);
    Rope(string new_sector_id, string name, Vector2f head, Vector2f tail, string type);

    void addExplicitNode(Vector2f position, Vector2f velocity, string attached, Vector2f attachment_coords);

    string id = "";
    shared_ptr<Properties> properties;
    vector<shared_ptr<RopeNode> > nodes;

    string sector_id;

    bool broken = false;

    void run(double dt);
    void draw();
    void attach(string ent_id, Vector2f relative);
    void attach(int index, string ent_id, Vector2f relative);

    int getNearestNode(Vector2f point);
};


#endif // WEC_ROPE

