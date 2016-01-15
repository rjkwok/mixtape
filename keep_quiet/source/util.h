#ifndef REB_UTIL
#define REB_UTIL

#include "main.h"

using namespace std;
using namespace sf;

struct TileProperties{

	TileProperties();

	string type_name;
	int type_id;
	vector<int> texture_indexes;
};

struct Worker{

	//entity that can fill production slots in structures
	//controls a representation of itself, a rebel npc, that moves and animates according to worker's current task

	Worker();

	string tasked_structure_id;
};



//generators
char rndChar();
int randInt(int ubound); //returns a random integer between 1 and ubound (randInt(1) will always return 1, randInt(2) will either return 1 or 2, etc.)
string createUniqueId();
struct Caption{

    Caption();
    Caption(string contents, string font, Vector2f coords, int char_size, Color color = Color(255,255,255,200), string bias = "middle");

    Text text;

    void draw(RenderWindow &window);
};

RectangleShape createBoundingRectangle(Sprite sprite, Color color);
CircleShape createBoundingCircle(Sprite sprite, Color color, double margin = 0);
RectangleShape createRectangle(Vector2f new_position, Vector2f new_size, int outline_width, Color fill_color, Color outline_color);
RectangleShape createLine(Vector2f start_position, Vector2f axis, double length, Color color);
Sprite createSprite(string texture_id, Vector2f position, string bias = "middle");
//

//useful transformations/conversions
string asString(int number);
string asString(double number);
Vector2f getPerpendicularAxis(Vector2f axis);
Vector2f normalize(Vector2f v);
Vector2f rotateAboutOrigin(Vector2f p, double degrees);
Sprite* formatSprite(Sprite* sprite, double x, double y);
Sprite* formatSprite(Sprite* sprite, double x, double y, double width, double height);
Vector2f wrapPoint(Vector2f point, int terrain_max_x);
double dot(Vector2f a, Vector2f b);
double getRotationFromAxis(Vector2f axis);
void untangleSprite(Sprite &sprite, FloatRect rect, int terrain_max_x);
void untangleSprites(Sprite &a, Sprite &b);
void keepSpriteOutOfTerrain(Sprite &sprite, Vector2f grid_ref, map<int,map<int,int> > &terrain, int terrain_max_x, int terrain_max_y);
//

//file and memory management
void loadConfigs();
void destroyTextures();
//

//grid management

//retrieval functions
Texture* getTexture(string key);
IntRect getTextureRect(string key);
//

//checks
double returnHigher(double a, double b);
double returnLower(double a, double b);
bool spritesIntersecting(Sprite a, Sprite b, double tolerance);
//

//operator extensions
Vector2f operator *(const Vector2f& left, const Vector2f& right);
Vector2f operator /(const Vector2f& left, const Vector2f& right);
Vector2f operator +(const Vector2f& left, const double& right);
Vector2f operator -(const Vector2f& left, const double& right);
Vector2f operator *(const Vector2f& left, const double& right);
Vector2f operator /(const Vector2f& left, const double& right);
//

#endif