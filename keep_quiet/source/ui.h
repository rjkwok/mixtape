#ifndef REB_UI
#define REB_UI

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

};

struct VisualsStruct{

	//handles all processing and display of UI graphics such as highlights, text, etc.
	VisualsStruct();

	void update(InputStruct input);
	void draw();
};

//tools for manipulating the view
void scaleView(double delta);
void translateView(double x, double y);
//

#endif