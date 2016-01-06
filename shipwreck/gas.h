#ifndef WEC_GAS
#define WEC_GAS

using namespace std;
using namespace sf;
//
//class Cloud{
//
//public:
//
//    Cloud();
//    Cloud(string new_sector_id, string new_id, double new_kilomoles, Vector2f new_coords, Vector2f new_velocity, string type);
//
//    string sector_id;
//    shared_ptr<Properties> properties;
//
//    string id;
//    Vector2f coords;
//    Vector2f p;
//    double kilomoles;
//    double radius;
//
//    double spin = 0;
//
//    Animator animator;
//
//    void run(double dt);
//    void draw();
//    Vector2f getMomentum();
//    void addKilomoles(double);
//    void applyImpulse(double,double);
//    bool inRange(Vector2f point, double range);
//
//    void split();
//
//    Sprite sprite;
//
//};

void applyHeat(string sector_id, double quantity, Vector2f point,double radius);
//
//string makeNewCloud(string new_sector_id, string new_id, double new_kilomoles, Vector2f new_coords, Vector2f new_velocity, string type);
//string makeNewCloud(string new_id, double new_kilomoles, Vector2f new_coords, Vector2f new_velocity, string type);
//


#endif // WEC_GAS
