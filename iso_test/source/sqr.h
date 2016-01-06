#ifndef ASH_SQR
#define ASH_SQR

#include "main.h"

using namespace std;
using namespace sf;

struct SquareSprite{

	SquareSprite();
	SquareSprite(string c_type_id, double x, double y, double z);

	string type_id;
	Sprite sprite;
	Vector3f position;
	double height;
	double side_length;
	double a;
	string direction;

	int faceIntersecting(Vector2f cursor); //cursor is input as window coords relative to the view. 0 is false, 1 is N face, 2 is E face, 3 is S face, 4 is W face, 5 is top
	bool cheapIntersecting(Vector2f cursor); //cursor is input as window coords relative to the view. 0 is false, 1 is "sprite texture contains cursor"
	bool mapIntersecting(Vector2f cursor); //cursor is input as window coords relative to the view. 0 is false, 1 is "2d-projected cursor is within 2d squaresprite bounds"

	int frame_index;
	double elapsed_time;
	double fps;
	int first_frame;
	int last_frame;

	double getScreenY(); //for determining draw order
	void play(int c_first_frame, int c_last_frame);
	void update(double dt);
	void draw();
};

#endif