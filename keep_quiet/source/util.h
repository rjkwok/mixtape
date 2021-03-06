#ifndef REB_UTIL
#define REB_UTIL

#include "main.h"

using namespace std;
using namespace sf;

//generators
char rndChar();
int randInt(int ubound); //returns a random integer between 1 and ubound (randInt(1) will always return 1, randInt(2) will either return 1 or 2, etc.)
int randSign();
string createUniqueId();
struct Caption{

    Caption();
    Caption(string contents, string font, Vector2f coords, int char_size, Color color = Color(255,255,255,200), string bias = "middle");

    Text text;

    void draw(RenderWindow &window);
};

struct Animation{

	Animation();
	Animation(int c_start, int c_end, bool c_looping = false);

	int start_frame;
	int end_frame;
	bool is_looping;
	int current_frame;
	double time_since_last_frame;

	bool play(double dt, Sprite &sprite, double fps = 16.0);
};

RectangleShape createBoundingRectangle(Sprite sprite, Color color);
RectangleShape createBoundingRectangle(FloatRect rect, Color color);
CircleShape createBoundingCircle(Sprite sprite, Color color, double margin = 0);
CircleShape createCircle(double radius, Vector2f position, Color colour);
RectangleShape createRectangle(Vector2f new_position, Vector2f new_size, int outline_width, Color fill_color, Color outline_color);
RectangleShape createLine(Vector2f start_position, Vector2f axis, double length, Color color);
Sprite createSprite(string texture_id, Vector2f position, string bias = "middle");
Sprite* createNewSprite(string texture_id, Vector2f position, string bias = "middle");
//

//useful transformations/conversions
string asString(int number);
string asString(double number);
Vector2f getPerpendicularAxis(Vector2f axis);
Vector2f normalize(Vector2f v);
Vector2f rotateAboutOrigin(Vector2f p, double degrees);
double dot(Vector2f a, Vector2f b);
double getRotationFromAxis(Vector2f axis);
//

//file and memory management
void loadConfigs();
void destroyTextures();
//

//grid management

//retrieval functions
Texture* getTexture(string key);
IntRect getTextureRect(string key);
IntRect getFrame(int frame_index, Sprite &sprite);
//

//checks
double returnHigher(double a, double b);
double returnLower(double a, double b);

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