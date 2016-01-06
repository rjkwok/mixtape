#ifndef WEC_TERMINAL
#define WEC_TERMINAL

#include "main.h"

using namespace std;
using namespace sf;

struct Chip: public Item{

    Chip();
    Chip(string new_sector_id, string item_id);

    map<int, double> var_num;
    map<int, string> var_str;

    virtual void process(double dt);
};

struct SwitchChip: public Chip{

    SwitchChip();
    SwitchChip(string new_sector_id, string item_id);

    virtual void process(double dt);
};

struct BatteryChip: public Chip{

    BatteryChip();
    BatteryChip(string new_sector_id, string item_id);

    virtual void process(double dt);
};

class Terminal: public Machine{ //1 slot

public:

    Terminal(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);

    vector<Sprite> fixed_sprites;
    vector<CircleShape> fixed_circles;
    vector<RectangleShape> fixed_rectangles;
    vector<Caption> captions;

    FloatRect terminal_window;

    double progression = 0;
};

string registerNewItem(string,string,string);

#endif // WEC_TERMINAL
