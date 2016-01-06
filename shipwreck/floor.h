#ifndef WEC_FLOOR
#define WEC_FLOOR

#include "main.h"

using namespace std;
using namespace sf;

class Floor: public Entity
{

    public:

    Floor();
    Floor(string new_sector_id, string f_id, double x , double y, double x_tiles, double y_tiles, string type);

    virtual void run(double);
    virtual void draw();
    virtual void displace(double x, double y);

    void flagBonded();
    void determinePassengers();
    void applyGrip(double);

    virtual double getMyMass();

    set<string> passengers;

    double tiles_x;
    double tiles_y;

    vector<Sprite> tiles;
    vector<Vector2f> tile_positions;
    vector<double> tile_light_levels;
    double tile_radius = 1;

    double k; // coeffecient of friction

    Vector2f v_last = Vector2f(0,0);
    double rot_v_last = 0;

};

string makeNewFloor(string sector_id, string ent_id, double x, double y, double x_tiles, double y_tiles, double rotation, string type);

#endif // WEC_FLOOR
