#ifndef WEC_VIS
#define WEC_VIS

#include "main.h"

using namespace std;
using namespace sf;

void castLight(string sector_id, Vector2f point, double diffuse, double intensity);

void createEffect(string sector_id, Vector2f start_pos, string type);
void createEffect(string sector_id, Vector2f start_pos, Vector2f velocity, string type);

struct ArcShape{

    ArcShape(double arc_angle, double radius);

    ConvexShape shape;

    void setPosition(Vector2f position);
    void setRotation(double rotation);
    void setFillColor(Color color);
    void draw();
};


struct Caption{

    Caption();
    Caption(string contents, string font, Vector2f coords, int char_size, Color color = Color(255,255,255,200), string bias = "middle");

    Text text;

    void draw();
};

RectangleShape createBoundingRectangle(Sprite sprite, Color color);
CircleShape createBoundingCircle(Sprite sprite, Color color, double margin = 0);
RectangleShape createRectangle(Vector2f new_position, Vector2f new_size, int outline_width, Color fill_color, Color outline_color);
RectangleShape createLine(Vector2f start_position, Vector2f axis, double length, Color color);

struct Effect
{
    Effect();
    Effect(string new_sector_id, string cid, Vector2f startpos, string type);
    Effect(string new_sector_id, string cid, Vector2f startpos, Vector2f vel, string type);

    string sector_id;
    shared_ptr<Properties> properties;
    string id;

    Sprite sprite;
    Vector2f velocity = Vector2f(0,0);
    Vector2f center = Vector2f(0,0);

    int frame = 0;
    IntRect frame_window = IntRect(0,0,0,0);
    int max_frame = 0;
    double elapsed = 0;
    double FPS = 24;

    void run(double dt);
    void draw();
};


#endif // WEC_VIS


