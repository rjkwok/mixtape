#ifndef REB_UI
#define REB_UI

#include "main.h"

using namespace std;
using namespace sf;

struct InputStruct{

	InputStruct();
	void collect(RenderWindow &window, View &view, View &window_view);

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

	View* view;
	View* window_view;
	View* back_view_1;
	View* back_view_2;
	View* back_view_3;
};

struct VisualsStruct{

	//handles all processing and display of UI graphics such as highlights, text, etc.
	VisualsStruct();

	void update(InputStruct input);
	void draw(RenderWindow &window);
};

//tools for manipulating the view
void scaleView(View &view, View &window_view, double delta);
void translateView(View &view, View &window_view, double x, double y);
//

#endif