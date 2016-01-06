//a world struct contains and manages the sectors (which in turn contain objects)
//should be loadable from XML
#ifndef WEC_WORLD
#define WEC_WORLD

#include "main.h"

using namespace std;
using namespace sf;

//struct Fadeout{
//
//    Fadeout();
//    Fadeout(set<shared_ptr<Entity> >);
//
//    set<shared_ptr<Sprite> > sprites;
//    shared_ptr<Sprite> background;
//    double fade = 0;
//    double fade_rate = 0;
//
//    void run(double);
//    void drawBG();
//    void draw();
//
//    void convertToUniversal(string);
//    void convertToLocal(string);
//};


struct Sector
{
    Sector();
    Sector(string, double radius, string tex, double ambient, bool plnt);

    string id;
    Vector2f universe_coords = Vector2f(0,0);
    double radius = 0;
    bool planetside = false;

    string anchor_ent;

    bool deep = false;

    map<string, shared_ptr<Connector> > last_connectors;

    string backdrop_tx_path;
    Texture backdrop_tx;
    Sprite backdrop;

    double ambient_light = 55;

    map<string, shared_ptr<Entity> > ents;
    map<string, weak_ptr<Player> > people; //also in ents
    map<string, shared_ptr<Floor> > floors;
    map<string, shared_ptr<Connector> > connectors;
    map<string, shared_ptr<Poster> > posters;
    map<string, weak_ptr<Machine> > machines; //also in ents
    map<string, weak_ptr<Terminal> > terminals; //also in machines
    map<string, shared_ptr<Item> > items;
    map<string, weak_ptr<Chip> > chips; //also in items
    map<string, shared_ptr<Interface> > interfaces;
    map<string, shared_ptr<Rope> > ropes;

    map<string, shared_ptr<Field> > fields;

    map<string, shared_ptr<Bond> > bonds; //formed live from connectors' contents
    map<string, shared_ptr<Effect> > effects;
    vector<Sprite> lights;

    CollisionGrid collision_grid;
    CollisionGrid floor_grid;

    set<string> garbage_bin;

    void process(double dt);
    void draw();
    void drawBackdrop();

    //double getPressure(Vector2f coords, double radius);

    //map<string, double> liftKilograms(Vector2f coords, double radius, double kg);

    string getClassification(string lookup_id);

    shared_ptr<Entity> getEnt(string ent_id);
    shared_ptr<Machine> getMachine(string ent_id);


    void trash(string g_id);
    void emptyGarbageBin();

    void trashEmptyBonds();
    void resortIntoBonds(set<string> ent_ids);
    void resortAllObjectsIntoBonds();

    void boom(Vector2f coords, double energy, double scaling_per_meter);

};

shared_ptr<Sector> getSector(string which_sector);

struct Change{

    Change();
    Change(string c_id, Date c_min, Date c_relative = Date(0,0,0,0,0,0));

    string id;

    Date min_date = Date(0,0,0,0,0,0);

    map<string, bool> prereq_events;
    Date calculated_date = Date(0,0,0,0,0,0);
    Date relative_date = Date(0,0,0,0,0,0);

    bool triggered = false;

    void updateFlags();
    bool trigger();
};



class World
{
    public:

    World();

    void generateStation(double seed);

    bool loadWorldFromFile(string worldsavename);
    bool saveWorldToFile(string worldsavename);

    map<string, shared_ptr<Sector> > sectors;

    string id;

    map<string,pair<string,pair<string, Vector2f> > > transfers;

    void enactChanges();
    void transfer(string origin_sector_id, string transfer_ent_id, string target_sector_id, Vector2f target_sector_spawn);
    void getRestingBonded(string ent_id, map<string,Vector2f> &ents_to_move, string origin_sector_id, string transfer_ent_id, string target_sector_id, Vector2f target_sector_spawn);
    void processTransfers();

    map<string, shared_ptr<Change> > changes;
    map<string, Date> enacted_changes;

    Date date = Date(8, 19, 1996, 0, 0, 0);

};


//void sendEntireWorld(TcpSocket &socket);
//void receiveEntireWorld(TcpSocket &socket);

//struct Map
//{
//    Map();
//
//    Vector2f map_center = Vector2f(0,0);
//    double scale = 1;
//    bool enabled = false;
//    void toggle();
//
//    Sprite backdrop;
//    FloatRect map_window;
//    map<string,Sprite> sector_sprites;
//    map<string,CircleShape> sector_circles;
//
//    void update();
//    void draw();
//    double getScale();
//};
//
//
//void sendToTheDeep(shared_ptr<Entity>);
#endif // WEC_WORLD
