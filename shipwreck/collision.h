#ifndef WEC_COLLISION
#define WEC_COLLISION

#include "main.h"

using namespace std;using namespace sf;

double getGap(shared_ptr<Entity> A, shared_ptr<Entity> B, Vector2f axis);
bool pointInSprite(Sprite a, Vector2f point);
bool spritesIntersecting(Sprite a, Sprite b, double tolerance = 0);
bool spriteIntersectingShape(Sprite a, RectangleShape b, double tolerance = 0);
bool spriteIntersectingShape(Sprite a, CircleShape b, double tolerance = 0);
Vector2f correctPositions(shared_ptr<Entity> A, shared_ptr<Entity> B, double dt);
//Vector2f correctPositionsFloorbond(string A_id,string B_id, double dt);
//bool doesCollide(Entity *A,Entity *B); //checks collision of two entities
//bool roughDoesCollide(Entity *A, Entity *B);
//

//void colReact(string, string, double);//corrects collisions to abide by physics


struct CollisionGrid
{
    double cell_side_length;
    unordered_map<int,unordered_map<int,set<string> > > grid;
    set<string> limbo;

    int max_cell_x = 0;
    int max_cell_y = 0;
    int min_cell_x = 0;
    int min_cell_y = 0;

    CollisionGrid();
    CollisionGrid(string new_sector_id, double c_s_length, Vector2f top_left, Vector2f bottom_right);

    void reset();
    void removeObject(string);
    void insertObject(string);
    void findCollisions(double dt);
    void processCollisions(double dt);
    void insertField(string);
    void processFields();
    bool lineOfSight(string ent_id, Vector2f point);
    set<string> getNearby(Vector2f point, double nearby_radius = 0);
    set<string> getNearby(shared_ptr<Entity> ent);
    bool checkRadialCollision(double radius, Vector2f point, string omit);

    string sector_id;
};

void processCollisions(vector<pair<string, string> >  detected_groups, double dt);
void redistributeMomentum(shared_ptr<Entity> A, shared_ptr<Entity> B, double);

//utilities


#endif // WEC_COLLISION
