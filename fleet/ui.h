#ifndef FLEET_UI
#define FLEET_UI
	
#include "main.h"

using namespace std;
using namespace sf;

struct Tile{

    Tile();
    Tile(double left, double bottom, double side_length, Texture &icon_tex, Color colour = Color(0,155,155,155));
    Tile(double left, double bottom, double side_length, string caption, Color colour = Color(0,155,155,155));
    
    bool is_text;

    FloatRect bounds;
    RectangleShape frame;
    Sprite icon;

    Text text;

    bool contains(double x, double y);
    void highlight();
    void draw();
};


//tile menus should store the tiles in a vector, so that there's a 1:1 correlation between the indexes of the tiles and the objects they are representing. Must be recalculated each tick though, as vector order may change.
class UI{

public:

	set<string> keys_released;
	set<string> keys_pressed;
	set<char> text_entered;
	Vector2f mouse;
	Vector2i fixed_mouse;
	double mmb_delta;
  Vector2f mouse_delta;
  Vector2f previous_mouse;

	UI();
	void initialize();
	void collect();
	void clearInput();
	void draw();
	bool keyWasReleased(string key_id);
	bool keyIsPressed(string key_id);
	Vector2f getMouse();
	Vector2i getFixedMouse();

	void updateGUI(MatchClient* current_round);

	//in the event that any representation of a ship is right-clicked, the ui should spawn a graphic display of that ship's details and ignore all input except the esc key until the esc key is pressed.
	

	//ui handles the selection of the ship

	Sprite selected_ship_thumb;
	Text score_text_a;
    Text score_text_b;
    Text total_score_text_a;
    Text total_score_text_b;
    Text ship_info_text;
    
  //  bool is_deck_open;
  //  bool is_dead_open;
  //  FloatRect deck_bounds;
  //  FloatRect dead_bounds;
    vector<CardGraphic> tiles_hand;
    vector<CardGraphic> tiles_for_sale;
    vector<Text> texts;
  //  vector<Tile> tiles_deck;
  //  vector<Tile> tiles_dead;
    Tile attack_button;
};

#endif