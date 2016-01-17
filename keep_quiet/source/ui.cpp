#include "main.h"

using namespace std;
using namespace sf;

extern map<string, StructureProperties> structure_properties;

InputStruct::InputStruct(){

	view_mouse = Vector2f(0,0);
	window_mouse = Vector2f(0,0);
	
	mmb_delta = 0;
	lmb_released = false;
	rmb_released = false;
	lmb_held = false;
	rmb_held = false;
}

void InputStruct::collect(RenderWindow &window, View &view, View &window_view){

	view_mouse = window.mapPixelToCoords(Mouse::getPosition(window),view);
	window_mouse = window.mapPixelToCoords(Mouse::getPosition(window),window_view);

	mmb_delta = 0;
	lmb_released = false;
	rmb_released = false;
	lmb_held = false;
	rmb_held = false;

	keys_released.clear();
	keys_held.clear();
	text_entered.clear();

	sf::Event event;
    while(window.pollEvent(event))
    {
        switch(event.type)
        {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::MouseWheelMoved:
                mmb_delta = event.mouseWheel.delta;
                break;

            case sf::Event::MouseButtonReleased:
                if(event.mouseButton.button==sf::Mouse::Left){  lmb_released = true;}
                else if(event.mouseButton.button==sf::Mouse::Right){ rmb_released = true;}
                break;

            case sf::Event::TextEntered:
                string key_id;
                if(event.text.unicode >= 32 && event.text.unicode <= 126){ text_entered.insert(event.text.unicode); }
                if(event.text.unicode == 32){ key_id += "space"; }
                else if(event.text.unicode == 8){ key_id += "delete"; }
                else if(event.text.unicode == 13){ key_id += "enter"; }
                else if(event.text.unicode == 27){ key_id += "esc"; }
                else{ key_id += char(event.text.unicode); }
                for(string::iterator parser = key_id.begin(); parser != key_id.end(); parser++){ *parser = tolower(*parser); }
                keys_released.insert(key_id);
                break;
        }
    }
    if(Keyboard::isKeyPressed(Keyboard::LShift)){ keys_held.insert("lshift");}
    if(Keyboard::isKeyPressed(Keyboard::A)){ keys_held.insert("a");}
    if(Keyboard::isKeyPressed(Keyboard::W)){ keys_held.insert("w");}
    if(Keyboard::isKeyPressed(Keyboard::S)){ keys_held.insert("s");}
    if(Keyboard::isKeyPressed(Keyboard::D)){ keys_held.insert("d");}
    if(Keyboard::isKeyPressed(Keyboard::Q)){ keys_held.insert("q");}
    if(Keyboard::isKeyPressed(Keyboard::E)){ keys_held.insert("e");}
    if(Keyboard::isKeyPressed(Keyboard::Space)){ keys_held.insert("space");}
    if(Keyboard::isKeyPressed(Keyboard::LAlt) || Keyboard::isKeyPressed(Keyboard::RAlt)){ keys_held.insert("alt");}
    if(Keyboard::isKeyPressed(Keyboard::Num1) || Keyboard::isKeyPressed(Keyboard::Numpad1)){ keys_held.insert("1");}
    if(Keyboard::isKeyPressed(Keyboard::Num2) || Keyboard::isKeyPressed(Keyboard::Numpad2)){ keys_held.insert("2");}
    if(Keyboard::isKeyPressed(Keyboard::Num3) || Keyboard::isKeyPressed(Keyboard::Numpad3)){ keys_held.insert("3");}
    if(Keyboard::isKeyPressed(Keyboard::Num4) || Keyboard::isKeyPressed(Keyboard::Numpad4)){ keys_held.insert("4");}
    if(Keyboard::isKeyPressed(Keyboard::Num5) || Keyboard::isKeyPressed(Keyboard::Numpad5)){ keys_held.insert("5");}
    if(Keyboard::isKeyPressed(Keyboard::Num6) || Keyboard::isKeyPressed(Keyboard::Numpad6)){ keys_held.insert("6");}
    if(Keyboard::isKeyPressed(Keyboard::Num7) || Keyboard::isKeyPressed(Keyboard::Numpad7)){ keys_held.insert("7");}
    if(Keyboard::isKeyPressed(Keyboard::Num8) || Keyboard::isKeyPressed(Keyboard::Numpad8)){ keys_held.insert("8");}
    if(Keyboard::isKeyPressed(Keyboard::Num9) || Keyboard::isKeyPressed(Keyboard::Numpad9)){ keys_held.insert("9");}
    if(Keyboard::isKeyPressed(Keyboard::Space)){ keys_held.insert("space");}
    if(Mouse::isButtonPressed(Mouse::Right)){ rmb_held = true;}
    if(Mouse::isButtonPressed(Mouse::Left)){ lmb_held = true;}

}

VisualsStruct::VisualsStruct(){

    show_build_menu = true;
    show_stats_menu = true;
    current_index = 0;
    selected_type_id = "";
    selected_structure_id = "";
}
void VisualsStruct::update(RenderWindow &window, InputStruct input, Terrain &terrain, map<string, Structure*> &structures, map<string, SmartWorker*> &workers, int total_ammunition, int total_fuel, int total_cash, int total_power, int total_supply, int total_construction, int total_workers, int used_power, int used_workers, int used_supply){

    //reset the visuals
    captions.clear();
    rectangles.clear();
    sprites.clear();
    window_captions.clear();
    window_rectangles.clear();
    window_sprites.clear();
    //

    //act on input to show base stats menu and construction menu
    if(input.keys_released.count("b") != 0){
        show_stats_menu = !show_stats_menu;
    }
    if(input.keys_released.count("c") != 0){
        show_build_menu = !show_build_menu;
    }
    //

    //if an already-built structure is selected, highlight it with extra UI
    if(selected_structure_id != ""){
        rectangles.push_back(createBoundingRectangle(structures[selected_structure_id]->bounds,Color(185,185,245,215)));
    }
    //

    //draw per-structure ui
    for(map<string, Structure*>::iterator i = structures.begin(); i != structures.end(); i++){

        //display a counter of tasked workers vs max workers above this structure
        int filled_workers = i->second->tasked_workers.size(); 
        int max_workers = i->second->getMaxWorkers();
        if(i->second->construction_name != ""){
            //if this structure is under construction
            max_workers = 4;
        }
        Vector2f anchor = Vector2f((i->second->bounds.left + i->second->bounds.width), i->second->bounds.top - (getTextureRect("filled_worker").height/2.0) - 8);
        for(int index = 0; index < max_workers; index++){
            if(index < filled_workers){
                sprites.push_back(createSprite("filled_worker", anchor, "right"));
            }
            else{
                sprites.push_back(createSprite("empty_worker", anchor, "right"));
            }
            anchor.x -= getTextureRect("filled_worker").width;
        }
        //

        if(i->second->bounds.contains(input.view_mouse)){
            rectangles.push_back(createBoundingRectangle(i->second->bounds,Color(253,130,43,215))); //highlight the structure if the mouse is hovering over top
            if(selected_type_id == ""){ //if the user is not actively placing a new structure, placed structures may respond to input 
                if(input.lmb_released && input.keys_held.count("lshift") != 0){ //open this structure's upgrade interface
                    if(i->second->construction_name == "" && selected_structure_id == i->first){
                        selected_structure_id = "";
                    }
                    else if(i->second->construction_name == ""){ //change/close which structure has its upgrade interface open
                        selected_structure_id = i->first;
                    }
                }
                else if(input.lmb_released){ //left click tasks another worker to this structure
                    //task worker
                    if(i->second->tasked_workers.size() < max_workers){
                        for(map<string, SmartWorker*>::iterator j = workers.begin(); j != workers.end(); j++){
                            if(j->second->tasked_structure_id == ""){
                                j->second->tasked_structure_id = i->first;
                                i->second->tasked_workers.push_back(j->second);
                                break;
                            }
                        }
                    }
                }
                else if(input.rmb_released){ //right click removes an already-tasked worker from this structure
                    //untask worker
                    if(i->second->tasked_workers.size() != 0){
                        i->second->tasked_workers[i->second->tasked_workers.size()-1]->tasked_structure_id = "";
                        i->second->tasked_workers.pop_back();
                    }
                }
            }
            
        }

        
    }
    //

    Vector2f top_left_anchor = Vector2f(0,0); //define the top left of the window for ui creation(for ease of changing later in development, like if someone wants to add a menu bar across the top of the window)

    if(show_stats_menu){ //if stats menu is supposed to be shown right now

        //menu properties
        double margin = 14;
        int character_size = 30;
        Color text_colour = Color(253,130,43,205);
        int outline_width = 4;
        string font_id = "font1";
        //

        Vector2f anchor = top_left_anchor + Vector2f(margin,margin); //the first caption should be inset from the top left corner by the thickness of the menu outline, so that the window_captions do not overlap the outline. Inset extra according to margin setting.
        double far_right = 0; //as captions are created note the far-right value, so that later on in this function we will know how wide the bounding box needs to be
        
        //first display information on the base's status
        window_captions.push_back(Caption("Construction Rate >> " + asString(total_construction), font_id, anchor, character_size, text_colour, "left")); //generate a left-oriented caption at the current value of the anchor
        anchor = anchor + Vector2f(0,window_captions[window_captions.size()-1].text.getGlobalBounds().height + margin); //shift the anchor down the screen by an amount equal to the height of the caption we just generated, plus an extra amount equal to the margin setting.
        if(anchor.x + window_captions[window_captions.size()-1].text.getGlobalBounds().width > far_right){ far_right = anchor.x + window_captions[window_captions.size()-1].text.getGlobalBounds().width; } //if the far-right tip of this caption is the rightmost point in this menu so far, set the rightmost point equal to the far-right tip of this caption
       
        window_captions.push_back(Caption("Used Power >> " + asString(used_power) + "/" + asString(total_power), font_id, anchor, character_size, text_colour, "left"));//generate a left-oriented caption at the current value of the anchor
        anchor = anchor + Vector2f(0,window_captions[window_captions.size()-1].text.getGlobalBounds().height + margin);//shift the anchor down the screen by an amount equal to the height of the caption we just generated, plus an extra amount equal to the margin setting.
        if(anchor.x + window_captions[window_captions.size()-1].text.getGlobalBounds().width > far_right){ far_right = anchor.x + window_captions[window_captions.size()-1].text.getGlobalBounds().width; }//generate a left-oriented caption at the current value of the anchor
       
        window_captions.push_back(Caption("Used Supply >> " + asString(used_supply) + "/" + asString(total_supply), font_id, anchor, character_size, text_colour, "left"));//generate a left-oriented caption at the current value of the anchor
        anchor = anchor + Vector2f(0,window_captions[window_captions.size()-1].text.getGlobalBounds().height + margin);//shift the anchor down the screen by an amount equal to the height of the caption we just generated, plus an extra amount equal to the margin setting.
        if(anchor.x + window_captions[window_captions.size()-1].text.getGlobalBounds().width > far_right){ far_right = anchor.x + window_captions[window_captions.size()-1].text.getGlobalBounds().width; }//generate a left-oriented caption at the current value of the anchor
      
        window_captions.push_back(Caption("Used Workers >> " + asString(used_workers) + "/" + asString(total_workers), font_id, anchor, character_size, text_colour, "left"));//generate a left-oriented caption at the current value of the anchor
        anchor = anchor + Vector2f(0,window_captions[window_captions.size()-1].text.getGlobalBounds().height + margin);//shift the anchor down the screen by an amount equal to the height of the caption we just generated, plus an extra amount equal to the margin setting.
        if(anchor.x + window_captions[window_captions.size()-1].text.getGlobalBounds().width > far_right){ far_right = anchor.x + window_captions[window_captions.size()-1].text.getGlobalBounds().width; }//generate a left-oriented caption at the current value of the anchor

        anchor = anchor + Vector2f(0,margin); //shift the anchor down the screen by the margin value (to add a visual gap between the base status captions and the base inventory captions)

        //now display information on the base's inventory
        //we don't need to update the rightmost point for any of the inventory window_captions except for the last one, since the last one is guaranteed to be farther right than the others.
        window_sprites.push_back(createSprite("fuel_icon", anchor, "left")); //generate a sprite icon to precede the fuel caption
        anchor = anchor + Vector2f(32 + margin,0); //shift the anchor across the screen by the width of the icon, plus the margin setting
        window_captions.push_back(Caption(asString(total_fuel), font_id, anchor, character_size, text_colour, "left")); //generate a left-oriented caption at the anchor displaying the total fuel
        anchor = anchor + Vector2f(window_captions[window_captions.size()-1].text.getGlobalBounds().width + margin,0); //shift the anchor across the screen by the width of the caption we just generated, plus the margin setting

        window_sprites.push_back(createSprite("ammunition_icon", anchor, "left")); //generate a sprite icon to precede the ammunition caption
        anchor = anchor + Vector2f(32 + margin,0); //shift the anchor across the screen by the width of the icon, plus the margin setting
        window_captions.push_back(Caption(asString(total_ammunition), font_id, anchor, character_size, text_colour, "left")); //generate a left-oriented caption at the anchor displaying the total ammunition
        anchor = anchor + Vector2f(window_captions[window_captions.size()-1].text.getGlobalBounds().width + margin,0); //shift the anchor across the screen by the width of the caption we just generated, plus the margin setting

        window_sprites.push_back(createSprite("money_icon", anchor, "left")); //generate a sprite icon to precede the cash caption
        anchor = anchor + Vector2f(32 + margin,0); //shift the anchor across the screen by the width of the icon, plus the margin setting
        window_captions.push_back(Caption(asString(total_cash), font_id, anchor, character_size, text_colour, "left")); //generate a left-oriented caption at the anchor displaying the total cash
        anchor = anchor + Vector2f(window_captions[window_captions.size()-1].text.getGlobalBounds().width + margin,0); //shift the anchor across the screen by the width of the caption we just generated, plus the margin setting

        if(anchor.x + window_captions[window_captions.size()-1].text.getGlobalBounds().width > far_right){ far_right = anchor.x+ window_captions[window_captions.size()-1].text.getGlobalBounds().width; } //now check if this last caption was farther right than the rightmost point
        double far_bottom = anchor.y + window_captions[window_captions.size()-1].text.getGlobalBounds().height; //the bottom of this last caption will be the farthest-down point on the menu
        far_right += (margin-outline_width); //add the margin setting to expand the right side of the menu (if the margin is > 0), and since we added the outline width earlier to offset the captions we need to negate that now so that the backdrop isn't made too large
        far_bottom += (margin-outline_width); //add the margin setting to expand the bottom side of the menu (if the margin is > 0), and since we added the outline width earlier to offset the captions we need to negate that now so that the backdrop isn't made too large

        window_rectangles.push_back(createRectangle(Vector2f((far_right/2.0),(far_bottom/2.0)), Vector2f(far_right,far_bottom), outline_width, Color(15,15,15,205), Color(253,130,43,75))); //create the menu background of size width=far_right, height=far_bottom
    }

    if(show_build_menu){ //if the build menu is supposed to be shown right now

        //menu properties
        int character_size = 30;
        Color text_colour = Color(253,130,43,205);
        int outline_width = 4;
        string font_id = "font1";
        double build_menu_width = 220; //backdrop width is constant for this menu but calculated for the stats menu. This is because the stats menu requires a bit more formatting and needs to be tight and so calculating the necessary width is easier, whereas for the build menu there's so many items it's easier to just set one all-encompassing width.
        double margin = 8;
        //

        vector<string> build_list; //make an ordered indexed list of all the different kinds of structures
        if(selected_structure_id == ""){ //list all standalone buildings if no structure is selected
            for(map<string, StructureProperties>::iterator i = structure_properties.begin(); i != structure_properties.end(); i++){
                if(i->second.parent != ""){ continue; }
                build_list.push_back(i->first);
            }
        }
        else{ //if a building is selected list all possible upgrades
            for(map<string, StructureProperties>::iterator i = structure_properties.begin(); i != structure_properties.end(); i++){
                if(i->second.parent == structures[selected_structure_id]->type_name && !structures[selected_structure_id]->hasUpgrade(i->first)){ build_list.push_back(i->first); }  
            }
        }

        double build_menu_height = window.getSize().y; //to determine how many items can be shown in the build list at any given time, we note the window height and if the stats menu is shown, we subtract that height.
        if(show_stats_menu){
            build_menu_height -= (window_rectangles[window_rectangles.size()-1].getGlobalBounds().height);
        }

        double build_menu_item_thickness = 2.0*character_size; //determine how many items can fit in a build menu of this height, and if they don't all fit at once set the max scroll index to a number > 0 to allow scrolling
        int max_index = build_list.size() - floor(build_menu_height/build_menu_item_thickness);
        if(max_index < 0){ max_index = 0; }

        if(input.keys_held.count("lshift") == 0){ //if scrolling is happening and lshift is not held down, scroll the build menu
            current_index -= static_cast<int>(input.mmb_delta);
        }
      
        if(current_index < 0){ current_index = 0; } //prevents scrolling past the top of the build menu
        if(current_index > max_index){ current_index = max_index; } //prevents scrolling past the bottom of the build menu
        
        //determine the current hovered-over menu item
        int selected_index = current_index + floor((input.window_mouse.y-(window.getSize().y-build_menu_height))/build_menu_item_thickness);
        if(input.window_mouse.x > build_menu_width){ selected_index = -1; } //if mouse not in menu along x axis set no item as selected

        //create the backdrop of the build menu
        window_rectangles.push_back(createRectangle(Vector2f((build_menu_width/2.0),window.getSize().y - build_menu_height +(build_menu_height/2.0)), Vector2f(build_menu_width,build_menu_height), outline_width, Color(15,15,15,205), Color(253,130,43,75))); //create the menu background of size width=far_right, height=far_bottom

        Vector2f anchor = top_left_anchor + Vector2f(margin,window.getSize().y - build_menu_height + character_size); //set anchor to the centre of top-left item in build menu to start
        
        if(selected_structure_id == ""){ //if no structure is selected then run the general build interface
            //generate the list items starting with the current index and moving up. The greater the index, the farther down the screen it will be.
            for(int index = current_index; index < build_list.size(); index++){

                if(index == selected_index){
                    window_captions.push_back(Caption(build_list[index], font_id, anchor, character_size, Color(255,255,255,225), "left")); //HIGHLIGHT and generate a left-oriented caption at the current value of the anchor
                    if(input.lmb_released){ //if mouse is clicked while highlighting a menu item then select that menu item for construction
                        if(build_list[index] == selected_type_id){
                            selected_type_id = ""; //if this item was previously selected then deselect now
                        }
                        else{
                            selected_type_id = build_list[index];
                        }
                    }
                }
                else{
                    window_captions.push_back(Caption(build_list[index], font_id, anchor, character_size, text_colour, "left")); //generate a left-oriented caption at the current value of the anchor
                }
                window_rectangles.push_back(createLine(anchor+Vector2f(0,character_size),Vector2f(-1,0),build_menu_width-(2*margin),text_colour)); //draw a line between this item and the next one
                anchor = anchor + Vector2f(0,build_menu_item_thickness); //shift the anchor down the screen by the item thickness
            }

            if(selected_type_id != ""){ //if there is a building type selected
                
                

                Sprite preview = createSprite(structure_properties[selected_type_id].texture_id,input.view_mouse); //generate a preview sprite of the structure's basic shape at the mouse location
                preview.move(0,distanceFromGround(preview.getGlobalBounds(), terrain));
                if(preview.getGlobalBounds().top + preview.getGlobalBounds().height > terrain.grid_ref.y){
                    preview.setPosition(preview.getPosition().x, terrain.grid_ref.y - (preview.getGlobalBounds().height/2.0));
                }

                //check if the preview is overlapping with any structures or the terrain
                bool preview_obstructed = isIntersectingTerrain(preview, terrain);
                if(!preview_obstructed){
                    for(map<string, Structure*>::iterator i = structures.begin(); i != structures.end(); i++){
                        if(isIntersecting(preview, *i->second)){
                            preview_obstructed = true;
                            break;
                        }
                    }
                }
                //

                //set the correct frame and colour (highlight in red if the preview is obstructed) 
                IntRect preview_window = getFrame(structure_properties[selected_type_id].start_index["default"], preview);
                preview.setTextureRect(preview_window);
                if(preview_obstructed){
                    preview.setColor(Color(155,5,5,155));
                }
                else{
                    preview.setColor(Color(125,125,225,155));
                }
                //
                
                //add the preview to the UI queue for rendering
                sprites.push_back(preview);
                //

                //act on input
                if(!preview_obstructed && input.lmb_released && input.window_mouse.x > build_menu_width){ //if the preview isn't overlapping anything and the mouse is outside the build menu and the left mouse button is clicked:
                    structures[createUniqueId()] = new Structure(selected_type_id, preview.getPosition()); //place a structure of the selected type at the mouse location
                    selected_type_id = ""; //reset the selected structure type
                }
                //
            }
        }
        else{ //if a structure is selected show the structure upgrade interface

            //generate the list items starting with the current index and moving up. The greater the index, the farther down the screen it will be.
            for(int index = current_index; index < build_list.size(); index++){

                if(index == selected_index){
                    window_captions.push_back(Caption(build_list[index], font_id, anchor, character_size, Color(255,255,255,225), "left")); //HIGHLIGHT and generate a left-oriented caption at the current value of the anchor
                    
                    Sprite preview = createSprite(structure_properties[build_list[index]].texture_id, Vector2f(structures[selected_structure_id]->sprite[structures[selected_structure_id]->type_name].getPosition().x, structures[selected_structure_id]->sprite[structures[selected_structure_id]->type_name].getPosition().y - ((getTextureRect(structure_properties[build_list[index]].texture_id).height - getTextureRect(structure_properties[structures[selected_structure_id]->type_name].texture_id).height)/2.0)));
                    
                    //check if the preview is overlapping with any structures or the terrain
                    bool preview_obstructed = isIntersectingTerrain(preview, terrain);
                    if(!preview_obstructed){
                        for(map<string, Structure*>::iterator i = structures.begin(); i != structures.end(); i++){
                            if(i->first == selected_structure_id){ continue; }
                            if(isIntersecting(preview, *i->second)){
                                preview_obstructed = true;
                                break;
                            }
                        }
                    }
                    //

                    IntRect preview_window = getFrame(structure_properties[build_list[index]].start_index["default"], preview);

                    preview.setTextureRect(preview_window);
                    if(preview_obstructed){
                        preview.setColor(Color(225,55,55,215));
                    }
                    else{
                        preview.setColor(Color(125,125,225,215));
                    }
                    //

                    sprites.push_back(preview);

                    if(input.lmb_released && !preview_obstructed){
                        structures[selected_structure_id]->upgrade(build_list[index]);
                        selected_structure_id = "";
                    }
                }
                else{
                    window_captions.push_back(Caption(build_list[index], font_id, anchor, character_size, text_colour, "left")); //generate a left-oriented caption at the current value of the anchor
                }
                window_rectangles.push_back(createLine(anchor+Vector2f(0,character_size),Vector2f(-1,0),build_menu_width-(2*margin),text_colour)); //draw a line between this item and the next one
                anchor = anchor + Vector2f(0,build_menu_item_thickness); //shift the anchor down the screen by the item thickness
            }
        }

    }
 
}
void VisualsStruct::draw(RenderWindow &window, View &world_view, View &window_view){

    window.setView(world_view);

    for(vector<RectangleShape>::iterator i = rectangles.begin(); i != rectangles.end(); i++){
        window.draw(*i);
    }
    for(vector<Caption>::iterator i = captions.begin(); i != captions.end(); i++){
        i->draw(window);
    }
    for(vector<Sprite>::iterator i = sprites.begin(); i != sprites.end(); i++){
        window.draw(*i);
    }

    window.setView(window_view);

    for(vector<RectangleShape>::iterator i = window_rectangles.begin(); i != window_rectangles.end(); i++){
        window.draw(*i);
    }
    for(vector<Caption>::iterator i = window_captions.begin(); i != window_captions.end(); i++){
        i->draw(window);
    }
    for(vector<Sprite>::iterator i = window_sprites.begin(); i != window_sprites.end(); i++){
        window.draw(*i);
    }
}

void scaleView(View &view, View &window_view, double delta){

	double current_scale = window_view.getSize().x/view.getSize().x; //returns fraction of full size objects will appear as
	current_scale += delta;
	if(current_scale < 0.15){ current_scale = 0.15; }
	if(current_scale > 1.5){ current_scale = 1.5; }
	double new_size_x = window_view.getSize().x/current_scale;
	double new_size_y = window_view.getSize().y/current_scale;
	view.setSize(new_size_x,new_size_y);

}

void translateView(View &view, View &window_view, double x, double y){

	double current_scale = window_view.getSize().x/view.getSize().x;
	view.move(x/current_scale,y/current_scale);
}
