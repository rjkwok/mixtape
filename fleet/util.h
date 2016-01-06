#ifndef FLEET_UTIL
#define FLEET_UTIL

#include "main.h"

using namespace std;
using namespace sf;

string asString(double x);
string asString(int x);

CircleShape makeCircle(double radius, Color fill_colour, Vector2f position);
RectangleShape makeRect(double left, double top, double width, double height, Color colour);

map<int, map<int, Vector2f> > calculateSockets(Vector2f core_a, Vector2f core_b);

struct Properties{

	Properties();
	~Properties();

	string name;
	string classification;
	int base_strength;
	int main_window_x;
	int main_window_y;
	Texture* main_tex;
	Texture* thumb_tex;
	set<string> abilities;
};

Properties* getProperties(string type_id);
void loadProperties();
void destroyProperties();

Texture* getTexture(string txid);
void loadTextures();
void destroyTextures();

Font* getFont(string font_id);
void loadFonts();
void destroyFonts();

struct CardSocket{

	CardSocket();
	CardSocket(string c_id, Vector2f c_point, double c_spin);

	Vector2f getCenter();

	double spin;
	Vector2f point;
	string id;

	string linked[3];

	int firstFree();
};


struct Ship{

	
	Ship();
	Ship(int c_owner, string c_type_id);

	string type_id;
	//Sprite sprite;

	double strength;
	vector<double> additions;
	vector<double> multiplications;
	vector<double> support_additions;
	vector<double> support_multiplications;
	bool support_blocked;
	bool nullify_strength;

	string getClass();
	int getOwner();
	set<string>* getAbilities();
	double getBase(); //calculate the base strength to be divided, taking the property and condition modifiers as parameters

	int owner;

};

struct CardGraphic{

	CardGraphic();
	CardGraphic(string card_name, double strength, Color frame_colour, Color strength_colour, Color name_colour, bool hidden = false);

	Sprite frame_sprite;
	Sprite main_sprite;
	Text name_text;
	Text strength_text;

	bool hidden_mode;

	double scale;

	void setScale(double factor);
	void setCenter(Vector2f position);
	void draw();
};

Ship* randomNewCard(int c_owner);

struct Scroller{

	Scroller();
	Scroller(Vector2f c_anchor, double width, double height);
	~Scroller();

	Sprite a;
	Sprite b;
	
	IntRect bounds;
	char current_active = 'a';
	Vector2f anchor;
	int scroll_direction = 1;

	void scrollDownTo(string type_id);
	void scrollUpTo(string type_id);
	void process(double dt);
	void draw();
	bool contains(double x, double y);
	
};


#endif