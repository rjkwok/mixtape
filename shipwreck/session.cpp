#include "main.h"

using namespace std;
using namespace sf;

extern RenderWindow window;
extern World world;

extern map<string, shared_ptr<Properties> > properties_map;

int randomSign(){

    int a = rand() % 100;
    if(a <= 50){
        return -1;
    }
    else{
        return 1;
    }
}

int randomAbsNumber(int max_bound){

    return rand() % max_bound;
}

void Session::bombardWithAsteroid(){

    double start_radius = getSector(sector_id)->radius + 1000 + (rand() % 2000);
    double target_radius = getSector(sector_id)->radius - 500*(rand() % int(getSector(sector_id)->radius/500));

    double start_angle = (rand() % 360) * (M_PI/180.0);
    double target_angle = (rand() % 360) * (M_PI/180.0);

    Vector2f start_location = Vector2f(start_radius*cos(start_angle),start_radius*sin(start_angle));
    Vector2f target_location = Vector2f(target_radius*cos(target_angle),target_radius*sin(target_angle));

    double h = hypot(target_location.x-start_location.x,target_location.y-start_location.y);
    Vector2f path_vector = Vector2f((target_location.x-start_location.x)/h,(target_location.y-start_location.y)/h);
    double speed = 500 + (rand() % 1000);
    Vector2f velocity = path_vector*speed;

    string asteroid_id;
    switch(rand() % 2){

        case 0:
            asteroid_id = makeNewEntity(sector_id, createUniqueId(), start_location.x, start_location.y, "Medium Asteroid 1");
            break;
        case 1:
            asteroid_id = makeNewEntity(sector_id, createUniqueId(), start_location.x, start_location.y, "Big Asteroid 1");
            break;
    }
    
    getSector(sector_id)->getEnt(asteroid_id)->setVelocity(velocity.x,velocity.y);
}

void Session::createAsteroids(int amount){

    double spawnable_radius = getSector(sector_id)->radius - 1000;

    for(int i = 0; i < amount; i++){ //i should be incremented whenever an iteration *successfully* completes

        //randomly determine a spawn location
        double spawn_radius = randomAbsNumber(spawnable_radius);
        double angle_in_degrees = randomAbsNumber(360);

        double x_sign = 1;
        double y_sign = -1;

        if(angle_in_degrees > 90){
            x_sign = 1;
            y_sign = 1;
        }
        if(angle_in_degrees > 180){
            x_sign = -1;
            y_sign = 1;
        }
        if(angle_in_degrees > 270){
            x_sign = -1;
            y_sign = -1;
        }

        while(angle_in_degrees > 90){ angle_in_degrees -= 90; }

        Vector2f spawn_coords = Vector2f(x_sign*spawn_radius*sin(((M_PI/180)*angle_in_degrees)),y_sign*spawn_radius*sin(((M_PI/180)*(90-angle_in_degrees))));

        //generate asteroid about this coordinate set

        double amount_of_rocks = 7+(randomAbsNumber(5)); //form between 5 and 7 rocks into one asteroid cluster
        set<string> rock_ids;
        for(int j = 0; j < amount_of_rocks; j++){
            int num = randomAbsNumber(10);
            if(num < 4){ //big rock
                rock_ids.insert(makeNewProp(sector_id, createUniqueId(), spawn_coords.x, spawn_coords.y, randomAbsNumber(360), "Big Asteroid 1"));
            }
            else if(num < 8){ //small rock
                rock_ids.insert(makeNewProp(sector_id, createUniqueId(), spawn_coords.x, spawn_coords.y, randomAbsNumber(360), "Medium Asteroid 1"));
            }
            else{
                rock_ids.insert(makeNewProp(sector_id, createUniqueId(), spawn_coords.x, spawn_coords.y, randomAbsNumber(360), "Huge Asteroid 1"));
            }
        }

        //now actually position these rocks
        set<string>::iterator parser = rock_ids.begin();
        Vector2f axis_a = getSector(sector_id)->getEnt(*parser)->getMyParallelAxis();
        Vector2f axis_b = getSector(sector_id)->getEnt(*parser)->getMyPerpendicularAxis();
        for(parser++; parser != rock_ids.end();){
            Vector2f displacement = (axis_a*randomAbsNumber(1500)*randomSign()) + (axis_b*randomAbsNumber(1500)*randomSign());
            shared_ptr<Entity> ent = getSector(sector_id)->getEnt(*parser);
            ent->displace(displacement.x,displacement.y);

            Vector2f decrement = displacement*(-0.01);
            for(int l = 0; l < 100; l++){
                int collisions_detected = 0;
                for(set<string>::iterator parser_2 = rock_ids.begin(); parser_2 != rock_ids.end(); parser_2++){
                    if(*parser_2 == *parser){continue;}
                    if(spritesIntersecting(ent->sprite,getSector(sector_id)->getEnt(*parser_2)->sprite,100)){
                        collisions_detected++;
                    }

                }
                if(collisions_detected > 1){
                    parser++;
                    break;
                }
                else{
                    ent->displace(decrement.x,decrement.y);
                }
            }
        }

        bool collision_detected = false;
        for(set<string>::iterator parser = rock_ids.begin(); parser != rock_ids.end(); parser++){
            shared_ptr<Entity> ent = getSector(sector_id)->getEnt(*parser);
            for(map<string,shared_ptr<Entity> >::iterator parser_2 = getSector(sector_id)->ents.begin(); parser_2 != getSector(sector_id)->ents.end(); parser_2++){
                if(rock_ids.count(parser_2->first)!=0){continue;}
                if(getSector(sector_id)->garbage_bin.count(parser_2->first)!=0){continue;}
                if(spritesIntersecting(ent->sprite, parser_2->second->sprite)){
                    collision_detected = true;
                    break;
                }
            }
            if(collision_detected){
                break;
            }
        }

        if(collision_detected){
            for(set<string>::iterator parser = rock_ids.begin(); parser != rock_ids.end(); parser++){
                getSector(sector_id)->trash(*parser);
            }
        }


    }
}

Session::Session(){}
Session::Session(string window_name)
{
    sector_id = "0_0";
    player_id = "player_1";
    window.create(VideoMode::getFullscreenModes()[0], window_name, Style::Fullscreen);
    window.setFramerateLimit(80);
    view.reset(FloatRect(window.getSize().x/2,window.getSize().y/2,window.getSize().x,window.getSize().y));
    fixedview = window.getView();

    toolbox_types = {"Steel Mount", "Steel Bracket", "Power Cable", "GPIO Cable", "Steel Pipe", "Hazard Pipe"};

    makeNewPlayer(sector_id, player_id, 0, 1, 0, "Human");

    time_til_asteroid = 5.0;
    time_til_rescue = 600.0;
    packages_saved = 0.0;
    dt = 0.0000001;

    //createAsteroids(30);
}

void Session::drawGUI(){

    window.setView(view);
    for(vector<CircleShape>::iterator parser = gui_circles.begin(); parser != gui_circles.end(); parser++){
        window.draw(*parser);
    }
    for(vector<RectangleShape>::iterator parser = gui_rectangles.begin(); parser != gui_rectangles.end(); parser++){
        window.draw(*parser);
    }
    for(vector<Sprite>::iterator parser = gui_sprites.begin(); parser != gui_sprites.end(); parser++){
        window.draw(*parser);
    }
    window.setView(fixedview);
    for(vector<CircleShape>::iterator parser = gui_fixed_circles.begin(); parser != gui_fixed_circles.end(); parser++){
        window.draw(*parser);
    }
    for(vector<RectangleShape>::iterator parser = gui_fixed_rectangles.begin(); parser != gui_fixed_rectangles.end(); parser++){
        window.draw(*parser);
    }
    for(vector<Sprite>::iterator parser = gui_fixed_sprites.begin(); parser != gui_fixed_sprites.end(); parser++){
        window.draw(*parser);
    }
    for(vector<Caption>::iterator parser = gui_captions.begin(); parser != gui_captions.end(); parser++){
        parser->draw();
    }
    window.setView(view);
}

void Session::updateGUI(){

    //INITIALIZE
    int alert_index = 0;
    gui_captions.clear();
    gui_fixed_sprites.clear();
    gui_fixed_circles.clear();
    gui_fixed_rectangles.clear();
    gui_circles.clear();
    gui_rectangles.clear();
    gui_sprites.clear();

    int minutes = floor(time_til_rescue/60.0);
    int seconds = floor(time_til_rescue - (minutes*60.0));
    if(time_til_rescue <= 0){
        minutes = 0;
        seconds = 0;
    }
    string seconds_string = asString(seconds);
    if(seconds >= 0 && seconds <= 9){
        seconds_string = "0" + seconds_string;
    }

    gui_captions.push_back(Caption(asString(minutes) + ":" + seconds_string, "font1", Vector2f(window.getSize().x/2.0,50), 72, Color(185,225,185,200), "middle"));
    //gui_captions.push_back(Caption(asString(1.0/dt), "font1", Vector2f(window.getSize().x/2.0,150), 72, Color(185,225,185,200), "middle"));
    
    if(!file_menu_open && gui_source_object_id == "" && input.keys_released.count("esc")!=0){ file_menu_open = true; }

    if(time_til_rescue > 0){

        Color gui_color_a;
        Color gui_color_b;
        if(getSector(sector_id)->planetside){
            gui_color_a =  Color(0,85,85,25);
            gui_color_b =  Color(40,40,40,25);
        }
        else{
            gui_color_a =  Color(0,155,155,0);
            gui_color_b =  Color(253,130,43,0);
        }

        

        shared_ptr<Player> player = getSector(sector_id)->people[player_id].lock();
        //

        //PRE-ANALYSIS

        double mouse_distance = hypot(player->coords.x - input.mouse.x, player->coords.y - input.mouse.y);
        bool in_range = (mouse_distance <= player->range);

        bool grabbing = false;
        for(set<string>::iterator parser = player->connectors.begin(); parser!=player->connectors.end(); parser++){
            if(getSector(player->sector_id)->connectors[*parser]->type=="GRAB"){
                grabbing = true;
                if(getSector(player->sector_id)->connectors[*parser]->a_id == player_id){
                    gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(getSector(player->sector_id)->connectors[*parser]->b_id)->sprite, gui_color_a + Color(0,0,0,155)));
                }
                else{
                    gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(getSector(player->sector_id)->connectors[*parser]->a_id)->sprite, gui_color_a + Color(0,0,0,155)));
                }
            }
        }

        string selected_object_id = "";
        string selected_port_id = "";

        set<string> nearby_ents = getSector(sector_id)->floor_grid.getNearby(input.mouse, getSector(sector_id)->getEnt(player_id)->collision_radius+50);
        for(set<string>::iterator parser = nearby_ents.begin(); parser != nearby_ents.end(); parser++){ //CHECK FLOORS FIRST
            Sprite ent_sprite = getSector(sector_id)->getEnt(*parser)->sprite;
            if(ent_sprite.getGlobalBounds().contains(input.mouse)){
                selected_object_id = *parser;
                break;
            }
        }
        nearby_ents = getSector(sector_id)->collision_grid.getNearby(input.mouse, getSector(sector_id)->getEnt(player_id)->collision_radius+50);
        double current_port_distance = 0;
        bool port_found = false;
        for(set<string>::iterator parser = nearby_ents.begin(); parser != nearby_ents.end(); parser++){ //NOW CHECK ENTS. ANY ENT FOUND WILL OVERRIDE ANY SELECTED FLOOR FROM THE PREVIOUS LOOP
            Sprite ent_sprite = getSector(sector_id)->getEnt(*parser)->sprite;
            if(ent_sprite.getGlobalBounds().contains(input.mouse)){
                if(*parser == player_id){ continue; }
                selected_object_id = *parser;
                selected_port_id = "";
            }
            if(getSector(sector_id)->machines.count(*parser)!=0){
                shared_ptr<Machine> machine = getSector(sector_id)->machines[*parser].lock();

                for(map<string, shared_ptr<FlowPort> >::iterator port_parser = machine->ports.begin(); port_parser != machine->ports.end(); port_parser++){

                    Vector2f disp = port_parser->second->sprite.getPosition() - input.mouse;
                    double h = hypot(disp.x, disp.y);
                    if(port_parser->second->connection != "" && getSector(sector_id)->interfaces[port_parser->second->connection]->sprite.size() == 1){
                        disp = getSector(sector_id)->interfaces[port_parser->second->connection]->sprite[0].getPosition() - input.mouse;
                        h = hypot(disp.x, disp.y);
                        if((!port_found || h < current_port_distance) && h <= hypot(getSector(sector_id)->interfaces[port_parser->second->connection]->sprite[0].getLocalBounds().width,getSector(sector_id)->interfaces[port_parser->second->connection]->sprite[0].getLocalBounds().height)){
                            selected_object_id = port_parser->second->connection;
                            selected_port_id = "";
                            current_port_distance = h;
                            port_found = true;
                        }
                    }
                    else{

                        if((!port_found || h < current_port_distance) && h <= hypot(port_parser->second->sprite.getLocalBounds().width, port_parser->second->sprite.getLocalBounds().height) + 8){
                            selected_object_id = *parser;
                            selected_port_id = port_parser->second->id;
                            current_port_distance = h;
                            port_found = true;
                        }
                    }

                }
            }
            for(set<string>::iterator c_parser = getSector(sector_id)->getEnt(*parser)->connectors.begin(); c_parser != getSector(sector_id)->getEnt(*parser)->connectors.end(); c_parser++){
                if(getSector(sector_id)->connectors[*c_parser]->has_sprite){
                    if(getSector(sector_id)->connectors[*c_parser]->sprite.getGlobalBounds().contains(input.mouse)){
                        selected_object_id = *c_parser;
                        selected_port_id = "";
                        break;
                    }
                }
            }
        }
        for(map<string, shared_ptr<Poster> >::iterator parser = getSector(sector_id)->posters.begin(); parser != getSector(sector_id)->posters.end(); parser++){
           if(nearby_ents.count(parser->second->parent_id) != 0){ //CHECK POSTERS HANGING FROM "NEARBY" ENTITITES. ANY POSTER FOUND WILL OVERRIDE ANY SELECTED ENT FROM THE PREVIOUS LOOP.
               if(pointInSprite(parser->second->sprite, input.mouse)){
                    selected_object_id = parser->first;
                    selected_port_id = "";
               }
           }
        }


        bool mouse_in_inventory = false;
        if(inventory_open){
            if(!inventory_bounds.contains(input.fixed_mouse)){
                inventory_open = false;
            }
            else{
                mouse_in_inventory = true;
            }
        }
        else{
            if(inventory_bounds.contains(input.fixed_mouse)){
                inventory_open = true;
                toolbox_open = false;
                mouse_in_inventory = true;
            }
        }

        if(toolbox_open){

            if(!toolbox_bounds.contains(input.fixed_mouse)){
                toolbox_open = false;
            }
            else{
                mouse_in_inventory = true;
            }
        }
        else{
            if(toolbox_bounds.contains(input.fixed_mouse)){
                toolbox_open = true;
                inventory_open = false;
                mouse_in_inventory = true;
            }

        }

        string* selected_slot = nullptr; //check every possible slot on screen to see if mouse is hovering over
        for(int i = 0; i < gui_slot_boxes.size(); i++){
            if(gui_slot_boxes[i].contains(input.fixed_mouse)){
                selected_slot = gui_slot_contents[i];
                break;
            }
        }


        gui_slot_boxes.clear();
        gui_slot_contents.clear();

        CircleShape circle_shape;
        circle_shape.setRadius(10000);
        circle_shape.setOutlineThickness(10);
        circle_shape.setOutlineColor(Color(255,0,0,255));
        circle_shape.setFillColor(Color(0,0,0,0));
        circle_shape.setOrigin(10000,10000);
        circle_shape.setPosition(0,0);
        gui_circles.push_back(circle_shape);

        //EVALUATE
        if(gui_hovering_item_id == ""){
            if(selected_slot != nullptr){ //RETURNS FALSE IF NO VALUE
                if(*selected_slot != ""){

                    gui_captions.push_back(Caption(getSector(sector_id)->items[*selected_slot]->properties->name, "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5 + (alert_index*12)), 28, Color(255,255,255,205), "left"));
                    alert_index++;
                    if(input.lmb_released){
                        gui_hovering_item_id = *selected_slot;
                        *selected_slot = "";

                        gui_machine_id_a = "";
                            gui_port_id_a = "";

                        floor_tile_1_placed = false;
                        floor_tile_1_coords = Vector2f(0,0);
                        floor_tile_1_rotation = 0;
                    }
                }
            }
        }
        else if(gui_hovering_item_id != ""){

            if(selected_slot != nullptr){
                if(*selected_slot != ""){
                    gui_captions.push_back(Caption(getSector(sector_id)->items[*selected_slot]->properties->name, "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5 + (alert_index*12)), 28, Color(255,255,255,205), "left"));
                    alert_index++;
                    if(input.lmb_released){
                        string id_to_switch = gui_hovering_item_id;
                        gui_hovering_item_id = *selected_slot;
                        *selected_slot = id_to_switch;

                        gui_machine_id_a = "";
                        gui_port_id_a = "";

                        floor_tile_1_placed = false;
                        floor_tile_1_coords = Vector2f(0,0);
                        floor_tile_1_rotation = 0;
                    }
                }
                else{
                    if(input.lmb_released){
                        *selected_slot = gui_hovering_item_id;
                        gui_hovering_item_id = "";

                        gui_machine_id_a = "";
                        gui_port_id_a = "";

                        floor_tile_1_placed = false;
                        floor_tile_1_coords = Vector2f(0,0);
                        floor_tile_1_rotation = 0;
                    }
                }
            }
        }

        if(gui_source_object_id != ""){

            string source_classification = getSector(sector_id)->getClassification(gui_source_object_id);

            if(source_classification == "POSTER"){ //POSTER GUI STRUCTURE
                gui_fixed_sprites.push_back(getSector(sector_id)->posters[gui_source_object_id]->poster);
                gui_fixed_sprites.back().setPosition(1440/2,900/2);
                if(input.lmb_released){
                    gui_source_object_id = "";
                }
            }
            if(source_classification == "WARP DRIVE"){
                RectangleShape bounding_box = createRectangle(Vector2f(720,450), Vector2f(400,400), 5, Color(0,155,155,105), Color(0,155,155,205));
                gui_fixed_rectangles.push_back(bounding_box);

                FloatRect bounds = bounding_box.getGlobalBounds();

                shared_ptr<Machine> machine = getSector(sector_id)->getMachine(gui_source_object_id);

                string sector_coords = "";
                for(string::iterator char_parser = sector_id.begin(); char_parser != sector_id.end(); char_parser++){
                    if(*char_parser == '_'){
                        sector_coords.push_back('.');
                        continue;
                    }
                    sector_coords.push_back(*char_parser);
                }
                gui_captions.push_back(Caption(sector_coords,"font1",Vector2f(bounds.left + 10,bounds.top+bounds.height-17),25,Color(255,255,255,225),"left"));
                gui_captions.push_back(Caption(asString(machine->internal_cells["Warp Target"]->vars["x"]) + "." + asString(machine->internal_cells["Warp Target"]->vars["y"]),"font1",Vector2f(bounds.left + bounds.width - 10,bounds.top+bounds.height-17),25,Color(255,255,255,225),"right"));

                if(input.keys_released.count("w")!=0){
                    machine->internal_cells["Warp Target"]->vars["y"]++;
                }
                if(input.keys_released.count("s")!=0){
                    machine->internal_cells["Warp Target"]->vars["y"]--;
                }
                if(input.keys_released.count("a")!=0){
                    machine->internal_cells["Warp Target"]->vars["x"]--;
                }
                if(input.keys_released.count("d")!=0){
                    machine->internal_cells["Warp Target"]->vars["x"]++;
                }
            }
            if(source_classification == "TERMINAL"){ //TERMINAL GUI STRUCTURE
                shared_ptr<Terminal> terminal = getSector(sector_id)->terminals[gui_source_object_id].lock();

                gui_captions.insert(gui_captions.end(), terminal->captions.begin(), terminal->captions.end());
                gui_fixed_sprites.insert(gui_fixed_sprites.end(), terminal->fixed_sprites.begin(), terminal->fixed_sprites.end());
                gui_fixed_circles.insert(gui_fixed_circles.end(), terminal->fixed_circles.begin(), terminal->fixed_circles.end());
                gui_fixed_rectangles.insert(gui_fixed_rectangles.end(), terminal->fixed_rectangles.begin(), terminal->fixed_rectangles.end());

                RectangleShape slot;
                slot.setSize(Vector2f(80,80));
                slot.setOrigin(40, 40);
                slot.setFillColor(gui_color_b - Color(40,40,40,0) + Color(0,0,0,25));
                slot.setOutlineColor(gui_color_b + Color(0,0,0,155));
                slot.setOutlineThickness(2);
                slot.setPosition(terminal->terminal_window.left + terminal->terminal_window.width + 55, terminal->terminal_window.top + 45);
                if(slot.getGlobalBounds().contains(input.fixed_mouse)){
                    slot.setScale(1.1,1.1);
                }
                gui_fixed_rectangles.push_back(slot);
                gui_slot_boxes.push_back(slot.getGlobalBounds());
                gui_slot_contents.push_back(&terminal->contents[1]);

                if(terminal->contents[1] != ""){
                    Sprite icon;
                    icon.setTexture(getSector(sector_id)->items[terminal->contents[1]]->properties->icon, true);
                    IntRect icon_size = icon.getTextureRect();
                    icon.setOrigin(icon_size.width/2, icon_size.height/2);
                    icon.setColor(Color(255,255,255,225));
                    icon.setPosition(slot.getPosition());
                    if(slot.getGlobalBounds().contains(input.fixed_mouse)){
                        icon.setScale(1.1,1.1);
                    }
                    gui_fixed_sprites.push_back(icon);
                }

                if(!mouse_in_inventory && !terminal->terminal_window.contains(input.fixed_mouse) && !slot.getGlobalBounds().contains(input.fixed_mouse)){
                    if(input.lmb_released){
                        gui_source_object_id = "";
                    }
                }
            }
            if(source_classification == "CONTAINER"){ //CONTAINER GUI STRUCTURE
                shared_ptr<Entity> container = getSector(input.sector_id)->getEnt(gui_source_object_id);

                int items_per_row = sqrt(container->getSlots());
                double side_length = (items_per_row*90) + 10;

                RectangleShape window_rectangle;
                window_rectangle.setSize(Vector2f(side_length, side_length));
                window_rectangle.setOrigin(side_length/2, side_length/2);
                window_rectangle.setFillColor(gui_color_a + Color(0,0,0,0));
                window_rectangle.setOutlineColor(gui_color_b + Color(0,0,0,155));
                window_rectangle.setOutlineThickness(3);
                window_rectangle.setPosition(720, 450);
                gui_fixed_rectangles.push_back(window_rectangle);

                for(int i = 1;  i <= container->getSlots(); i++){

                    RectangleShape rectangle;
                    rectangle.setSize(Vector2f(80, 80));
                    rectangle.setOrigin(80/2, 80/2);
                    rectangle.setFillColor(gui_color_b - Color(40,40,40,0) + Color(0,0,0,15));
                    rectangle.setOutlineColor(gui_color_b + Color(0,0,0,155));
                    rectangle.setOutlineThickness(2);
                    rectangle.setPosition((720 - (side_length/2) + 50) + (((i-1)%items_per_row)*90), (450 - (side_length/2)+ 50) + (floor((i-1)/items_per_row)*90));
                    if(rectangle.getGlobalBounds().contains(input.fixed_mouse)){
                            rectangle.setScale(1.1,1.1);
                    }
                    gui_fixed_rectangles.push_back(rectangle);
                    gui_slot_boxes.push_back(rectangle.getGlobalBounds());
                    gui_slot_contents.push_back(&container->contents[i]);

                    if(getSector(input.sector_id)->items.count(container->contents[i]) != 0){

                        Sprite icon;
                        icon.setTexture(getSector(input.sector_id)->items[container->contents[i]]->properties->icon, true);
                        IntRect bounds = icon.getTextureRect();
                        icon.setOrigin(bounds.width/2,bounds.height/2);
                        icon.setColor(Color(255,255,255,225));
                        icon.setPosition(rectangle.getPosition());
                        if(rectangle.getGlobalBounds().contains(input.fixed_mouse)){
                            icon.setScale(1.1,1.1);
                        }
                        gui_fixed_sprites.push_back(icon);
                    }
                }

                if(!mouse_in_inventory && !window_rectangle.getGlobalBounds().contains(input.fixed_mouse)){
                    if(input.lmb_released){
                        gui_source_object_id = "";
                    }
                }
            }

            if(input.keys_released.count("esc") != 0){
                gui_source_object_id = "";
            }


        }
        else{

            if(!mouse_in_inventory && gui_hovering_item_id == "" && selected_port_id != "" && getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->connection != ""){

                if(in_range){
                    gui_captions.push_back(Caption("[RMB] TO DISCONNECT.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                    alert_index++;

                    gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_b - Color(40,40,40,0) + Color(0,0,0,55)));
                    gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->sprite, gui_color_b + Color(0,0,0,120), 8));
                    if(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->properties->get("port_type") == 0){
                       gui_captions.push_back(Caption("+"+asString(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->voltage)+"V", "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 34, Color(255,255,255,205), "left"));
                    }
                    else if(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->properties->get("port_type") == 2){
                        gui_captions.push_back(Caption(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->getCell()->id, "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 34, Color(255,255,255,205), "left"));
                    }


                    if(input.rmb_released){
                        gui_hovering_item_id = registerNewItem(sector_id, createUniqueId(),getSector(sector_id)->interfaces[getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->connection]->properties->name);
                        getSector(sector_id)->trash(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->connection);
                    }
                }
                else{
                    gui_captions.push_back(Caption("[RMB] TO DISCONNECT. (TOO FAR AWAY!)", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                    alert_index++;

                    gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(selected_object_id)->sprite, Color(255,55,55,55)));
                    gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->sprite, Color(255,55,55,120), 8));
                }

            }
            else if(!mouse_in_inventory && (gui_hovering_item_id == "" || getSector(sector_id)->items[gui_hovering_item_id]->properties->classification == "CHIP") && selected_object_id != ""){
                string selected_classification = getSector(sector_id)->getClassification(selected_object_id);

                if(selected_classification == "POSTER"){

                    if(in_range){
                        gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->posters[selected_object_id]->sprite, gui_color_b + Color(0,0,0,120)));
                        gui_captions.push_back(Caption("[LMB] TO VIEW POSTER.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;
                        if(input.lmb_released && gui_hovering_item_id == ""){
                            gui_source_object_id = selected_object_id;
                        }
                        if(!player->slaved){
                            gui_captions.push_back(Caption("[ALT-RMB] TO PICK UP.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                            alert_index++;
                            gui_circles.push_back(createBoundingCircle(getSector(sector_id)->posters[selected_object_id]->sprite, gui_color_a + Color(0,0,0,55), 12));
                            if(input.rmb_released && input.keys_held.count("alt") != 0){
                                string new_id = createUniqueId();
                                if(player->addToContents(registerNewItem(sector_id, new_id, getSector(sector_id)->posters[selected_object_id]->properties->name))){
                                    getSector(sector_id)->trash(selected_object_id);
                                }
                                else{
                                    getSector(sector_id)->trash(new_id);
                                }
                            }
                        }
                    }
                    else{
                        gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->posters[selected_object_id]->sprite, Color(255,55,55,120)));
                        gui_captions.push_back(Caption("[LMB] TO VIEW POSTER. (TOO FAR AWAY!)", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                        alert_index++;

                        if(!player->slaved){
                            gui_captions.push_back(Caption("[ALT-RMB] TO PICK UP. (TOO FAR AWAY!)", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                            alert_index++;
                            gui_circles.push_back(createBoundingCircle(getSector(sector_id)->posters[selected_object_id]->sprite, gui_color_a + Color(255,55,55,55), 12));
                        }
                    }

                }
                if(selected_classification == "WARP DRIVE"){
                    if(selected_port_id == ""){
                        gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_b + Color(0,0,0,120)));
                    }
                    else{
                        gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_b - Color(40,40,40,0) + Color(0,0,0,55)));
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->sprite, gui_color_b + Color(0,0,0,120), 8));
                        if(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->properties->get("port_type") == 0){
                           gui_captions.push_back(Caption("+"+asString(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->voltage)+"V", "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 34, Color(255,255,255,205), "left"));
                        }
                        else if(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->properties->get("port_type") == 2){
                            gui_captions.push_back(Caption(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->getCell()->id, "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 34, Color(255,255,255,205), "left"));
                        }
                    }
                    gui_captions.push_back(Caption("[LMB] TO ACCESS CONTROL PANEL.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                    alert_index++;
                    if(input.lmb_released){
                        gui_source_object_id = selected_object_id;
                    }
                    if(!player->slaved && !grabbing && selected_port_id == ""){
                        gui_captions.push_back(Caption("[RMB] TO GRAB.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_a + Color(0,0,0,55), 12));
                        if(!getSector(sector_id)->getEnt(selected_object_id)->bonded && !getSector(sector_id)->getEnt(selected_object_id)->bound_to_sector){
                            if(input.rmb_released && input.keys_held.count("alt") == 0){
                                player->grab(selected_object_id);
                            }
                        }
                        else{
                            if(input.rmb_released){
                                player->grab(selected_object_id);
                            }
                        }
                    }
                    if(!player->slaved && selected_port_id == "" && !getSector(sector_id)->getEnt(selected_object_id)->bound_to_sector && !getSector(sector_id)->getEnt(selected_object_id)->bonded){
                        gui_captions.push_back(Caption("[ALT-RMB] TO PICK UP.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_a + Color(0,0,0,55), 12));
                        if(input.rmb_released && input.keys_held.count("alt") != 0){
                            string new_id = createUniqueId();
                            if(player->addToContents(registerNewItem(sector_id, new_id, getSector(sector_id)->getEnt(selected_object_id)->properties->name))){
                                getSector(sector_id)->trash(selected_object_id);
                            }
                            else{
                                getSector(sector_id)->trash(new_id);
                            }
                        }
                    }
                }
                if(selected_classification == "CONNECTOR"){
                    gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->connectors[selected_object_id]->sprite, gui_color_b + Color(0,0,0,120)));
                    gui_captions.push_back(Caption("HOLD [LMB] TO REMOVE.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                    alert_index++;

                    RectangleShape integrity_bar;
                    integrity_bar.setSize(Vector2f(getSector(sector_id)->connectors[selected_object_id]->integrity,3));
                    integrity_bar.setOrigin(0,1.5);
                    integrity_bar.setPosition(input.fixed_mouse.x,input.fixed_mouse.y-20);
                    integrity_bar.setFillColor(Color(255,55,55,225));
                    integrity_bar.setOutlineThickness(0);
                    gui_fixed_rectangles.push_back(integrity_bar);
                    if(input.lmb_held){
                        getSector(sector_id)->connectors[selected_object_id]->integrity -= (dt*10);
                        if(getSector(sector_id)->connectors[selected_object_id]->integrity <= 0){
                            getSector(sector_id)->trash(selected_object_id);
                            createEffect(sector_id, input.mouse, Vector2f(0,0), "Smoke");
                        }
                    }
                }
                if(selected_classification == "TERMINAL"){
                    if(selected_port_id == ""){
                        gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_b + Color(0,0,0,120)));
                    }
                    else{
                        gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_b - Color(40,40,40,0) + Color(0,0,0,55)));
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->sprite, gui_color_b + Color(0,0,0,120), 8));
                        if(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->properties->get("port_type") == 0){
                           gui_captions.push_back(Caption("+"+asString(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->voltage)+"V", "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 34, Color(255,255,255,205), "left"));
                        }
                        else if(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->properties->get("port_type") == 2){
                            gui_captions.push_back(Caption(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->getCell()->id, "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 34, Color(255,255,255,205), "left"));
                        }
                    }
                    gui_captions.push_back(Caption("[LMB] TO ACCESS TERMINAL.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                    alert_index++;
                    if(input.lmb_released){
                        gui_source_object_id = selected_object_id;
                    }
                    if(!player->slaved && !grabbing && selected_port_id == ""){
                        gui_captions.push_back(Caption("[RMB] TO GRAB.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_a + Color(0,0,0,55), 12));
                        if(!getSector(sector_id)->getEnt(selected_object_id)->bonded && !getSector(sector_id)->getEnt(selected_object_id)->bound_to_sector){
                            if(input.rmb_released && input.keys_held.count("alt") == 0){
                                player->grab(selected_object_id);
                            }
                        }
                        else{
                            if(input.rmb_released){
                                player->grab(selected_object_id);
                            }
                        }
                    }
                    if(!player->slaved && selected_port_id == "" && !getSector(sector_id)->getEnt(selected_object_id)->bound_to_sector && !getSector(sector_id)->getEnt(selected_object_id)->bonded){
                        gui_captions.push_back(Caption("[ALT-RMB] TO PICK UP.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_a + Color(0,0,0,55), 12));
                        if(input.rmb_released && input.keys_held.count("alt") != 0){
                            string new_id = createUniqueId();
                            if(player->addToContents(registerNewItem(sector_id, new_id, getSector(sector_id)->getEnt(selected_object_id)->properties->name))){
                                getSector(sector_id)->trash(selected_object_id);
                            }
                            else{
                                getSector(sector_id)->trash(new_id);
                            }
                        }
                    }
                }
                if(selected_classification == "CONTAINER"){
                    gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_b + Color(0,0,0,120)));
                    gui_captions.push_back(Caption("[LMB] TO SEARCH CONTENTS.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                    alert_index++;
                    if(input.lmb_released && gui_hovering_item_id == ""){
                        gui_source_object_id = selected_object_id;
                    }
                    if(!player->slaved && !grabbing && selected_port_id == ""){
                        gui_captions.push_back(Caption("[RMB] TO GRAB.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_a + Color(0,0,0,55), 12));
                        if(!getSector(sector_id)->getEnt(selected_object_id)->bonded && !getSector(sector_id)->getEnt(selected_object_id)->bound_to_sector){
                            if(input.rmb_released && input.keys_held.count("alt") == 0){
                                player->grab(selected_object_id);
                            }
                        }
                        else{
                            if(input.rmb_released){
                                player->grab(selected_object_id);
                            }
                        }
                    }
                    if(!player->slaved && !getSector(sector_id)->getEnt(selected_object_id)->bound_to_sector && !getSector(sector_id)->getEnt(selected_object_id)->bonded){
                        gui_captions.push_back(Caption("[ALT-RMB] TO PICK UP.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_a + Color(0,0,0,55), 12));
                        if(input.rmb_released && input.keys_held.count("alt") != 0){
                            string new_id = createUniqueId();
                            if(player->addToContents(registerNewItem(sector_id, new_id, getSector(sector_id)->getEnt(selected_object_id)->properties->name))){
                                getSector(sector_id)->trash(selected_object_id);
                            }
                            else{
                                getSector(sector_id)->trash(new_id);
                            }
                        }
                    }
                }
                if(selected_classification == "ENTITY"){
                    gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_b + Color(0,0,0,120)));
                    if(!player->slaved && !grabbing && selected_port_id == ""){
                        gui_captions.push_back(Caption("[RMB] TO GRAB.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_a + Color(0,0,0,55), 12));
                        if(!getSector(sector_id)->getEnt(selected_object_id)->bonded && !getSector(sector_id)->getEnt(selected_object_id)->bound_to_sector){
                            if(input.rmb_released && input.keys_held.count("alt") == 0){
                                player->grab(selected_object_id);
                            }
                        }
                        else{
                            if(input.rmb_released){
                                player->grab(selected_object_id);
                            }
                        }
                    }
                    if(!player->slaved && !getSector(sector_id)->getEnt(selected_object_id)->bound_to_sector && !getSector(sector_id)->getEnt(selected_object_id)->bonded){
                        gui_captions.push_back(Caption("[ALT-RMB] TO PICK UP.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_a + Color(0,0,0,55), 12));
                        if(input.rmb_released && input.keys_held.count("alt") != 0){
                            string new_id = createUniqueId();
                            if(player->addToContents(registerNewItem(sector_id, new_id, getSector(sector_id)->getEnt(selected_object_id)->properties->name))){
                                getSector(sector_id)->trash(selected_object_id);
                            }
                            else{
                                getSector(sector_id)->trash(new_id);
                            }
                        }
                    }
                }
                if(selected_classification == "FLOOR"){
                    gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_b + Color(0,0,0,120)));
                    if(!player->slaved && !grabbing && selected_port_id == ""){
                        gui_captions.push_back(Caption("[RMB] TO GRAB.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_a + Color(0,0,0,55), 12));
                        if(!getSector(sector_id)->getEnt(selected_object_id)->bonded && !getSector(sector_id)->getEnt(selected_object_id)->bound_to_sector){
                            if(input.rmb_released && input.keys_held.count("alt") == 0){
                                player->grab(selected_object_id);
                            }
                        }
                        else{
                            if(input.rmb_released){
                                player->grab(selected_object_id);
                            }
                        }
                    }
                    if(!player->slaved && !getSector(sector_id)->getEnt(selected_object_id)->bound_to_sector && !getSector(sector_id)->getEnt(selected_object_id)->bonded){
                        gui_captions.push_back(Caption("[ALT-RMB] TO PICK UP.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_a + Color(0,0,0,55), 12));
                        if(input.rmb_released && input.keys_held.count("alt") != 0){
                            string new_id = createUniqueId();
                            if(player->addToContents(registerNewItem(sector_id, new_id, getSector(sector_id)->getEnt(selected_object_id)->properties->name))){
                                getSector(sector_id)->trash(selected_object_id);
                            }
                            else{
                                getSector(sector_id)->trash(new_id);
                            }
                        }
                    }
                }
                if(selected_classification == "MACHINE"){

                    if(selected_port_id == ""){
                        gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_b + Color(0,0,0,120)));
                    }
                    else{
                        gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(selected_object_id)->sprite,gui_color_b - Color(40,40,40,0) + Color(0,0,0,55)));
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->sprite, gui_color_b + Color(0,0,0,120), 8));
                        if(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->properties->get("port_type") == 0){
                           gui_captions.push_back(Caption("+"+asString(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->voltage)+"V", "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 34, Color(255,255,255,205), "left"));
                        }
                        else if(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->properties->get("port_type") == 2){
                            gui_captions.push_back(Caption(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->getCell()->id, "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 34, Color(255,255,255,205), "left"));
                        }
                    }

                    if(getSector(sector_id)->machines.count(selected_object_id) != 0 && getSector(sector_id)->machines[selected_object_id].lock()->can_be_occupied){
                        if(!player->slaved && player->connectors.size()==0 && getSector(sector_id)->machines[selected_object_id].lock()->internal_cells["User Info"]->data["id"] == ""){
                            gui_captions.push_back(Caption("[LMB] TO USE THIS.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                            alert_index++;
                            if(input.lmb_released){
                                getSector(sector_id)->machines[selected_object_id].lock()->internal_cells["User Info"]->data["id"] = player_id;
                                player->slaved = true;
                            }
                        }
                        else if(getSector(sector_id)->machines[selected_object_id].lock()->internal_cells["User Info"]->data["id"] == player->id){
                            gui_captions.push_back(Caption("[LMB] TO STOP USING.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                            alert_index++;
                            if(input.lmb_released){
                                getSector(sector_id)->machines[selected_object_id].lock()->internal_cells["User Info"]->data["id"] = "";
                                player->slaved = false;
                                player->slaved = false;
                            }
                        }

                        if(!player->slaved && getSector(sector_id)->machines[selected_object_id].lock()->internal_cells["User Info"]->data["id"] != ""){
                            gui_captions.push_back(Caption("THIS IS ALREADY OCCUPIED.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                            alert_index++;
                        }
                        if(player->connectors.size()!=0){
                            gui_captions.push_back(Caption("YOU ARE CURRENTLY GRABBING SOMETHING.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                            alert_index++;
                        }
                        if(player->slaved && getSector(sector_id)->machines[selected_object_id].lock()->internal_cells["User Info"]->data["id"] == ""){
                            gui_captions.push_back(Caption("YOU CAN'T SIT IN TWO MACHINES AT ONCE.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                            alert_index++;
                        }
                    }
                 //   if(getSector(sector_id)->getEnt(selected_object_id)->properties->name == "Pressure Gauge"){

                       // gui_captions.push_back(Caption(asString(getSector(sector_id)->machines[selected_object_id].lock()->internal_cells["PSI Data Output"]->vars["value"])+" psi", "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 34, Color(255,255,255,205), "left"));
                  //  }
                    if(getSector(sector_id)->getEnt(selected_object_id)->properties->name == "Lever"){
                        gui_captions.push_back(Caption("[LMB] TO THROW LEVER.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;

                        if(input.lmb_released){
                            if(getSector(sector_id)->machines[selected_object_id].lock()->internal_cells["Lever State Output"]->vars["value"] == 0){
                                getSector(sector_id)->machines[selected_object_id].lock()->internal_cells["Lever State Output"]->vars["value"] = 1;
                            }
                            else{
                                getSector(sector_id)->machines[selected_object_id].lock()->internal_cells["Lever State Output"]->vars["value"] = 0;
                            }
                        }
                    }
                    if(!player->slaved && !grabbing && selected_port_id == ""){
                        gui_captions.push_back(Caption("[RMB] TO GRAB.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_a + Color(0,0,0,55), 12));
                        if(!getSector(sector_id)->getEnt(selected_object_id)->bonded && !getSector(sector_id)->getEnt(selected_object_id)->bound_to_sector){
                            if(input.rmb_released && input.keys_held.count("alt") == 0){
                                player->grab(selected_object_id);
                            }
                        }
                        else{
                            if(input.rmb_released){
                                player->grab(selected_object_id);
                            }
                        }

                    }
                    if(!player->slaved && !getSector(sector_id)->getEnt(selected_object_id)->bound_to_sector && selected_port_id == "" && !getSector(sector_id)->getEnt(selected_object_id)->bonded){
                        gui_captions.push_back(Caption("[ALT-RMB] TO PICK UP.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_a + Color(0,0,0,55), 12));
                        if(input.rmb_released && input.keys_held.count("alt") != 0){
                            string new_id = createUniqueId();
                            if(player->addToContents(registerNewItem(sector_id, new_id, getSector(sector_id)->getEnt(selected_object_id)->properties->name))){
                                getSector(sector_id)->trash(selected_object_id);
                            }
                            else{
                                getSector(sector_id)->trash(new_id);
                            }
                        }
                    }
                }
                if(selected_classification == "INTERFACE"){

                    if(getSector(sector_id)->interfaces[selected_object_id]->sprite.size() == 1){
                        gui_captions.push_back(Caption("[LMB] TO ACTIVATE", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->interfaces[selected_object_id]->sprite[0], gui_color_a + Color(0,0,0,200), 12));
                        if(input.lmb_released && (gui_hovering_item_id == "" || getSector(sector_id)->items[gui_hovering_item_id]->properties->classification == "CHIP")){
                            getSector(sector_id)->interfaces[selected_object_id]->activate();
                        }
                        if(gui_hovering_item_id == ""){
                            gui_captions.push_back(Caption("[RMB] TO DISCONNECT", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                            alert_index++;
                            if(input.rmb_released){
                                gui_hovering_item_id = registerNewItem(sector_id, createUniqueId(),getSector(sector_id)->interfaces[selected_object_id]->properties->name);
                                getSector(sector_id)->trash(selected_object_id);
                            }
                        }
                    }
                }
            }
            if(!mouse_in_inventory && gui_hovering_item_id != "" && getSector(sector_id)->items[gui_hovering_item_id]->properties->classification != "CHIP" && selected_slot == nullptr){

                //interact with world

                string selected_classification;
                if(selected_object_id != ""){
                    selected_classification = getSector(sector_id)->getClassification(selected_object_id);
                }

                if(selected_classification == "ENTITY" || selected_classification == "CONTAINER" || selected_classification == "FLOOR" || selected_classification == "MACHINE" || selected_classification == "TERMINAL" || selected_classification == "WARP DRIVE" || selected_classification == "CONTROLBOARD"){
                    if(selected_port_id == ""){
                        gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_b + Color(0,0,0,120)));
                    }
                    else{
                        gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_b - Color(40,40,40,0) + Color(0,0,0,55)));
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->sprite, gui_color_b + Color(0,0,0,120), 8));
                    }
                }

                if(gui_hovering_item_id != "" && getSector(sector_id)->getClassification(gui_hovering_item_id) == "POSTER"){

                    Sprite hovering_preview;
                    hovering_preview.setTexture(getProperties("PHYSICAL POSTER")->sheet, true);
                    IntRect preview_bounds = hovering_preview.getTextureRect();
                    hovering_preview.setOrigin(preview_bounds.width/2,preview_bounds.height/2);
                    hovering_preview.setPosition(input.mouse);

                    if(selected_classification == "ENTITY" || selected_classification == "CONTAINER" || selected_classification == "MACHINE" || selected_classification == "TERMINAL" || selected_classification == "CONTROLBOARD" || selected_classification == "WARP DRIVE"){
                        gui_captions.push_back(Caption("[LMB] TO HANG POSTER", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;

                        hovering_preview.setColor(gui_color_a + Color(0,0,0,200));

                        shared_ptr<Entity> parent = getSector(sector_id)->getEnt(selected_object_id);
                        double Q = (360 - parent->sprite.getRotation()) * (M_PI/180);
                        Vector2f position = input.mouse - parent->sprite.getPosition();
                        Vector2f mount_position =  Vector2f(position.x*cos(Q)-position.y*sin(Q),position.x*sin(Q)+position.y*cos(Q)) ;

                        FloatRect parent_bounds = parent->sprite.getLocalBounds();
                        map<string, double> displacement;
                        displacement["top"] = mount_position.y - (-parent_bounds.height/2);
                        displacement["bottom"] = mount_position.y - (parent_bounds.height/2);
                        displacement["right"] = mount_position.x - (parent_bounds.width/2);
                        displacement["left"] = mount_position.x - (-parent_bounds.width/2);

                        double closest_disp = -1;
                        string closest_side = "";
                        for(map<string, double>::iterator parser = displacement.begin(); parser != displacement.end(); parser++){

                            if(closest_disp < 0 || abs(parser->second) < closest_disp){
                                closest_disp = abs(parser->second);
                                closest_side = parser->first;
                            }
                        }


                        if(closest_side == "bottom"){
                            hovering_preview.setRotation(parent->sprite.getRotation());
                            mount_position.y = (parent_bounds.height/2) + (preview_bounds.height/2);
                        }
                        if(closest_side == "top"){
                            hovering_preview.setRotation(parent->sprite.getRotation()+180);
                            mount_position.y = (-parent_bounds.height/2) - (preview_bounds.height/2);
                        }
                        if(closest_side == "right"){
                            hovering_preview.setRotation(parent->sprite.getRotation()-90);
                            mount_position.x = (parent_bounds.width/2) + (preview_bounds.height/2);
                        }
                        if(closest_side == "left"){
                            hovering_preview.setRotation(parent->sprite.getRotation()+90);
                            mount_position.x = (-parent_bounds.width/2) - (preview_bounds.height/2);
                        }

                        Q = parent->sprite.getRotation() * (M_PI/180);
                        position =  parent->sprite.getPosition() + Vector2f(mount_position.x*cos(Q)-mount_position.y*sin(Q),mount_position.x*sin(Q)+mount_position.y*cos(Q)) ;
                        hovering_preview.setPosition(position);


                        if(input.lmb_released){
                            makeNewPoster(sector_id, createUniqueId(), getSector(sector_id)->items[gui_hovering_item_id]->properties->name, selected_object_id, mount_position);
                            getSector(sector_id)->trash(gui_hovering_item_id);

                            gui_hovering_item_id = "";
                        }
                    }
                    else{
                        gui_captions.push_back(Caption("AN OBJECT MUST BE SELECTED TO HANG A POSTER.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                        alert_index++;

                        hovering_preview.setColor(Color(255,55,55,200));
                    }
                    gui_sprites.push_back(hovering_preview);

                }
                else if(selected_classification != "" && (selected_classification == "ENTITY" || selected_classification == "CONTAINER" || selected_classification == "MACHINE" || selected_classification == "TERMINAL" || selected_classification == "CONTROLBOARD" || selected_classification == "WARP DRIVE") && !getSector(sector_id)->getEnt(selected_object_id)->prop && gui_hovering_item_id != "" && (getSector(sector_id)->getClassification(gui_hovering_item_id) == "ENTITY" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "CONTAINER" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "TERMINAL" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "CONTROLBOARD" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "MACHINE")){

                    shared_ptr<Entity> parent = getSector(sector_id)->getEnt(selected_object_id);
                    double Q = (360 - parent->sprite.getRotation()) * (M_PI/180);
                    Vector2f position = input.mouse - parent->sprite.getPosition();
                    Vector2f mount_position =  Vector2f(position.x*cos(Q)-position.y*sin(Q),position.x*sin(Q)+position.y*cos(Q)) ;

                    FloatRect parent_bounds = parent->sprite.getLocalBounds();
                    map<string, double> displacement;
                    displacement["top"] = mount_position.y - (-parent_bounds.height/2);
                    displacement["bottom"] = mount_position.y - (parent_bounds.height/2);
                    displacement["right"] = mount_position.x - (parent_bounds.width/2);
                    displacement["left"] = mount_position.x - (-parent_bounds.width/2);

                    double closest_disp = -1;
                    string closest_side = "";
                    for(map<string, double>::iterator parser = displacement.begin(); parser != displacement.end(); parser++){

                        if(closest_disp < 0 || abs(parser->second) < closest_disp){
                            closest_disp = abs(parser->second);
                            closest_side = parser->first;
                        }
                    }

                    Sprite hovering_preview;
                    IntRect hovering_rect(0,0,getSector(sector_id)->items[gui_hovering_item_id]->properties->get("width"), getSector(sector_id)->items[gui_hovering_item_id]->properties->get("height"));
                    hovering_preview.setTexture(getSector(sector_id)->items[gui_hovering_item_id]->properties->sheet, false);
                    hovering_preview.setOrigin(hovering_rect.width/2,hovering_rect.height/2);
                    hovering_preview.setTextureRect(hovering_rect);

                    //4 possible angles
                    double angle_1 = parent->sprite.getRotation();
                    double angle_2 = parent->sprite.getRotation() + 90;
                    double angle_3 = parent->sprite.getRotation() - 90;
                    double angle_4 = parent->sprite.getRotation() + 180;

                    if(angle_2 > 360){ angle_2 -= 360; }
                    if(angle_3 < 0){ angle_3 += 360; }
                    if(angle_4 > 360){ angle_4 -= 360; }

                    double hovering_rotation = getSector(sector_id)->getEnt(player_id)->sprite.getRotation();

                    double shortest_rot = angle_1 - hovering_rotation;
                    bool switch_height_and_width = false;
                    if(abs(angle_2 - hovering_rotation) < abs(shortest_rot)){ shortest_rot = angle_2 - hovering_rotation; switch_height_and_width = true;}
                    if(abs(angle_3 - hovering_rotation) < abs(shortest_rot)){ shortest_rot = angle_3 - hovering_rotation; switch_height_and_width = true;}
                    if(abs(angle_4 - hovering_rotation) < abs(shortest_rot)){ shortest_rot = angle_4 - hovering_rotation; switch_height_and_width = false;}
                    hovering_rotation += shortest_rot;

                    if(switch_height_and_width){
                        if(closest_side == "bottom"){
                        mount_position.y = (parent_bounds.height/2) + (hovering_rect.width/2) + 0.2;
                        }
                        if(closest_side == "top"){
                            mount_position.y = ((-parent_bounds.height/2) - (hovering_rect.width/2)) - 0.2;
                        }
                        if(closest_side == "right"){
                            mount_position.x = (parent_bounds.width/2) + (hovering_rect.height/2) + 0.2;
                        }
                        if(closest_side == "left"){
                            mount_position.x = ((-parent_bounds.width/2) - (hovering_rect.height/2)) - 0.2;
                        }
                    }
                    else{
                        if(closest_side == "bottom"){
                        mount_position.y = (parent_bounds.height/2) + (hovering_rect.height/2) + 0.2;
                        }
                        if(closest_side == "top"){
                            mount_position.y = ((-parent_bounds.height/2) - (hovering_rect.height/2)) - 0.2;
                        }
                        if(closest_side == "right"){
                            mount_position.x = (parent_bounds.width/2) + (hovering_rect.width/2) + 0.2;
                        }
                        if(closest_side == "left"){
                            mount_position.x = ((-parent_bounds.width/2) - (hovering_rect.width/2)) - 0.2;
                        }
                    }


                    hovering_preview.setRotation(hovering_rotation);
                    Q = parent->sprite.getRotation() * (M_PI/180);
                    position =  parent->sprite.getPosition() + Vector2f(mount_position.x*cos(Q)-mount_position.y*sin(Q),mount_position.x*sin(Q)+mount_position.y*cos(Q)) ;
                    hovering_preview.setPosition(position);

                    set<string> possible_collisions;
                    possible_collisions = getSector(sector_id)->collision_grid.getNearby(hovering_preview.getPosition(), hypot(hovering_rect.width/2, hovering_rect.height/2));

                    bool valid_placement = true;
                    for(set<string>::iterator parser = possible_collisions.begin(); parser != possible_collisions.end(); parser++){
                        if(spritesIntersecting(hovering_preview, getSector(sector_id)->getEnt(*parser)->sprite)){
                            valid_placement = false;
                            break;
                        }
                    }

                    if(valid_placement){
                        hovering_preview.setColor(gui_color_a + Color(0,0,0,200));
                        gui_captions.push_back(Caption("[LMB] TO CONSTRUCT.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;

                        if(input.lmb_released){
                            string new_ent_id;

                            if(getSector(sector_id)->getClassification(gui_hovering_item_id) == "ENTITY" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "CONTAINER"){
                                new_ent_id = makeNewEntity(sector_id, createUniqueId(), hovering_preview.getPosition().x, hovering_preview.getPosition().y, hovering_rotation, getSector(sector_id)->items[gui_hovering_item_id]->properties->name);
                            }
                            else if(getSector(sector_id)->getClassification(gui_hovering_item_id) == "MACHINE" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "CONTROLBOARD" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "TERMINAL"){
                                new_ent_id = makeNewMachine(sector_id, createUniqueId(), hovering_preview.getPosition().x, hovering_preview.getPosition().y, hovering_rotation, getSector(sector_id)->items[gui_hovering_item_id]->properties->name);
                            }

                            getSector(sector_id)->getEnt(new_ent_id)->setVelocity(parent->getVelocity().x, parent->getVelocity().y);
                            getSector(sector_id)->trash(gui_hovering_item_id);
                            gui_hovering_item_id = "";
                        }
                    }
                    else{
                        hovering_preview.setColor(Color(255,55,55,200));
                        gui_captions.push_back(Caption("CANNOT CONSTRUCT - NOT ENOUGH SPACE.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                        alert_index++;

                    }
                    gui_sprites.push_back(hovering_preview);

                }
                else if(!floor_tile_1_placed && selected_classification == "FLOOR" && gui_hovering_item_id != "" && getSector(sector_id)->getClassification(gui_hovering_item_id) == "FLOOR"){

                    shared_ptr<Entity> parent = getSector(sector_id)->getEnt(selected_object_id);
                    double Q = (360 - parent->sprite.getRotation()) * (M_PI/180);
                    Vector2f position = input.mouse - parent->sprite.getPosition();
                    Vector2f mount_position =  Vector2f(position.x*cos(Q)-position.y*sin(Q),position.x*sin(Q)+position.y*cos(Q)) ;

                    FloatRect parent_bounds = parent->sprite.getLocalBounds();
                    map<string, double> displacement;
                    displacement["top"] = mount_position.y - (-parent_bounds.height/2);
                    displacement["bottom"] = mount_position.y - (parent_bounds.height/2);
                    displacement["right"] = mount_position.x - (parent_bounds.width/2);
                    displacement["left"] = mount_position.x - (-parent_bounds.width/2);

                    double closest_disp = -1;
                    string closest_side = "";
                    for(map<string, double>::iterator parser = displacement.begin(); parser != displacement.end(); parser++){

                        if(closest_disp < 0 || abs(parser->second) < closest_disp){
                            closest_disp = abs(parser->second);
                            closest_side = parser->first;
                        }
                    }

                    Sprite hovering_preview;
                    IntRect hovering_rect(0,0,getSector(sector_id)->items[gui_hovering_item_id]->properties->get("width"), getSector(sector_id)->items[gui_hovering_item_id]->properties->get("height"));
                    hovering_preview.setTexture(getSector(sector_id)->items[gui_hovering_item_id]->properties->sheet, false);
                    hovering_preview.setOrigin(hovering_rect.width/2,hovering_rect.height/2);
                    hovering_preview.setTextureRect(hovering_rect);

                    //4 possible angles
                    double angle_1 = parent->sprite.getRotation();
                    double angle_2 = parent->sprite.getRotation() + 90;
                    double angle_3 = parent->sprite.getRotation() - 90;
                    double angle_4 = parent->sprite.getRotation() + 180;

                    if(angle_2 > 360){ angle_2 -= 360; }
                    if(angle_3 < 0){ angle_3 += 360; }
                    if(angle_4 > 360){ angle_4 -= 360; }

                    double hovering_rotation = getSector(sector_id)->getEnt(player_id)->sprite.getRotation();

                    double shortest_rot = angle_1 - hovering_rotation;
                    bool switch_height_and_width = false;
                    if(abs(angle_2 - hovering_rotation) < abs(shortest_rot)){ shortest_rot = angle_2 - hovering_rotation; switch_height_and_width = true;}
                    if(abs(angle_3 - hovering_rotation) < abs(shortest_rot)){ shortest_rot = angle_3 - hovering_rotation; switch_height_and_width = true;}
                    if(abs(angle_4 - hovering_rotation) < abs(shortest_rot)){ shortest_rot = angle_4 - hovering_rotation; switch_height_and_width = false;}
                    hovering_rotation += shortest_rot;

                    if(switch_height_and_width){
                        if(closest_side == "bottom"){
                        mount_position.y = (parent_bounds.height/2) + (hovering_rect.width/2) + 0.2;
                        }
                        if(closest_side == "top"){
                            mount_position.y = ((-parent_bounds.height/2) - (hovering_rect.width/2)) - 0.2;
                        }
                        if(closest_side == "right"){
                            mount_position.x = (parent_bounds.width/2) + (hovering_rect.height/2) + 0.2;
                        }
                        if(closest_side == "left"){
                            mount_position.x = ((-parent_bounds.width/2) - (hovering_rect.height/2)) - 0.2;
                        }
                    }
                    else{
                        if(closest_side == "bottom"){
                        mount_position.y = (parent_bounds.height/2) + (hovering_rect.height/2) + 0.2;
                        }
                        if(closest_side == "top"){
                            mount_position.y = ((-parent_bounds.height/2) - (hovering_rect.height/2)) - 0.2;
                        }
                        if(closest_side == "right"){
                            mount_position.x = (parent_bounds.width/2) + (hovering_rect.width/2) + 0.2;
                        }
                        if(closest_side == "left"){
                            mount_position.x = ((-parent_bounds.width/2) - (hovering_rect.width/2)) - 0.2;
                        }
                    }


                    hovering_preview.setRotation(hovering_rotation);
                    Q = parent->sprite.getRotation() * (M_PI/180);
                    position =  parent->sprite.getPosition() + Vector2f(mount_position.x*cos(Q)-mount_position.y*sin(Q),mount_position.x*sin(Q)+mount_position.y*cos(Q)) ;
                    hovering_preview.setPosition(position);

                    set<string> possible_collisions;
                    possible_collisions = getSector(sector_id)->floor_grid.getNearby(hovering_preview.getPosition(), hypot(hovering_rect.width/2, hovering_rect.height/2));

                    bool valid_placement = true;
                    for(set<string>::iterator parser = possible_collisions.begin(); parser != possible_collisions.end(); parser++){
                        if(spritesIntersecting(hovering_preview, getSector(sector_id)->getEnt(*parser)->sprite)){
                            valid_placement = false;
                            break;
                        }
                    }

                    if(valid_placement){
                        hovering_preview.setColor(gui_color_a + Color(0,0,0,200));
                        gui_captions.push_back(Caption("[LMB] TO CONSTRUCT.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;

                        if(input.lmb_released){
                            floor_tile_1_placed = true;
                            floor_tile_1_coords = hovering_preview.getPosition();
                            floor_tile_1_rotation = hovering_rotation;
                        }
                    }
                    else{
                        hovering_preview.setColor(Color(255,55,55,200));
                        gui_captions.push_back(Caption("CANNOT CONSTRUCT - NOT ENOUGH SPACE.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                        alert_index++;

                    }
                    gui_sprites.push_back(hovering_preview);
                }
                else if(gui_hovering_item_id != "" && getSector(sector_id)->getClassification(gui_hovering_item_id) == "INTERFACE"){

                    if(selected_port_id != "" && (selected_classification == "MACHINE" || selected_classification == "TERMINAL" || selected_classification == "CONTROLBOARD" || selected_classification == "WARP DRIVE")){
                        if(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->properties->get("port_type") == 0){
                           gui_captions.push_back(Caption("+"+asString(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->voltage)+"V", "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 34, Color(255,255,255,205), "left"));
                        }
                        else if(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->properties->get("port_type") == 2){
                            gui_captions.push_back(Caption(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->getCell()->id, "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 34, Color(255,255,255,205), "left"));
                        }

                    }

                    if(selected_port_id != "" && (selected_classification == "MACHINE" || selected_classification == "TERMINAL" || selected_classification == "CONTROLBOARD" || selected_classification == "WARP DRIVE") && getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->connection == "" && getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->properties->get("port_type") == getSector(sector_id)->items[gui_hovering_item_id]->properties->get("port_type")){
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->sprite, gui_color_a + Color(0,0,0,200), 12));
                        gui_captions.push_back(Caption("[LMB] TO INSTALL INTERFACE.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;

                        if(input.lmb_released){

                            makeNewInterface(sector_id, createUniqueId(), selected_port_id, selected_object_id, Vector2f(0,0),getSector(sector_id)->items[gui_hovering_item_id]->properties->name);
                            getSector(sector_id)->trash(gui_hovering_item_id);
                            gui_hovering_item_id = "";
                        }

                    }
                    else if(selected_port_id != "" && (selected_classification == "MACHINE" || selected_classification == "TERMINAL" || selected_classification == "CONTROLBOARD" || selected_classification == "WARP DRIVE") && getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->connection == ""){
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->sprite, Color(255,55,55,225), 12));
                        gui_captions.push_back(Caption("CANNOT INSTALL INTERFACE - INCORRECT PORT TYPE", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                        alert_index++;

                    }
                    else{
                        gui_captions.push_back(Caption("VACANT PORT REQUIRED TO INSTALL INTERFACE.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                        alert_index++;
                    }

                }
                else if(gui_hovering_item_id != "" && getSector(sector_id)->getClassification(gui_hovering_item_id) == "TRANSFER"){

                    if(gui_machine_id_a != "" && gui_port_id_a != ""){
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[gui_machine_id_a].lock()->ports[gui_port_id_a]->sprite, Color(255,55,55,200), 12));
                    }

                    if(selected_port_id != "" && (selected_classification == "MACHINE" || selected_classification == "TERMINAL" || selected_classification == "CONTROLBOARD" || selected_classification == "WARP DRIVE")){
                        if(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->properties->get("port_type") == 0){
                           gui_captions.push_back(Caption("+"+asString(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->voltage)+"V", "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 34, Color(255,255,255,205), "left"));
                        }
                        else if(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->properties->get("port_type") == 2){
                            gui_captions.push_back(Caption(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->getCell()->id, "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 34, Color(255,255,255,205), "left"));
                        }

                    }
                    if(selected_port_id != "" && (selected_object_id != gui_machine_id_a || selected_port_id != gui_port_id_a) && (selected_classification == "MACHINE" || selected_classification == "TERMINAL" || selected_classification == "CONTROLBOARD" || selected_classification == "WARP DRIVE") && getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->connection == "" && getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->properties->get("port_type") == 2 && getSector(sector_id)->items[gui_hovering_item_id]->properties->get("port_type") == 2){
                        if(gui_machine_id_a == "" && gui_port_id_a == ""){
                            gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->sprite, gui_color_a + Color(0,0,0,200), 12));
                            gui_captions.push_back(Caption("[LMB] TO CONNECT CONDUIT.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                            alert_index++;

                            if(input.lmb_released){
                                gui_machine_id_a = selected_object_id;
                                gui_port_id_a = selected_port_id;
                            }

                        }
                        else{
                            bool compatible_port = getSector(sector_id)->machines[gui_machine_id_a].lock()->ports[gui_port_id_a]->is_input != getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->is_input;
                            if(compatible_port){
                                gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->sprite, gui_color_a + Color(0,0,0,200), 12));
                                gui_captions.push_back(Caption("[LMB] TO CONNECT CONDUIT.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                                alert_index++;
                                gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[gui_machine_id_a].lock()->ports[gui_port_id_a]->sprite, gui_color_a + Color(0,0,0,200), 12));

                                if(input.lmb_released){
                                    makeNewTransfer(sector_id, createUniqueId(), gui_port_id_a, selected_port_id, gui_machine_id_a, selected_object_id, getSector(sector_id)->items[gui_hovering_item_id]->properties->name);
                                    gui_machine_id_a = "";
                                    gui_port_id_a = "";
                                    getSector(sector_id)->trash(gui_hovering_item_id);
                                    gui_hovering_item_id = "";
                                    floor_tile_1_placed = false;
                                    floor_tile_1_coords = Vector2f(0,0);
                                    floor_tile_1_rotation = 0;
                                }
                            }
                            else{
                                gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->sprite, Color(255,55,55,225), 12));
                                gui_captions.push_back(Caption("CANNOT CONNECT CONDUIT - INPUTS MUST CONNECT TO OUTPUTS", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                                alert_index++;

                                gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[gui_machine_id_a].lock()->ports[gui_port_id_a]->sprite, Color(255,55,55,200), 12));
                            }
                        }
                    }
                    else if(selected_port_id != "" && (selected_object_id != gui_machine_id_a || selected_port_id != gui_port_id_a) && (selected_classification == "MACHINE" || selected_classification == "TERMINAL" || selected_classification == "CONTROLBOARD" || selected_classification == "WARP DRIVE") && getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->connection == "" && getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->properties->get("port_type") == getSector(sector_id)->items[gui_hovering_item_id]->properties->get("port_type")){
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->sprite, gui_color_a + Color(0,0,0,200), 12));
                        gui_captions.push_back(Caption("[LMB] TO CONNECT CONDUIT.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;

                        if(gui_machine_id_a != "" && gui_port_id_a != ""){
                            gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[gui_machine_id_a].lock()->ports[gui_port_id_a]->sprite, gui_color_a + Color(0,0,0,200), 12));
                        }

                        if(input.lmb_released){

                            if(gui_machine_id_a == ""){
                                gui_machine_id_a = selected_object_id;
                                gui_port_id_a = selected_port_id;
                            }
                            else{
                                makeNewTransfer(sector_id, createUniqueId(), gui_port_id_a, selected_port_id, gui_machine_id_a, selected_object_id, getSector(sector_id)->items[gui_hovering_item_id]->properties->name);
                                gui_machine_id_a = "";
                                gui_port_id_a = "";
                                getSector(sector_id)->trash(gui_hovering_item_id);
                                gui_hovering_item_id = "";
                                floor_tile_1_placed = false;
                                floor_tile_1_coords = Vector2f(0,0);
                                floor_tile_1_rotation = 0;
                            }
                        }

                    }
                    else if((selected_classification == "MACHINE" || selected_classification == "TERMINAL" || selected_classification == "CONTROLBOARD" || selected_classification == "WARP DRIVE") && (selected_object_id != gui_machine_id_a || selected_port_id != gui_port_id_a) && selected_port_id != "" && getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->connection == ""){
                        gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[selected_object_id].lock()->ports[selected_port_id]->sprite, Color(255,55,55,225), 12));
                        gui_captions.push_back(Caption("CANNOT CONNECT CONDUIT - INCORRECT PORT TYPE", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                        alert_index++;

                        if(gui_machine_id_a != "" && gui_port_id_a != ""){
                            gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[gui_machine_id_a].lock()->ports[gui_port_id_a]->sprite, Color(255,55,55,200), 12));
                        }
                    }
                    else{
                        if(gui_machine_id_a == ""){
                            gui_captions.push_back(Caption("VACANT PORT REQUIRED TO CONNECT CONDUIT", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                            alert_index++;
                        }
                        else{
                            gui_captions.push_back(Caption("SECOND VACANT PORT REQUIRED TO COMPLETE CONDUIT", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                            alert_index++;

                            gui_circles.push_back(createBoundingCircle(getSector(sector_id)->machines[gui_machine_id_a].lock()->ports[gui_port_id_a]->sprite, Color(255,55,55,200), 12));

                        }
                    }
                }
                else if(gui_hovering_item_id != "" && getSector(sector_id)->getClassification(gui_hovering_item_id) == "MOUNTER"){

                    Sprite hovering_preview;
                    hovering_preview.setTexture(getSector(sector_id)->items[gui_hovering_item_id]->properties->sheet, true);
                    IntRect preview_bounds = hovering_preview.getTextureRect();
                    hovering_preview.setOrigin(preview_bounds.width/2,preview_bounds.height/2);
                    hovering_preview.setPosition(input.mouse);
                    hovering_preview.setRotation(getSector(sector_id)->getEnt(player_id)->sprite.getRotation());

                    if(selected_classification == "ENTITY" || selected_classification == "CONTAINER" || selected_classification == "MACHINE" || selected_classification == "TERMINAL" || selected_classification == "CONTROLBOARD" || selected_classification == "WARP DRIVE"){

                        shared_ptr<Entity> parent = getSector(sector_id)->getEnt(selected_object_id);
                        double Q = (360 - parent->sprite.getRotation()) * (M_PI/180);
                        Vector2f position = input.mouse - parent->sprite.getPosition();
                        Vector2f mount_position =  Vector2f(position.x*cos(Q)-position.y*sin(Q),position.x*sin(Q)+position.y*cos(Q)) ;

                        FloatRect parent_bounds = parent->sprite.getLocalBounds();
                        map<string, double> displacement;
                        displacement["top"] = mount_position.y - (-parent_bounds.height/2);
                        displacement["bottom"] = mount_position.y - (parent_bounds.height/2);
                        displacement["right"] = mount_position.x - (parent_bounds.width/2);
                        displacement["left"] = mount_position.x - (-parent_bounds.width/2);

                        double closest_disp = -1;
                        string closest_side = "";
                        for(map<string, double>::iterator parser = displacement.begin(); parser != displacement.end(); parser++){

                            if(closest_disp < 0 || abs(parser->second) < closest_disp){
                                closest_disp = abs(parser->second);
                                closest_side = parser->first;
                            }
                        }

                        if(closest_side == "bottom"){
                            hovering_preview.setRotation(parent->sprite.getRotation());
                            mount_position.y = (parent_bounds.height/2) + (preview_bounds.height/2);
                        }
                        if(closest_side == "top"){
                            hovering_preview.setRotation(parent->sprite.getRotation()+180);
                            mount_position.y = (-parent_bounds.height/2) - (preview_bounds.height/2);
                        }
                        if(closest_side == "right"){
                            hovering_preview.setRotation(parent->sprite.getRotation()-90);
                            mount_position.x = (parent_bounds.width/2) + (preview_bounds.height/2);
                        }
                        if(closest_side == "left"){
                            hovering_preview.setRotation(parent->sprite.getRotation()+90);
                            mount_position.x = (-parent_bounds.width/2) - (preview_bounds.height/2);
                        }

                        Q = parent->sprite.getRotation() * (M_PI/180);
                        position =  parent->sprite.getPosition() + Vector2f(mount_position.x*cos(Q)-mount_position.y*sin(Q),mount_position.x*sin(Q)+mount_position.y*cos(Q)) ;
                        hovering_preview.setPosition(position);

                        set<string> nearby_floors = getSector(sector_id)->floor_grid.getNearby(hovering_preview.getPosition(), hypot(preview_bounds.width, preview_bounds.height)/2);
                        string floor_underneath = "";
                        for(set<string>::iterator parser = nearby_floors.begin(); parser != nearby_floors.end(); parser++){
                            if(spritesIntersecting(getSector(sector_id)->getEnt(*parser)->sprite, hovering_preview)){

                                floor_underneath = *parser;
                                break;
                            }
                        }

                        if(floor_underneath != ""){
                            hovering_preview.setColor(gui_color_a + Color(0,0,0,200));
                            gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(floor_underneath)->sprite, gui_color_a + Color(0,0,0,155)));
                            gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(selected_object_id)->sprite, gui_color_a + Color(0,0,0,155)));
                            gui_captions.push_back(Caption("[LMB] TO MOUNT OBJECT TO FLOOR.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                            alert_index++;

                            if(input.lmb_released){
                                string new_id = makeNewConnector(sector_id, createUniqueId(), getSector(sector_id)->items[gui_hovering_item_id]->properties->name, selected_object_id, floor_underneath);
                                getSector(sector_id)->connectors[new_id]->setSprite(hovering_preview);
                                getSector(sector_id)->trash(gui_hovering_item_id);
                                gui_hovering_item_id = "";

                            }
                        }
                        else{
                            hovering_preview.setColor(Color(255,55,55,200));
                            gui_rectangles.push_back(createBoundingRectangle(getSector(sector_id)->getEnt(selected_object_id)->sprite, Color(255,55,55,155)));
                            gui_captions.push_back(Caption("FLOOR REQUIRED TO MOUNT OBJECT.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                            alert_index++;
                        }

                    }
                    else{
                        hovering_preview.setColor(Color(255,55,55,200));
                        gui_captions.push_back(Caption("OBJECT REQUIRED TO ATTACH MOUNT.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                        alert_index++;
                    }
                    gui_sprites.push_back(hovering_preview);
                }
                else if(gui_hovering_item_id != "" && getSector(sector_id)->getClassification(gui_hovering_item_id) == "CONNECTOR"){

                    Sprite hovering_preview;
                    hovering_preview.setTexture(getSector(sector_id)->items[gui_hovering_item_id]->properties->sheet, true);
                    IntRect hovering_rect = hovering_preview.getTextureRect();
                    hovering_preview.setOrigin(hovering_rect.width/2,hovering_rect.height/2);
                    hovering_preview.setPosition(input.mouse);
                    hovering_preview.setRotation(getSector(sector_id)->getEnt(player_id)->sprite.getRotation());

                    double connector_radius = hypot(hovering_rect.width/2,hovering_rect.height/2);

                    string id_a = "";
                    string id_b = "";

                    set<string> possible = getSector(sector_id)->collision_grid.getNearby(input.mouse, connector_radius);
                    for(set<string>::iterator parser = possible.begin(); parser != possible.end(); parser++){
                        shared_ptr<Entity> ob = getSector(sector_id)->getEnt(*parser);
                        Vector2f disp = input.mouse - ob->getPosition();
                        if(spritesIntersecting(ob->sprite, hovering_preview)){

                            if(id_a == ""){
                                id_a = *parser;
                                continue;
                            }
                            else{
                                id_b = *parser;
                                break;
                            }
                        }

                    }

                    if(id_a != "" && id_b != ""){
                        shared_ptr<Entity> ob_a = getSector(sector_id)->getEnt(id_a);
                        shared_ptr<Entity> ob_b = getSector(sector_id)->getEnt(id_b);
                        gui_rectangles.push_back(createBoundingRectangle(ob_a->sprite, gui_color_a + Color(0,0,0,155)));
                        gui_rectangles.push_back(createBoundingRectangle(ob_b->sprite, gui_color_a + Color(0,0,0,155)));
                        hovering_preview.setColor(gui_color_a + Color(0,0,0,155));
                        gui_captions.push_back(Caption("[LMB] TO BOND OBJECTS TOGETHER.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;

                        if(input.lmb_released){
                            string new_id = makeNewConnector(sector_id, createUniqueId(), getSector(sector_id)->items[gui_hovering_item_id]->properties->name, id_a, id_b);
                            createEffect(sector_id, input.mouse, Vector2f(0,0), "Smoke");
                            getSector(sector_id)->connectors[new_id]->setSprite(hovering_preview);
                            getSector(sector_id)->trash(gui_hovering_item_id);
                            gui_hovering_item_id = "";
                        }
                    }
                    else if(id_a != ""){
                        shared_ptr<Entity> ob = getSector(sector_id)->getEnt(id_a);
                        gui_rectangles.push_back(createBoundingRectangle(ob->sprite, Color(255,55,55,155)));
                        hovering_preview.setColor(Color(255,55,55,155));
                        gui_captions.push_back(Caption("TWO OBJECTS REQUIRED TO PLACE A BOND.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                        alert_index++;
                    }
                    else{
                        hovering_preview.setColor(Color(255,55,55,155));
                        gui_captions.push_back(Caption("TWO OBJECTS REQUIRED TO PLACE A BOND.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                        alert_index++;
                    }
                    gui_sprites.push_back(hovering_preview);

                }
                else if(gui_hovering_item_id != "" && (getSector(sector_id)->getClassification(gui_hovering_item_id) == "ENTITY"|| getSector(sector_id)->getClassification(gui_hovering_item_id) == "CONTAINER" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "TERMINAL" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "CONTROLBOARD" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "MACHINE")){

                    //add a hovering preview to sprites
                    Sprite hovering_preview;
                    IntRect hovering_rect(0,0,getSector(sector_id)->items[gui_hovering_item_id]->properties->get("width"), getSector(sector_id)->items[gui_hovering_item_id]->properties->get("height"));

                    hovering_preview.setTexture(getSector(sector_id)->items[gui_hovering_item_id]->properties->sheet, false);
                    hovering_preview.setTextureRect(hovering_rect);
                    hovering_preview.setOrigin(hovering_rect.width/2,hovering_rect.height/2);
                    hovering_preview.setPosition(input.mouse);
                    hovering_preview.setRotation(getSector(sector_id)->getEnt(player_id)->sprite.getRotation());

                    set<string> possible_collisions;
                    if(getSector(sector_id)->getClassification(gui_hovering_item_id) == "ENTITY" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "CONTAINER" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "MACHINE" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "TERMINAL" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "CONTROLBOARD"){
                        possible_collisions = getSector(sector_id)->collision_grid.getNearby(input.mouse, hypot(hovering_rect.width/2, hovering_rect.height/2));
                    }


                    bool valid_placement = true;
                    for(set<string>::iterator parser = possible_collisions.begin(); parser != possible_collisions.end(); parser++){
                        if(spritesIntersecting(hovering_preview, getSector(sector_id)->getEnt(*parser)->sprite)){
                            valid_placement = false;
                            break;
                        }
                    }

                    if(valid_placement){
                        hovering_preview.setColor(gui_color_a + Color(0,0,0,200));
                        gui_captions.push_back(Caption("[LMB] TO CONSTRUCT.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                        alert_index++;
                        if(input.lmb_released){
                            if(getSector(sector_id)->getClassification(gui_hovering_item_id) == "ENTITY" || getSector(sector_id)->getClassification(gui_hovering_item_id) ==  "CONTAINER"){
                                string new_ent_id = makeNewEntity(sector_id, createUniqueId(), input.mouse.x, input.mouse.y, getSector(sector_id)->getEnt(player_id)->sprite.getRotation(),getSector(sector_id)->items[gui_hovering_item_id]->properties->name);
                            }
                            else if(getSector(sector_id)->getClassification(gui_hovering_item_id) == "MACHINE" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "TERMINAL" || getSector(sector_id)->getClassification(gui_hovering_item_id) == "CONTROLBOARD"){
                                string new_ent_id = makeNewMachine(sector_id, createUniqueId(), input.mouse.x, input.mouse.y, getSector(sector_id)->getEnt(player_id)->sprite.getRotation(), getSector(sector_id)->items[gui_hovering_item_id]->properties->name);
                            }
                            getSector(sector_id)->trash(gui_hovering_item_id);
                            gui_hovering_item_id = "";
                        }

                    }
                    else{
                        hovering_preview.setColor(Color(255,55,55,200));
                        gui_captions.push_back(Caption("CANNOT CONSTRUCT - NOT ENOUGH SPACE.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                        alert_index++;
                    }

                    if(gui_hovering_item_id != ""){
                        gui_sprites.push_back(hovering_preview);
                    }
                }
                else if(gui_hovering_item_id != "" && getSector(sector_id)->getClassification(gui_hovering_item_id) == "FLOOR"){

                    if(!floor_tile_1_placed){
                        //add a hovering preview to sprites
                        Sprite hovering_preview;
                        IntRect hovering_rect(0,0,getSector(sector_id)->items[gui_hovering_item_id]->properties->get("width"), getSector(sector_id)->items[gui_hovering_item_id]->properties->get("height"));

                        hovering_preview.setTexture(getSector(sector_id)->items[gui_hovering_item_id]->properties->sheet, false);
                        hovering_preview.setTextureRect(hovering_rect);
                        hovering_preview.setOrigin(hovering_rect.width/2,hovering_rect.height/2);
                        hovering_preview.setPosition(input.mouse);
                        hovering_preview.setRotation(getSector(sector_id)->getEnt(player_id)->sprite.getRotation());

                        set<string> possible_collisions;
                        possible_collisions = getSector(sector_id)->floor_grid.getNearby(input.mouse, hypot(hovering_rect.width/2, hovering_rect.height/2));

                        bool valid_placement = true;
                        for(set<string>::iterator parser = possible_collisions.begin(); parser != possible_collisions.end(); parser++){
                            if(spritesIntersecting(hovering_preview, getSector(sector_id)->getEnt(*parser)->sprite)){
                                valid_placement = false;
                                break;
                            }
                        }

                        if(valid_placement){
                            hovering_preview.setColor(gui_color_a + Color(0,0,0,200));
                            gui_captions.push_back(Caption("[LMB] TO CONSTRUCT.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                            alert_index++;
                            if(input.lmb_released){
                                floor_tile_1_placed = true;
                                floor_tile_1_coords = hovering_preview.getPosition();
                                floor_tile_1_rotation = getSector(sector_id)->getEnt(player_id)->sprite.getRotation();
                            }

                        }
                        else{
                            hovering_preview.setColor(Color(255,55,55,200));
                            gui_captions.push_back(Caption("CANNOT CONSTRUCT - NOT ENOUGH SPACE.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                            alert_index++;
                        }

                        if(gui_hovering_item_id != ""){
                            gui_sprites.push_back(hovering_preview);
                        }
                    }
                    else{

                         Sprite tile_template;
                         IntRect tile_rect(0,0,getSector(sector_id)->items[gui_hovering_item_id]->properties->get("width"), getSector(sector_id)->items[gui_hovering_item_id]->properties->get("height"));
                         tile_template.setTexture(getSector(sector_id)->items[gui_hovering_item_id]->properties->sheet, false);
                         tile_template.setTextureRect(tile_rect);
                         tile_template.setOrigin(tile_rect.width/2,tile_rect.height/2);
                         tile_template.setRotation(floor_tile_1_rotation);

                         Vector2f parallel_axis = Vector2f(sin((floor_tile_1_rotation)*(M_PI/180)),-cos((floor_tile_1_rotation)*(M_PI/180)));
                         Vector2f perpendicular_axis = Vector2f(sin((floor_tile_1_rotation + 90)*(M_PI/180)),-cos((floor_tile_1_rotation + 90)*(M_PI/180)));

                         Vector2f diagonal = input.mouse - floor_tile_1_coords;

                         double parallel_component = (diagonal.x*parallel_axis.x) + (diagonal.y*parallel_axis.y);
                         double perpendicular_component = (diagonal.x*perpendicular_axis.x) + (diagonal.y*perpendicular_axis.y);

                         int parallel_tiles = 1 + floor(abs(parallel_component/tile_rect.height));
                         int perpendicular_tiles = 1 + floor(abs(perpendicular_component/tile_rect.width));

                         vector<Sprite> hovering_preview;
                         bool valid_placement = true;

                         for(int i = 0; i < parallel_tiles; i++){
                            Vector2f parallel_position;
                            if(abs(parallel_component)>=1){
                                parallel_position = floor_tile_1_coords + (parallel_axis*tile_rect.height*i*(parallel_component/abs(parallel_component)));
                            }
                            else{
                                parallel_position = floor_tile_1_coords;
                            }
                            for(int j = 0; j < perpendicular_tiles; j++){
                                Vector2f final_position ;
                                if(abs(perpendicular_component)>=1){
                                    final_position = parallel_position + (perpendicular_axis*tile_rect.width*j*(perpendicular_component/abs(perpendicular_component)));
                                }
                                else{
                                    final_position = parallel_position;
                                }
                                tile_template.setPosition(final_position);

                                if(valid_placement){
                                    set<string> possible_collisions;
                                    possible_collisions = getSector(sector_id)->floor_grid.getNearby(final_position, hypot(tile_rect.width/2, tile_rect.height/2));


                                    for(set<string>::iterator parser = possible_collisions.begin(); parser != possible_collisions.end(); parser++){
                                        if(spritesIntersecting(tile_template, getSector(sector_id)->getEnt(*parser)->sprite)){
                                            valid_placement = false;
                                        }
                                    }
                                }

                                hovering_preview.push_back(tile_template);
                            }
                         }

                         Vector2f center = (floor_tile_1_coords + hovering_preview[hovering_preview.size()-1].getPosition())/2.0;

                         if(valid_placement){
                             for(vector<Sprite>::iterator parser = hovering_preview.begin(); parser != hovering_preview.end(); parser++){
                                 parser->setColor(gui_color_a + Color(0,0,0,200));
                             }
                             gui_captions.push_back(Caption("[LMB] TO CONSTRUCT.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255), "left"));
                             alert_index++;
                             if(input.lmb_released){

                                makeNewFloor(sector_id, createUniqueId(), center.x, center.y, perpendicular_tiles, parallel_tiles,floor_tile_1_rotation,getSector(sector_id)->items[gui_hovering_item_id]->properties->name);
                                getSector(sector_id)->trash(gui_hovering_item_id);
                                gui_hovering_item_id = "";

                                floor_tile_1_placed = false;
                                floor_tile_1_coords = Vector2f(0,0);
                                floor_tile_1_rotation = 0;
                             }
                         }
                         else{
                             for(vector<Sprite>::iterator parser = hovering_preview.begin(); parser != hovering_preview.end(); parser++){
                                 parser->setColor(Color(255,55,55,200));
                             }
                            gui_captions.push_back(Caption("CANNOT CONSTRUCT - NOT ENOUGH SPACE.", "font1", Vector2f(10, 15 + (alert_index*12)), 26, Color(255,55,55,255), "left"));
                            alert_index++;
                         }
                         //determine validity and colour and allow option to place.

                         for(vector<Sprite>::iterator parser = hovering_preview.begin(); parser != hovering_preview.end(); parser++){
                            gui_sprites.push_back(*parser);
                         }

                    }
                }
            }

            if(input.keys_released.count("f") != 0){ player->flashlight_on = !player->flashlight_on; }

            if(input.keys_held.count("lshift") != 0){ player->jetpack_on = true; }
            else{ player->jetpack_on = false; }
            if(input.keys_held.count("a") != 0){ player->walk(Left,dt); }
            if(input.keys_held.count("w") != 0){ player->walk(Forward,dt); }
            if(input.keys_held.count("s") != 0){ player->walk(Backward,dt); }
            if(input.keys_held.count("d") != 0){ player->walk(Right,dt); }
            if(input.keys_held.count("q") != 0){ player->turn(Counterclockwise,dt); }
            if(input.keys_held.count("e") != 0){ player->turn(Clockwise,dt); }
        }

        if(grabbing){
            gui_captions.push_back(Caption("[RMB] TO LET GO", "font1", Vector2f(10, 15 + (alert_index*12)), 26, gui_color_a + Color(0,0,0,255),"left"));
            alert_index++;
            if(input.rmb_released != 0){
                player->grab("");
            }
        }

        inventory_bounds = FloatRect(0,0,0,0);
        if(inventory_open){
            if(!free_build){
                for(int i = 1; i <= player->contents.size(); i++){
                    RectangleShape slot;
                    slot.setSize(Vector2f(80,80));
                    slot.setOrigin(40, 40);
                    slot.setFillColor(gui_color_b - Color(40,40,40,0) + Color(0,0,0,15));
                    slot.setOutlineColor(gui_color_b + Color(0,0,0,155));
                    slot.setOutlineThickness(2);
                    slot.setPosition(50 + (((i-1)%3)*90), 850 - (floor((i-1)/3)*90));
                    if(slot.getGlobalBounds().contains(input.fixed_mouse)){
                        slot.setScale(1.1,1.1);
                    }

                    if(slot.getGlobalBounds().left + slot.getGlobalBounds().width > inventory_bounds.width){ inventory_bounds.width = slot.getGlobalBounds().left + slot.getGlobalBounds().width; }
                    if(900-slot.getGlobalBounds().top > inventory_bounds.height){ inventory_bounds.height = 900-slot.getGlobalBounds().top; }

                    gui_fixed_rectangles.push_back(slot);
                    gui_slot_boxes.push_back(slot.getGlobalBounds());
                    gui_slot_contents.push_back(&player->contents[i]);

                    if(player->contents[i] != ""){
                        Sprite icon;
                        icon.setTexture(getSector(sector_id)->items[player->contents[i]]->properties->icon, true);
                        IntRect icon_size = icon.getTextureRect();
                        icon.setOrigin(icon_size.width/2, icon_size.height/2);
                        icon.setColor(Color(255,255,255,225));
                        icon.setPosition(slot.getPosition());
                        if(slot.getGlobalBounds().contains(input.fixed_mouse)){
                            icon.setScale(1.1,1.1);
                        }
                        gui_fixed_sprites.push_back(icon);
                    }

                }

            }
            else{
                int i = 0;
                for(map<string, shared_ptr<Properties> >::iterator parser = properties_map.begin(); parser != properties_map.end(); parser++){
                    if((parser->second->classification == "ENTITY" || parser->second->classification == "POSTER" || parser->second->classification=="FLOOR" || parser->second->classification == "INTERFACE" || parser->second->classification == "CONTAINER" || parser->second->classification == "MACHINE" || parser->second->classification == "TERMINAL" || parser->second->classification == "CONTROLBOARD") && parser->second->name != "Piston Head" && parser->second->name != "Huge Asteroid 1" && parser->second->name != "Big Asteroid 1" && parser->second->name != "Medium Asteroid 1"){
                        RectangleShape slot;
                        slot.setSize(Vector2f(80,80));
                        slot.setOrigin(40, 40);
                        slot.setFillColor(gui_color_b - Color(40,40,40,0) + Color(0,0,0,15));
                        slot.setOutlineColor(gui_color_b + Color(0,0,0,155));
                        slot.setOutlineThickness(2);
                        slot.setPosition(50 + ((i%12)*90), 850 - (floor(i/12.0)*90));

                        if(slot.getGlobalBounds().left + slot.getGlobalBounds().width > inventory_bounds.width){ inventory_bounds.width = slot.getGlobalBounds().left + slot.getGlobalBounds().width; }
                        if(900-slot.getGlobalBounds().top > inventory_bounds.height){ inventory_bounds.height = 900-slot.getGlobalBounds().top; }

                        Sprite icon;
                        icon.setTexture(parser->second->icon, true);
                        IntRect icon_size = icon.getTextureRect();
                        icon.setOrigin(icon_size.width/2, icon_size.height/2);
                        icon.setColor(Color(255,255,255,225));
                        icon.setPosition(slot.getPosition());


                        if(slot.getGlobalBounds().contains(input.fixed_mouse)){

                            gui_captions.push_back(Caption(parser->second->name, "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 28, Color(255,255,255,205), "left"));

                            slot.setScale(1.1,1.1);
                            icon.setScale(1.1,1.1);

                            if(input.lmb_released){
                                if(gui_hovering_item_id != "" && parser->second->name == getSector(sector_id)->items[gui_hovering_item_id]->properties->name){
                                    getSector(sector_id)->trash(gui_hovering_item_id);
                                    gui_hovering_item_id = "";
                                }
                                else if(gui_hovering_item_id != ""){
                                    getSector(sector_id)->trash(gui_hovering_item_id);
                                    gui_hovering_item_id = registerNewItem(sector_id, createUniqueId(),parser->second->name);
                                }
                                else{
                                    gui_hovering_item_id = registerNewItem(sector_id, createUniqueId(),parser->second->name);
                                }

                                gui_machine_id_a = "";
                                gui_port_id_a = "";

                                floor_tile_1_placed = false;
                                floor_tile_1_coords = Vector2f(0,0);
                                floor_tile_1_rotation = 0;

                            }
                        }

                        gui_fixed_rectangles.push_back(slot);
                        gui_fixed_sprites.push_back(icon);
                        i++;
                    }
                }
            }

        }
        else{

            if(free_build){
                RectangleShape slot;
                slot.setSize(Vector2f(80,80));
                slot.setOrigin(40, 40);
                slot.setFillColor(gui_color_b - Color(40,40,40,0) + Color(0,0,0,15));
                slot.setOutlineColor(gui_color_b + Color(0,0,0,155));
                slot.setOutlineThickness(2);
                slot.setPosition(50 + (((0)%3)*85), 850 - (floor((0)/3)*85));
                gui_fixed_rectangles.push_back(slot);

                Sprite icon;
                icon.setTexture(properties_map.begin()->second->icon, true);
                IntRect icon_size = icon.getTextureRect();
                icon.setOrigin(icon_size.width/2, icon_size.height/2);
                icon.setColor(Color(255,255,255,225));
                icon.setPosition(slot.getPosition());
                gui_fixed_sprites.push_back(icon);


                gui_captions.push_back(Caption("FREE BUILD", "font1", Vector2f(12, 790), 28, gui_color_b + Color(0,0,0,155),"left"));
            }
            else{
                RectangleShape slot;
                slot.setSize(Vector2f(80,80));
                slot.setOrigin(40, 40);
                slot.setFillColor(gui_color_b - Color(40,40,40,0) + Color(0,0,0,15));
                slot.setOutlineColor(gui_color_b + Color(0,0,0,155));
                slot.setOutlineThickness(2);
                slot.setPosition(50 + (((0)%3)*85), 850 - (floor((0)/3)*85));
                gui_fixed_rectangles.push_back(slot);
                gui_slot_boxes.push_back(slot.getGlobalBounds());
                gui_slot_contents.push_back(&player->contents[1]);

                if(player->contents[1] != ""){
                    Sprite icon;
                    icon.setTexture(getSector(sector_id)->items[player->contents[1]]->properties->icon, true);
                    IntRect icon_size = icon.getTextureRect();
                    icon.setOrigin(icon_size.width/2, icon_size.height/2);
                    icon.setColor(Color(255,255,255,225));
                    icon.setPosition(slot.getPosition());
                    gui_fixed_sprites.push_back(icon);
                }

                gui_captions.push_back(Caption("INVENTORY", "font1", Vector2f(12, 790), 28, gui_color_b + Color(0,0,0,155),"left"));
            }


            inventory_bounds.width += 100;
            inventory_bounds.height += 100;
        }

        inventory_bounds.left = 0;
        inventory_bounds.top = 900 - inventory_bounds.height;

        toolbox_bounds = FloatRect(0,0,0,0);
        if(toolbox_open){

            bool hovering_is_tool_type = false;
            if(gui_hovering_item_id != ""){
                for(vector<string>::iterator parser = toolbox_types.begin(); parser != toolbox_types.end(); parser++){
                    if(getSector(sector_id)->items[gui_hovering_item_id]->properties->name == *parser){
                        hovering_is_tool_type = true;
                        break;
                    }
                }
            }
            for(int i = 0; i < toolbox_types.size(); i++){
                RectangleShape slot;
                slot.setSize(Vector2f(80,80));
                slot.setOrigin(40, 40);
                slot.setFillColor(gui_color_a + Color(0,0,0,15));
                slot.setOutlineColor(gui_color_a + Color(0,0,0,155));
                slot.setOutlineThickness(2);
                slot.setPosition(50 + ((i%2)*85), (inventory_bounds.top - 70) - (floor(i/2)*85));

                if(slot.getGlobalBounds().left + slot.getGlobalBounds().width > toolbox_bounds.width){ toolbox_bounds.width = slot.getGlobalBounds().left + slot.getGlobalBounds().width; }
                if((inventory_bounds.top - 30)-slot.getGlobalBounds().top > toolbox_bounds.height){ toolbox_bounds.height = (inventory_bounds.top - 30)-slot.getGlobalBounds().top; }


                Sprite icon;
                icon.setTexture(getProperties(toolbox_types[i])->icon, true);
                IntRect icon_size = icon.getTextureRect();
                icon.setOrigin(icon_size.width/2, icon_size.height/2);
                icon.setColor(Color(255,255,255,225));
                icon.setPosition(slot.getPosition());

                if(slot.getGlobalBounds().contains(input.fixed_mouse)){
                    slot.setScale(1.1,1.1);
                    icon.setScale(1.1,1.1);

                    gui_captions.push_back(Caption(toolbox_types[i], "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 28, Color(255,255,255,205), "left"));

                    if(free_build){
                        if(input.lmb_released){
                            if(gui_hovering_item_id != "" && getSector(sector_id)->items[gui_hovering_item_id]->properties->name == toolbox_types[i]){
                                getSector(sector_id)->trash(gui_hovering_item_id);
                                gui_hovering_item_id = "";
                            }
                            else if(gui_hovering_item_id != ""){
                                getSector(sector_id)->trash(gui_hovering_item_id);
                                gui_hovering_item_id = registerNewItem(sector_id, createUniqueId(),toolbox_types[i]);
                            }
                            else{
                                gui_hovering_item_id = registerNewItem(sector_id, createUniqueId(),toolbox_types[i]);
                            }

                            gui_machine_id_a = "";
                            gui_port_id_a = "";

                            floor_tile_1_placed = false;
                            floor_tile_1_coords = Vector2f(0,0);
                            floor_tile_1_rotation = 0;
                        }
                    }
                    else{
                        if(input.lmb_released && (gui_hovering_item_id == "" || hovering_is_tool_type)){
                            if(hovering_is_tool_type && getSector(sector_id)->items[gui_hovering_item_id]->properties->name == toolbox_types[i]){
                                getSector(sector_id)->trash(gui_hovering_item_id);
                                gui_hovering_item_id = "";
                            }
                            else if(hovering_is_tool_type){
                                getSector(sector_id)->trash(gui_hovering_item_id);
                                gui_hovering_item_id = registerNewItem(sector_id, createUniqueId(),toolbox_types[i]);
                            }
                            else{
                                gui_hovering_item_id = registerNewItem(sector_id, createUniqueId(),toolbox_types[i]);
                            }

                            gui_machine_id_a = "";
                            gui_port_id_a = "";

                            floor_tile_1_placed = false;
                            floor_tile_1_coords = Vector2f(0,0);
                            floor_tile_1_rotation = 0;
                        }
                    }

                }

                gui_fixed_rectangles.push_back(slot);
                gui_fixed_sprites.push_back(icon);
            }
        }
        else{
            RectangleShape slot;
            slot.setSize(Vector2f(80,80));
            slot.setOrigin(40, 40);
            slot.setFillColor(gui_color_a + Color(0,0,0,15));
            slot.setOutlineColor(gui_color_a + Color(0,0,0,155));
            slot.setOutlineThickness(2);
            slot.setPosition(50, (inventory_bounds.top-30)-40);
            gui_captions.push_back(Caption("TOOLBOX", "font1", Vector2f(12, inventory_bounds.top-130), 28, gui_color_a + Color(0,0,0,155),"left"));

            Sprite icon;
            icon.setTexture(getProperties(toolbox_types[0])->icon, true);
            IntRect icon_size = icon.getTextureRect();
            icon.setOrigin(icon_size.width/2, icon_size.height/2);
            icon.setColor(Color(255,255,255,225));
            icon.setPosition(slot.getPosition());

            gui_fixed_rectangles.push_back(slot);
            gui_fixed_sprites.push_back(icon);

            toolbox_bounds.width += 100;
            toolbox_bounds.height += 100;
        }

        toolbox_bounds.left = 0;
        toolbox_bounds.top = (inventory_bounds.top - 30) - toolbox_bounds.height;

        inventory_bounds.height += 60;
        inventory_bounds.width += 60;
        inventory_bounds.top -= 60;
        toolbox_bounds.top -= 60;
        toolbox_bounds.width += 60;
        toolbox_bounds.height += 80;

        if(gui_hovering_item_id != ""){
            Sprite icon;
            icon.setTexture(getSector(sector_id)->items[gui_hovering_item_id]->properties->icon, true);
            IntRect icon_size = icon.getTextureRect();
            icon.setOrigin(icon_size.width/2, icon_size.height/2);
            icon.setColor(Color(255,255,255,155));
            icon.setPosition(input.fixed_mouse);
            gui_fixed_sprites.push_back(icon);
        }
        if(selected_object_id != "" && getSector(sector_id)->getClassification(selected_object_id) == "INTERFACE"){
            gui_captions.push_back(Caption(getSector(sector_id)->interfaces[selected_object_id]->caption, "font1", Vector2f(input.fixed_mouse.x + 40, input.fixed_mouse.y - 5), 34, Color(255,255,255,205), "left"));
        }
    }
    else{
        gui_captions.push_back(Caption(asString(packages_saved) + " Packages Survived", "font1", Vector2f(window.getSize().x/2.0,window.getSize().y/2.0), 120, Color(185,225,185,200), "middle"));
    }

    //meta controls
    double scale_rate = scale*scale*20;
    if(scale_rate > 80){scale_rate = 80;}
    if(input.mmb_delta>0 and scale > 0.3){scale -= scale_rate*dt; if(scale < 0.3){scale = 0.3;}}
    else if(input.mmb_delta<0 and scale < 100){scale += scale_rate*dt;}

    if(input.keys_released.count("c") != 0){
        free_build = !free_build;
    }

    //
}

bool Session::processGame()
{
    window.clear();

    dt = timer.getElapsedTime().asSeconds();
    if(dt>1){dt = 0.001;}
    timer.restart();

    if(getSector(sector_id)->ents.count(player_id) == 0){
        for(map<string, shared_ptr<Sector> >::iterator parser = world.sectors.begin(); parser != world.sectors.end(); parser++){
            if(parser->second->ents.count(player_id) != 0){
                sector_id = parser->first;
                break;
            }
        }
    }

    if(!file_menu_open){
        world.date.update(dt);
        world.enactChanges();
    }

    input.reset();
    input.dt = dt;
    input.player_id = player_id;
    input.sector_id = sector_id;

    //collect input
    input.mouse = window.mapPixelToCoords(Mouse::getPosition(window),view);
    input.fixed_mouse = window.mapPixelToCoords(Mouse::getPosition(window),fixedview);

    sf::Event event;
    while(window.pollEvent(event))
    {
        switch(event.type)
        {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::MouseWheelMoved:
                input.mmb_delta = event.mouseWheel.delta;
                break;

            case sf::Event::MouseButtonReleased:
                if(event.mouseButton.button==sf::Mouse::Left){  input.lmb_released = true;}
                else if(event.mouseButton.button==sf::Mouse::Right){input.rmb_released = true;}
                break;

            case sf::Event::TextEntered:
                string key_id;
                if(event.text.unicode >= 32 && event.text.unicode <= 126){ input.text_entered.insert(event.text.unicode); }
                if(event.text.unicode == 32){ key_id += "space"; }
                else if(event.text.unicode == 8){ key_id += "delete"; }
                else if(event.text.unicode == 13){ key_id += "enter"; }
                else if(event.text.unicode == 27){ key_id += "esc"; }
                else{ key_id += char(event.text.unicode); }
                for(string::iterator parser = key_id.begin(); parser != key_id.end(); parser++){ *parser = tolower(*parser); }
                input.keys_released.insert(key_id);
                break;
        }
    }
    if(Keyboard::isKeyPressed(Keyboard::LShift)){input.keys_held.insert("lshift");}
    if(Keyboard::isKeyPressed(Keyboard::A)){input.keys_held.insert("a");}
    if(Keyboard::isKeyPressed(Keyboard::W)){input.keys_held.insert("w");}
    if(Keyboard::isKeyPressed(Keyboard::S)){input.keys_held.insert("s");}
    if(Keyboard::isKeyPressed(Keyboard::D)){input.keys_held.insert("d");}
    if(Keyboard::isKeyPressed(Keyboard::Q)){input.keys_held.insert("q");}
    if(Keyboard::isKeyPressed(Keyboard::E)){input.keys_held.insert("e");}
    if(Keyboard::isKeyPressed(Keyboard::Space)){input.keys_held.insert("space");}
    if(Keyboard::isKeyPressed(Keyboard::LAlt) || Keyboard::isKeyPressed(Keyboard::RAlt)){input.keys_held.insert("alt");}
    if(Keyboard::isKeyPressed(Keyboard::Num1) || Keyboard::isKeyPressed(Keyboard::Numpad1)){input.keys_held.insert("1");}
    if(Keyboard::isKeyPressed(Keyboard::Num2) || Keyboard::isKeyPressed(Keyboard::Numpad2)){input.keys_held.insert("2");}
    if(Keyboard::isKeyPressed(Keyboard::Num3) || Keyboard::isKeyPressed(Keyboard::Numpad3)){input.keys_held.insert("3");}
    if(Keyboard::isKeyPressed(Keyboard::Num4) || Keyboard::isKeyPressed(Keyboard::Numpad4)){input.keys_held.insert("4");}
    if(Keyboard::isKeyPressed(Keyboard::Num5) || Keyboard::isKeyPressed(Keyboard::Numpad5)){input.keys_held.insert("5");}
    if(Keyboard::isKeyPressed(Keyboard::Num6) || Keyboard::isKeyPressed(Keyboard::Numpad6)){input.keys_held.insert("6");}
    if(Keyboard::isKeyPressed(Keyboard::Num7) || Keyboard::isKeyPressed(Keyboard::Numpad7)){input.keys_held.insert("7");}
    if(Keyboard::isKeyPressed(Keyboard::Num8) || Keyboard::isKeyPressed(Keyboard::Numpad8)){input.keys_held.insert("8");}
    if(Keyboard::isKeyPressed(Keyboard::Num9) || Keyboard::isKeyPressed(Keyboard::Numpad9)){input.keys_held.insert("9");}
    if(Keyboard::isKeyPressed(Keyboard::Space)){input.keys_held.insert("space");}
    if(Mouse::isButtonPressed(Mouse::Right)){input.rmb_held = true;}
    if(Mouse::isButtonPressed(Mouse::Left)){input.lmb_held = true;}
    //

    if(!file_menu_open){

        if(time_til_rescue > 0){
            //update world and gui (and forward input from this session to all world machines which need it)
            for(map<string, weak_ptr<Machine> >::iterator parser = getSector(sector_id)->machines.begin(); parser != getSector(sector_id)->machines.end(); parser++){
                if(parser->second.lock()->takes_input){
                    parser->second.lock()->readInput(input);
                }
            }

            //time_til_asteroid -= dt;
            if(time_til_asteroid <= 0 && !free_build){

                int u_bound = 15 + (rand() % 20);
                for(int b_index = 0; b_index < u_bound; b_index++){
                    bombardWithAsteroid();
                }
                time_til_asteroid = 90.0;
            }

            for(map<string, weak_ptr<Machine> >::iterator i = getSector(sector_id)->machines.begin(); i != getSector(sector_id)->machines.end(); i++){

                shared_ptr<Machine> mach = i->second.lock();
                if(mach->properties->name == "Beacon"){

                    if(mach->internal_cells["Buffered Power"]->vars["milliamp hours"] >= ((1000*(mach->wattage/mach->voltage_rating)/60)/60)*dt){
                        //decrement counter
                        time_til_rescue -= dt;
                        if(time_til_rescue <= 0){
                            //count packages
                            packages_saved = 0;
                            for(map<string, shared_ptr<Entity> >::iterator j = getSector(sector_id)->ents.begin(); j != getSector(sector_id)->ents.end(); j++){
                                if(j->second->properties->name == "Package"){
                                    packages_saved++;
                                }
                            }
                        }
                    }
                }
            }

            getSector(sector_id)->process(dt);
            world.processTransfers();
            getSector(sector_id)->emptyGarbageBin();
            if(getSector(sector_id)->ents.count(player_id) == 0){
                return true;
            }
            
        }
        updateGUI();
        //getSector(sector_id)->getEnt(player_id)->ghost = free_build;
        getSector(sector_id)->getEnt(player_id)->ghost = false;
        //
    }
    else{
        if(input.keys_released.count("esc")!=0){ file_menu_open = false; }
    }


    //update the viewport
    view.setSize(fixedview.getSize().x*scale,fixedview.getSize().y*scale);
    view.setCenter(getSector(sector_id)->getEnt(player_id)->sprite.getPosition());
    view.setRotation(getSector(sector_id)->getEnt(player_id)->sprite.getRotation());
    //


    //draw everything in layers
    if(getSector(sector_id)->planetside){
        window.setView(view);
    }
    else{
        window.setView(fixedview);
    }

    getSector(sector_id)->drawBackdrop();
    window.setView(view);
    getSector(sector_id)->draw();
    drawGUI();
    //

    if(file_menu_open){
        //file menu

        vector<RectangleShape> file_rects;
        vector<Caption> file_texts;

        file_rects.push_back(createRectangle(Vector2f(window.getSize().x/2,window.getSize().y/2), Vector2f(window.getSize().x,window.getSize().y), 0, Color(0,0,0,225), Color(0,0,0,0)));
        file_texts.push_back(Caption("FILE MENU", "font1", Vector2f(50, 30), 72, Color(255,255,255,200), "left"));
        file_rects.push_back(createRectangle(Vector2f(window.getSize().x/2,70), Vector2f(window.getSize().x-60,5), 0, Color(255,255,255,200), Color(0,0,0,0)));
        file_texts.push_back(Caption("SAVE", "font1", Vector2f(150, 100), 72, Color(255,255,255,200), "middle"));
        file_texts.push_back(Caption("LOAD", "font1", Vector2f(500, 100), 72, Color(255,255,255,200), "middle"));
        file_texts.push_back(Caption("HELP", "font1", Vector2f(850, 100), 72, Color(255,255,255,200), "middle"));
        file_texts.push_back(Caption("EXIT", "font1", Vector2f(1200, 100), 72, Color(255,255,255,200), "middle"));

        if(hypot(input.fixed_mouse.x-file_texts[1].text.getPosition().x,input.fixed_mouse.y-file_texts[1].text.getPosition().y)<120){
            file_texts[1].text.setScale(0.6,0.6);
            if(input.lmb_released){
                load_menu_mode = false;
                save_menu_mode = true;
            }
        }
        if(hypot(input.fixed_mouse.x-file_texts[2].text.getPosition().x,input.fixed_mouse.y-file_texts[2].text.getPosition().y)<120){
            file_texts[2].text.setScale(0.6,0.6);
            if(input.lmb_released){
                save_menu_mode = false;
                load_menu_mode = true;
            }
        }
        if(hypot(input.fixed_mouse.x-file_texts[3].text.getPosition().x,input.fixed_mouse.y-file_texts[3].text.getPosition().y)<120){
            file_texts[3].text.setScale(0.6,0.6);
        }
        if(hypot(input.fixed_mouse.x-file_texts[4].text.getPosition().x,input.fixed_mouse.y-file_texts[4].text.getPosition().y)<120){
            file_texts[4].text.setScale(0.6,0.6);
            if(input.lmb_released){
                window.close();
                return false;
            }
        }

        if(save_menu_mode){
            file_texts.push_back(Caption("Enter save name:", "font1", Vector2f((window.getSize().x/2)-240, (window.getSize().y/2)-75), 55, Color(255,255,255,200), "left"));
            file_rects.push_back(createRectangle(Vector2f(window.getSize().x/2, window.getSize().y/2), Vector2f(500, 80), 5, Color(55,55,55,225), Color(255,255,255,200)));
            if(input.keys_released.count("delete") != 0 && file_string.size() > 0){
                file_string.pop_back();
            }
            else{
                for(set<char>::iterator key = input.text_entered.begin(); key != input.text_entered.end(); key++){
                    if(file_string.size() < 27){
                        file_string = file_string + *key;
                    }
                }
            }

            file_texts.push_back(Caption(file_string, "font1", Vector2f((window.getSize().x/2)-236, (window.getSize().y/2)-5), 60, Color(255,255,255,200), "left"));
            blinker += dt;
            if(file_string.size() < 27 && sin(blinker*6)>0){
                file_texts.push_back(Caption("|", "font1", Vector2f(((window.getSize().x/2)-236)+(file_string.size()*17.6), (window.getSize().y/2)), 60, Color(255,255,255,200), "left"));
            }
            file_texts.push_back(Caption("Done", "font1", Vector2f((window.getSize().x/2)+208, (window.getSize().y/2)+75), 55, Color(255,255,255,200), "middle"));
            if(hypot(input.fixed_mouse.x-file_texts[file_texts.size()-1].text.getPosition().x,input.fixed_mouse.y-file_texts[file_texts.size()-1].text.getPosition().y)<80){
                file_texts[file_texts.size()-1].text.setScale(0.55,0.55);
                if(input.lmb_released && file_string != ""){
                    world.saveWorldToFile(file_string);
                    file_string = "";
                }
            }
            if(input.keys_released.count("enter") != 0 && file_string != ""){
                world.saveWorldToFile(file_string);
                file_string = "";
            }

        }
        if(load_menu_mode){

            file_rects.push_back(createRectangle(Vector2f(window.getSize().x/2, (window.getSize().y/2)+80), Vector2f(window.getSize().x-80, window.getSize().y-300), 5, Color(55,55,55,225), Color(255,255,255,200)));
            TiXmlDocument main_index("saves\\index.xml");
            main_index.LoadFile();
            int i = 0;
            int display_i = 0;
            for(TiXmlElement* save_parser = main_index.FirstChildElement(); save_parser != NULL; save_parser = save_parser->NextSiblingElement("Save")){
                if(i>=list_start_index && display_i<12){
                    file_texts.push_back(Caption(save_parser->GetText(),"font1", Vector2f(55, (((window.getSize().y/2)+95)-((window.getSize().y-300)/2))+(display_i*50)), 60, Color(255,255,255,200), "left"));
                    if(abs(input.fixed_mouse.y-file_texts[file_texts.size()-1].text.getPosition().y)<23){
                        file_rects.push_back(createRectangle(Vector2f(window.getSize().x/2,file_texts[file_texts.size()-1].text.getPosition().y+10), Vector2f(window.getSize().x-80, 50), 0, Color(0,155,155,155), Color(0,0,0,0)));
                        if(input.lmb_released){
                            shared_ptr<Item> carry_over;
                            if(gui_hovering_item_id != ""){
                                carry_over = getSector(sector_id)->items[gui_hovering_item_id];
                            }
                            world.loadWorldFromFile(file_texts[file_texts.size()-1].text.getString());
                            if(gui_hovering_item_id != ""){
                                getSector(sector_id)->items[gui_hovering_item_id] = carry_over;
                            }
                        }
                    }
                    display_i++;
                }
                i++;
            }
            if(input.mmb_delta > 0){ list_start_index -= 100*dt; }
            if(input.mmb_delta < 0){ list_start_index += 100*dt; }
            if(list_start_index > i-12){list_start_index = i-12;}
            if(list_start_index < 0){list_start_index = 0;}

        }

        window.setView(fixedview);
        for(vector<RectangleShape>::iterator parser = file_rects.begin(); parser != file_rects.end(); parser++){
            window.draw(*parser);
        }
        for(vector<Caption>::iterator parser = file_texts.begin(); parser != file_texts.end(); parser++){
            parser->draw();
        }
    }

    window.display(); //draw window to screen
    return true;
}



