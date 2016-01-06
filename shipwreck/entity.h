#ifndef WEC_ENTITY
#define WEC_ENTITY

#include "main.h"

using namespace std;
using namespace sf;

void loadAllProperties();

struct Properties
{
    Properties();

    Texture sheet;
    Texture icon;

    string name;
    string classification; //entity, floor, player, container, machine, etc.
    string details;

    double get(string key);
    map<string, double> properties;
};

bool propertiesExistFor(string key);
shared_ptr<Properties> getProperties(string key);

//void generateScrapMaps();
struct Animator{

    Animator();

    bool loop_completed = false;
    int frame_index_1 = 0;
    int frame_index_2 = 0;
    int previous_frame_index_1 = 0;
    int previous_frame_index_2 = 0;
    double fps = 24;
    double elapsed_loop_time = 0;

    Vector2f getFrameTopLeft(int frame_index, Sprite &sprite);
    void update(double dt, Sprite &sprite);
    void play(int new_frame_index_1, int new_frame_index_2);
};


class Entity
{
    //class defining the base for all other entities that exist in the game world. Binds everything to the laws of physics.
    public:

        //object identity
        string id;
        shared_ptr<Properties> properties;

        string sector_id;
        //constructor
        Entity();
        Entity(string new_sector_id, string e_id, double x, double y, string type);
        void constructEntity(string new_sector_id, string e_id, double x, double y, string type);

        //primary server interface
        virtual void run(double dt);
        virtual void draw();
        void runPhysicsAndGraphics(double dt);
        virtual void cleanup();

        Animator animator;

        //primary world interface
        virtual double getMyMass();
        double getMass();
        double getMyInertia();
        double getInertia();

         void setPosition(double x,double y);
        virtual void displace(double x, double y);
        void setRotation(double d);

        virtual void applyImpulse(double x,double y, bool apply_damage, bool apply_spin);
        virtual void applyImpulse(double x,double y, Vector2f, bool apply_damage, bool apply_spin);
        virtual void applyRotationalImpulse(double d);

        void setVelocity(double x,double y);//should rarely use
        Vector2f getAngularVelocity(double dt); //outputs the velocity as part of a bond
        Vector2f getAngularVelocity(double dt, Vector2f point); //outputs the velocity of a point relative to this entity
        void setRotationalVelocity(double d);//should rarely use

        //basic information interface
        Vector2f getPosition();
        Vector2f getUniverseCoords();
        double getRotation();
        Vector2f getMomentum();
        double getRotationalMomentum();
        Vector2f getVelocity();
        double getRotationalVelocity();

        void light(double amount);
        void overrideLight(int r,int g,int b);

        //physics support functions
        bool intersects(string);
        bool intersects(Vector2f point, double radius);
        bool contains(Vector2f);
        shared_ptr<Entity> getThis();

        Vector2f getTopRight();
        Vector2f getBottomLeft();

        Vector2f getd(int);
        Vector2f getMaxPoint(Vector2f axis);
        Vector2f getMinPoint(Vector2f axis);
        Vector2f getClosestPoint(Vector2f point);

        Vector2f getMyParallelAxis();
        Vector2f getMyPerpendicularAxis();
        void collectNormals(vector<Vector2f> &axises);

        //object data
        bool dead = false;
        bool prop = false;
        bool slaved = false;
        bool bound_to_sector = false;
        bool double_draw = false;
        bool ghost = false;
        
        Vector2f coords;
        Vector2f p = Vector2f(0,0);
        double rot_p = 0;
        double temperature = 0;

        double collision_radius;

        Sprite sprite;

        double light_level = 100;
        Color light_override = Color(0,0,0);

        bool bonded = false;
        string bond = "";
        set<string> connectors;
        void getConnected(set<string>*);
        void getConnectedNoPiston(set<string>*);
        void cleanupDeadConnectors();

        bool bonded_to_floor = false;

        set<string> floors;
        set<string> floor_bonds;

        map<int, string> contents;
        void setSlots(int);
        int getSlots();
        int getSlotIndexOf(string);
        void removeFromContents(string);
        bool addToContents(string);
        bool addToContents(string,int);

        double current_health = 0;//max health stored in info
        double getHealthPercentage();
        void applyImpulseDamage(double x, double y);


};

string makeNewEntity(string sector_id, string ent_id, double x, double y, string type);
string makeNewEntity(string sector_id, string ent_id, double x, double y, double rotation, string type);

bool operator<(const weak_ptr<Entity> &A, const weak_ptr<Entity> &B);
bool operator==(const weak_ptr<Entity> &A, const weak_ptr<Entity> &B);

#endif // WEC_ENTITY

