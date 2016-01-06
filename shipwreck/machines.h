#ifndef WEC_MACHS
#define WEC_MACHS

#include "main.h"

using namespace std;using namespace sf;
using namespace sf;

class GasTank: public Machine{

public:

    GasTank(string new_sector_id, string e_id, double x, double y);
};
class GasValve: public Machine{

public:

    GasValve(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
};
class GasVent: public Machine{

public:

    GasVent(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
};

class PipeJoint: public Machine{

public:

    PipeJoint(string new_sector_id, string e_id, double x, double y);
};

class Switch: public Machine{

public:

    Switch(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
};

class Transformer: public Machine{

public:

    Transformer(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
};

class JunctionBox: public Machine{

public:

    JunctionBox(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
};

class DataDuplicator: public Machine{

public:

    DataDuplicator(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
};

class Battery12V: public Machine{

public:

    Battery12V(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
};

class Battery240V: public Machine{

public:

    Battery240V(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
};

class WarpDrive: public Machine{

public:

    WarpDrive(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
};

class LEDBar: public Machine{

public:

    LEDBar(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
};

class Ironworker: public Machine{

public:

    Ironworker(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
    virtual void draw();

    Animator top_animator;
    Sprite top_sprite;
};
//
//class Militant: public Machine{
//
//public:
//
//    Militant(string new_sector_id, string e_id, double x, double y);
//};

class Piston: public Machine{

public:

    Piston(string new_sector_id, string e_id, double x, double y, double rotation, bool loading);
    virtual bool performFunction(double dt);
    virtual void run(double dt);
    virtual void draw();

    vector<Sprite> scissor_bars;
};

class PistonHead: public Machine{


public:

    PistonHead(string new_sector_id, string e_id, double x, double y, double rotation);
};

class GasPump: public Machine{

public:

    GasPump(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
};

class CombustionThruster: public Machine{

public:

    CombustionThruster(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);

    bool needs_reset = false;
};

class PlasmaThruster: public Machine{

public:

    PlasmaThruster(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
};

class Lever: public Machine{


public:

    Lever(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
};

class PressureGauge: public Machine{


public:

    PressureGauge(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
    virtual void draw();

    double max_bound; //psi
    double turn_speed; //degrees per second
    double max_rotation; //degrees
    double current_rotation;

    Vector2f arm_coords;
    RectangleShape arm;
};

class ControlBoard: public Machine{

public:

    ControlBoard(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
    virtual void draw();
};

class Beacon: public Machine{

public:

    Beacon(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);
    virtual void draw();
};

string registerNewMachine(string sector_id, string new_id, double x, double y, double rotation, string type, bool loading = false);

#endif
