#ifndef WEC_BOND
#define WEC_BOND

#include "main.h"

using namespace std;
using namespace sf;

struct Connector
{
    Connector();
    Connector(string, string, string, string a, string b);

    void cleanup();

    string sector_id;
    string id;
    string type;
    string classification;

    bool queued_for_deletion = false;

    string a_id;
    string b_id;

    double integrity = 45;
    bool has_sprite = false;
    Sprite sprite;
    double collision_radius;
    double light_level = 0;
    Vector2f relative_position;
    double relative_rotation;
    void setSprite(Sprite new_sprite);
    void light(double amount);
    void updateSprite();
    void draw();
};

string makeNewConnector(string sector_id, string c_id, string c_type, string a_id, string b_id);
void makeNewBond(string sector_id, set<string> new_members);
void makeNewBond(string sector_id, string b_id, set<string> new_members);
//void showConnectors(vector<weak_ptr<Entity> >,ConnectorType,Color);

struct Bond
{
    Bond(string);
    Bond(string,string);

    set<string> members;
    vector<set<string> > subgroups;
    void createSubgroups();
    set<string> getGroupWith(string ent_id);
    void displaceGroupContaining(string ent_id, Vector2f disp);
    map<string,Vector2f> positions;
    map<string,double> orientations;

    string sector_id;

    string id = "";
    Vector2f center = Vector2f(0,0);
    Vector2f p = Vector2f(0,0);
    double bond_radius = 0;
    double rotation = 0;
    double rot_p = 0;
    double mass = 1;

    bool floor_attached = false;

    bool moveable = true;

    Vector2f getUniverseCoords();

    void addMember(string);
    void removeMember(string);
    void applyBond(double dt);
    bool inBond(string e_id);

    double getInertia();

    void applyImpulse(double x, double y, Vector2f origin, bool apply_spin = true);
    void applyRotationalImpulseOnly(double x, double y, Vector2f origin);
    void setVelocity(double,double);
    Vector2f getVelocity();
    void setRotationalVelocity(double);
    void setRotation(double);

    void resetProperties();

//    void showAllConnectors();
//    void showAllConnectors(Color);

    void cleanup();

    set<string> collided_bonds;
    Vector2f backup_p = Vector2f(0,0);

};

#endif // WEC_BOND
