#ifndef ASH_UI
#define ASH_UI

#include "main.h"

using namespace std;
using namespace sf;

struct InputStruct{

	InputStruct();
	void collect();

	Vector2f view_mouse; //cursor coords that have been scaled to match the current view
	Vector2f window_mouse; //cursor coords in terms of the window

	double mmb_delta;
	bool lmb_released;
	bool rmb_released;
	bool lmb_held;
	bool rmb_held;
	set<string> keys_released;
	set<string> keys_held;
	set<char> text_entered;

	string selected_id;
	string selected_building;
	int selected_face;
};

struct VisualsStruct{ //contains and updates ui visuals only, reading only from precomputed values

	VisualsStruct();

	DiamondShape diamond_hl;
	IsoSideShape iso_hl;

	int hl_type; //0 is none, 1 is diamond, 2 is iso

	void update(InputStruct &input);
	void draw();	
};

//tools for manipulating the view
void scaleView(double delta);
void translateView(double x, double y);
void rotateViewClockwise();
void rotateViewCounterClockwise();
//

#endif