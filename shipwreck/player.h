#ifndef WEC_PLAYER
#define WEC_PLAYER

#include "main.h"

using namespace std;
using namespace sf;

enum Direction{Forward,Backward,Left,Right,Clockwise,Counterclockwise};



class Player: public Entity
{
    public:


    Player();
    Player(string new_sector_id, string name, double x, double y, string type);

    virtual void run(double);

    void setDirection(Direction);
    void say(string);

    void walk(Direction,double);
    void turn(Direction,double);

    void grab(string ent_id);

    bool jetpack_on = false;
    bool flashlight_on = false;

    double range = 0;

};

string makeNewPlayer(string sector_id, string ent_id, double x, double y, double rotation, string type);

#endif // WEC_PLAYER
