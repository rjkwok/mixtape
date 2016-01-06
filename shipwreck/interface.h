#ifndef WEC_INTS
#define WEC_INTS

#include "main.h"

using namespace std;
using namespace sf;


class Interface{

    public:
    //define an object that does not collide, positions itself relative to a singular machine, can buffer quantities and
    Interface();
    void disconnect();

    string sector_id;

    string id;
    shared_ptr<Properties> properties;

    string caption;

    string machine_id = "";
    string port_id = "";

    string machine_id_2 = "";//only used in transfer class
    string port_id_2 = "";//only used in transfer class

    shared_ptr<FlowPort> getPort();
    shared_ptr<Machine> getMachine();

    virtual void activate();

    void runPhysicsAndGraphics(double dt);

    virtual void run(double dt);
    virtual void draw();

    vector<Vector2f> relative_position;
    vector<double> relative_rotation;
    vector<Sprite> sprite;

    vector<double> light_level;

    map<string,double> var_num;
};


class Transfer: public Interface{

    public:

    Transfer();

    string physical_id;

    shared_ptr<FlowPort> getPort2();
    shared_ptr<Machine> getMachine2();

    void setConnection(shared_ptr<FlowPort> A, shared_ptr<FlowPort> B);
    virtual void run(double dt);

    virtual void generateSections();
    virtual void updateSections();
    virtual void transfer(double dt);
};

class TransferGas: public Transfer{

    public:

    TransferGas(string new_sector_id, string new_id, shared_ptr<FlowPort> A, shared_ptr<FlowPort> B, string type);
    virtual void transfer(double dt);
};

class TransferPower: public Transfer{

    public:

    TransferPower(string new_sector_id,string new_id, shared_ptr<FlowPort> A, shared_ptr<FlowPort> B);

    virtual void generateSections();
    virtual void updateSections();
    virtual void transfer(double dt);
};

class TransferGPIO: public Transfer{

public:

    TransferGPIO(string new_sector_id, string new_id, shared_ptr<FlowPort> A, shared_ptr<FlowPort> B);

    virtual void generateSections();
    virtual void updateSections();
    virtual void transfer(double dt);
};

string registerNewInterface(string sector_id, string new_id, string port_id, string machine_id, Vector2f new_coords, string type);
string registerNewTransfer(string sector_id, string new_id, string port_id_a, string port_id_b, string machine_id_a, string machine_id_b, string type);
string makeNewInterface(string sector_id, string new_id, string port_id, string machine_id, Vector2f     new_coords, string type);
string makeNewTransfer(string sector_id, string new_id, string port_id_a, string port_id_b, string machine_id_a, string machine_id_b, string type);
//
//
//class Conveyor: public Interface{
//
//public:
//
//    Conveyor();
//    Conveyor(string new_id,string m_id_1, string m_id_2, string p_id_1, string p_id_2);
//
//    int direction = 1;//1= 1 to 2
//    double time_until_next = 0;
//    double items_per_second = 1;
//    double wattage = 1; //joules/second
//    virtual bool transfer(double dt);
//
//    shared_ptr<FlowCell> power_buffer;
//    shared_ptr<FlowCell> rollers;
//};
//
//
//class GasPipe: public Interface{
//
//public:
//
//    GasPipe();
//    GasPipe(string new_id,string m_id_1, string m_id_2, string p_id_1, string p_id_2);
//
//    virtual bool transfer(double dt);
//};
//
//class Conduit: public Interface{
//
//public:
//
//    Conduit();
//    Conduit(string new_id,string m_id_1, string m_id_2, string p_id_1, string p_id_2);
//
//    int direction = 1;//1= 1 to 2
//    double wattage = 1; //joules/second
//    virtual bool transfer(double dt);
//};
#endif
