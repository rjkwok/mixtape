#ifndef WEC_MACH
#define WEC_MACH

#include "main.h"

using namespace std;
using namespace sf;


struct FlowCell
{
    FlowCell();

    string id;
    map<string, double> vars;
    map<string, string> data;

    virtual void recalculate();
    virtual map<string, double> take(double);
};

struct GPIOCell: public FlowCell{

    GPIOCell(string new_id);
};

struct PowerCell: public FlowCell{

    PowerCell(string new_id, double max_charge);

    double charge_record[4] = {0.0,0.0,0.0,0.0};
    virtual void recalculate();
};

struct ItemCell: public FlowCell{

    ItemCell(string new_id, int slot_id);
};

struct GasCell: public FlowCell{

    GasCell(string new_id, double new_volume);

    virtual void recalculate();
    virtual map<string, double> take(double amount);
};


struct FlowPort
{
    //object that provides a visual interface for connecting machines together, and also limits how many machines can connect to one another
    FlowPort(string new_sector_id, string new_id, Vector2f new_relative_coords, double new_relative_rotation, string new_machine_id, string type);

    string id;
    shared_ptr<Properties> properties;
    Sprite sprite;
    string machine_id;
    map<int, string> cell_ids;

    bool closed = false;
    bool is_input = false;

    string sector_id;
    Vector2f relative_coords = Vector2f(0,0); //from center of parent machine
    double relative_rotation = 0;

    void tapCell(string);
    void tapCell(string, int);

    string connection; //id of a transfer type machine
    bool isConnected(); //if this port is already connected to something

    void updatePosition(Vector2f parent_coords, double parent_rotation);
    virtual void run(double dt);
    shared_ptr<FlowCell> getCell(int i = 1);

    double voltage = 0.0;
    double voltage_in = 0.0;

    void light(double amount);
    double light_level = 100;
};

class Machine: public Entity
{
    public:
    //entity that contains items of a medium tier and determines their combined output

    Machine();
    Machine(string new_sector_id, string e_id, double x, double y, string type);

    void disconnect();

    int getFirstUnusedSlot();

    virtual void run(double dt); //should not be redefined in subclasses of Machine
    virtual void draw();

    map<string, shared_ptr<FlowCell> > internal_cells;
    map<string, shared_ptr<FlowPort> > ports;

    void addPort(string new_id, Vector2f new_coords, double new_rotation, string type);

//    void eject(string cell_id, double kg);
//    void inject(string cell_id, double kg);

    virtual bool performFunction(double dt); //redefined for every machine subclass

    double voltage_rating;
    double wattage;

    bool takes_input = false;
    bool can_be_occupied = false;
    void readInput(InputData new_input);
    InputData input;
};


string makeNewMachine(string sector_id, string ent_id, double x, double y, double rotation, string type, bool loading = false);

#endif // WEC_MACH
