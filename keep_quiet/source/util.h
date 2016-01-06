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

//generators
char rndChar();
int randInt(int ubound); //returns a random integer between 1 and ubound (randInt(1) will always return 1, randInt(2) will either return 1 or 2, etc.)
string createUniqueId();
//

//useful transformations/conversions
string asString(int number);
string asString(double number);
Vector2f getPerpendicularAxis(Vector2f axis);
Vector2f normalize(Vector2f v);
Vector2f rotateAboutOrigin(Vector2f p, double degrees);
Sprite* formatSprite(Sprite* sprite, double x, double y);
Sprite* formatSprite(Sprite* sprite, double x, double y, double width, double height);
Vector2f wrapPoint(Vector2f point);
double dot(Vector2f a, Vector2f b);
void untangleSprite(Sprite &sprite, FloatRect rect);
void untangleSprites(Sprite &a, Sprite &b);
void keepSpriteOutOfTerrain(Sprite &sprite);
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