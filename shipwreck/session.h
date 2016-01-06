//a session controls the game loop and can be paused, saved, or loaded from file
#ifndef WEC_SESS
#include "main.h"

using namespace std;
using namespace sf;

class Session{

public:

    Session();
    Session(string window_name);

    void bombardWithAsteroid();
    void createAsteroids(int amount);
    bool processGame();

    string player_id;
    InputData input;

    string sector_id = "A1";

    double dt;
    Clock timer;

    double time_til_asteroid;
    double time_til_rescue;
    double packages_saved;
    
    void updateGUI();
    void drawGUI();

    vector<Caption> gui_captions;
    vector<Sprite> gui_fixed_sprites;
    vector<CircleShape> gui_fixed_circles;
    vector<RectangleShape> gui_fixed_rectangles;
    vector<CircleShape> gui_circles;
    vector<RectangleShape> gui_rectangles;
    vector<Sprite> gui_sprites;
    string gui_source_object_id;
    string gui_hovering_item_id;
    vector<FloatRect> gui_slot_boxes;
    vector<string*> gui_slot_contents;
    bool free_build = false;
    bool inventory_open = false;
    vector<string> toolbox_types;
    bool toolbox_open = false;
    FloatRect inventory_bounds;
    FloatRect toolbox_bounds;
    string gui_machine_id_a;
    string gui_port_id_a;
    bool floor_tile_1_placed = false;
    Vector2f floor_tile_1_coords;
    double floor_tile_1_rotation;
    bool file_menu_open = false;
    bool save_menu_mode = false;
    bool load_menu_mode = false;
    string file_string = "";
    double blinker = 0;
    double list_start_index = 0;

    double scale = 1;
    View view;
    View fixedview;
};

#endif // WEC_SESS

