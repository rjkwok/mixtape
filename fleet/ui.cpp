#include "main.h"

using namespace std;
using namespace sf;

extern RenderWindow window;


Tile::Tile(){}
Tile::Tile(double left, double bottom, double side_length, Texture &icon_tex, Color colour){

    bounds.width = side_length;
    bounds.height = side_length;
    bounds.left = left;
    bounds.top = bottom - side_length;

    frame.setSize(Vector2f(side_length,side_length));
    frame.setOutlineThickness(3);
    frame.setOutlineColor(colour);
    Color thinned_colour = colour;
    thinned_colour.a -= 100;
    frame.setFillColor(thinned_colour);
    frame.setOrigin(side_length/2.0,side_length/2.0);
    frame.setPosition(bounds.left + (bounds.width/2.0), bounds.top + (bounds.height/2.0));
    
    icon.setTexture(icon_tex, true);
    icon.setColor(Color(255,255,255,115));
    IntRect icon_rect = icon.getTextureRect();
    icon.setOrigin(icon_rect.width/2.0,icon_rect.height/2.0);
    double scale = (bounds.width/icon_rect.width);
    icon.setScale(scale,scale);
    icon.setPosition(bounds.left + (bounds.width/2.0), bounds.top + (bounds.height/2.0));

    is_text = false;
}

Tile::Tile(double left, double bottom, double side_length, string caption, Color colour){

    bounds.width = side_length;
    bounds.height = side_length;
    bounds.left = left;
    bounds.top = bottom - side_length;

    frame.setSize(Vector2f(side_length,side_length));
    frame.setOutlineThickness(3);
    frame.setOutlineColor(colour);
    Color thinned_colour = colour;
    thinned_colour.a -= 100;
    frame.setFillColor(thinned_colour);
    frame.setOrigin(side_length/2.0,side_length/2.0);
    frame.setPosition(bounds.left + (bounds.width/2.0), bounds.top + (bounds.height/2.0));
    
    text.setFont(*getFont("font1"));
    text.setCharacterSize(28);
    text.setColor(colour);
    text.setString(caption);
    text.setStyle(Text::Bold);
    FloatRect text_rect = text.getLocalBounds();
    text.setOrigin(text_rect.width/2.0,text_rect.height/2.0);
    text.setPosition(bounds.left + (bounds.width/2.0), bounds.top + (bounds.height/2.0));

    is_text = true;
}

bool Tile::contains(double x, double y){

    return bounds.contains(x, y);
}
void Tile::highlight(){

    if(is_text){
        text.setScale(1.1,1.1);
    }
    else{
        icon.setScale(icon.getScale().x*1.1,icon.getScale().y*1.1);
    }
    frame.setScale(1.1,1.1);
}

void Tile::draw(){

    window.draw(frame);
    if(is_text){
        window.draw(text);
    }
    else{
        window.draw(icon);
    }
}

UI::UI(){}

void UI::initialize(){

    mouse = Vector2f(0,0);
    fixed_mouse = Vector2i(0,0);
  //  is_deck_open = false;
 //  is_dead_open = false;

    score_text_a.setFont(*getFont("font1"));
    score_text_a.setCharacterSize(28);
    score_text_a.setColor(Color(253,130,43,155));
    score_text_a.setPosition((window.getSize().x/2.0)-80,10);
    score_text_a.setString("0");

    score_text_b.setFont(*getFont("font1"));
    score_text_b.setCharacterSize(28);
    score_text_b.setColor(Color(253,130,43,155));
    score_text_b.setPosition((window.getSize().x/2.0)+80,10);
    score_text_b.setString("0");

    total_score_text_a.setFont(*getFont("font1"));
    total_score_text_a.setCharacterSize(34);
    total_score_text_a.setColor(Color(0,155,155,155));
    total_score_text_a.setPosition(20,10);
    total_score_text_a.setString("0");

    total_score_text_b.setFont(*getFont("font1"));
    total_score_text_b.setCharacterSize(34);
    total_score_text_b.setColor(Color(0,155,155,155));
    total_score_text_b.setPosition(window.getSize().x - 60,10);
    total_score_text_b.setString("0");

    ship_info_text.setFont(*getFont("font1"));
    ship_info_text.setCharacterSize(22);
    ship_info_text.setColor(Color(0,155,0,155));
    ship_info_text.setString("");

    previous_mouse = Vector2f(0,0);
    mouse_delta = Vector2f(0,0);
}

Vector2f UI::getMouse(){

    return mouse;
}

Vector2i UI::getFixedMouse(){

    return fixed_mouse;
}

void UI::collect(){

    clearInput();

    sf::Event event;
    while (window.pollEvent(event))
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
				
				if(event.mouseButton.button == Mouse::Button::Left){ keys_released.insert("lmb"); }                
				if(event.mouseButton.button == Mouse::Button::Right){ keys_released.insert("rmb"); }             
				if(event.mouseButton.button == Mouse::Button::Middle){ keys_released.insert("mmb"); }             
                break;

            case sf::Event::MouseButtonPressed:
				
				if(event.mouseButton.button == Mouse::Button::Left){ keys_pressed.insert("lmb"); }                
				if(event.mouseButton.button == Mouse::Button::Right){ keys_pressed.insert("rmb"); }             
				if(event.mouseButton.button == Mouse::Button::Middle){ keys_pressed.insert("mmb"); }             
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

    if(Keyboard::isKeyPressed(Keyboard::Right)){ keys_pressed.insert("right"); }
    if(Keyboard::isKeyPressed(Keyboard::Left)){ keys_pressed.insert("left"); }
    if(Keyboard::isKeyPressed(Keyboard::Up)){ keys_pressed.insert("up"); }
    if(Keyboard::isKeyPressed(Keyboard::Down)){ keys_pressed.insert("down"); }
    if(Mouse::isButtonPressed(Mouse::Button::Left)){ keys_pressed.insert("lmb"); }
    if(Mouse::isButtonPressed(Mouse::Button::Right)){ keys_pressed.insert("rmb"); }

    mouse = window.mapPixelToCoords(Mouse::getPosition(window));
    fixed_mouse = Mouse::getPosition(window);
    
    mouse_delta = Vector2f(previous_mouse.x-fixed_mouse.x,previous_mouse.y-fixed_mouse.y);
    previous_mouse = Vector2f(fixed_mouse.x,fixed_mouse.y);
}

void UI::updateGUI(MatchClient* current_round){


    //arguments
    double side_length_hand = 100;
    double side_length_other = 60;
    int tiles_per_row = 4;
    Vector2f bottom_left_hand = Vector2f(10, window.getSize().y - 10); //only non-dependent vector2f for the menus
    //

    //determine whether to open, close, or leave the deck/dead tile menus
  /*  if(is_deck_open){
        is_deck_open = false;
        if(current_round->deck.size() > 0 && deck_bounds.contains(fixed_mouse.x,fixed_mouse.y)){
            is_deck_open = true;
        }
    }
    else{
        if(current_round->deck.size() > 0 && keyWasReleased("lmb") && deck_bounds.contains(fixed_mouse.x,fixed_mouse.y)){
            is_deck_open = true;
        }
        
    }
    if(is_dead_open){
        is_dead_open = false;
        if(current_round->discarded.size() > 0 && dead_bounds.contains(fixed_mouse.x,fixed_mouse.y)){
            is_dead_open = true;
        }
    }
    else{
        if(current_round->discarded.size() > 0 && keyWasReleased("lmb") && dead_bounds.contains(fixed_mouse.x,fixed_mouse.y)){
            is_dead_open = true;
        }
    }*/
    //

    //calculated
   // Vector2f bottom_left_deck = Vector2f(bottom_left_hand.x, (bottom_left_hand.y - side_length_hand)-20);
  //  int deck_rows = ceil(current_round->deck.size()/double(tiles_per_row));
   // Vector2f bottom_left_dead;
  //  if(is_deck_open){
  //      bottom_left_dead = Vector2f(bottom_left_deck.x, (bottom_left_deck.y - (deck_rows*(side_length_other+20))));
  //  }
  //  else{
  //      bottom_left_dead = Vector2f(bottom_left_deck.x, (bottom_left_deck.y - side_length_other)-20);
  //  }
    //

    //build tiles and resize active bounds
    tiles_hand.clear();
    tiles_for_sale.clear();
    texts.clear();
  //  tiles_deck.clear();
  //  tiles_dead.clear();

    double card_scale = 0.25;
    bottom_left_hand.x = (window.getSize().x/2.0) - (((card_scale*500)*2.5)+40);
    for(int index = 0; index < current_round->hand.size(); index++){
        if(current_round->hand[index] == NULL){
            CardGraphic new_graphic = CardGraphic("VOID", 0, Color(215,215,215,155), Color(215,215,215,155), Color(215,215,215,155));
            new_graphic.setScale(card_scale);
            new_graphic.setCenter(Vector2f(bottom_left_hand.x + (250*card_scale) + (((card_scale*500)+20)*index),bottom_left_hand.y - (card_scale*312)));
            tiles_hand.push_back(new_graphic);
        }
        else{
            CardGraphic new_graphic = CardGraphic(current_round->hand[index]->type_id, getProperties(current_round->hand[index]->type_id)->base_strength, Color(253,130,43,155), Color(215,215,215,155), Color(215,215,215,155));
            new_graphic.setScale(card_scale);
            new_graphic.setCenter(Vector2f(bottom_left_hand.x + (250*card_scale) + (((card_scale*500)+20)*index),bottom_left_hand.y - (card_scale*312)));
            tiles_hand.push_back(new_graphic);
        }
    }

    double for_sale_card_scale = card_scale * 1.00;
    bottom_left_hand.y -= (window.getSize().y * 0.8);
    for(int index = 0; index < 5; index++){
        if(current_round->for_sale[index] == NULL){
            CardGraphic new_graphic = CardGraphic("VOID", 0, Color(215,215,215,155), Color(215,215,215,155), Color(215,215,215,155), false);
            new_graphic.setScale(for_sale_card_scale);
            new_graphic.setCenter(Vector2f(bottom_left_hand.x + (250*for_sale_card_scale) + (((for_sale_card_scale*500)+20)*index),for_sale_card_scale*345));
            tiles_for_sale.push_back(new_graphic);
        }
        else{
            CardGraphic new_graphic = CardGraphic(current_round->for_sale[index]->type_id, getProperties(current_round->for_sale[index]->type_id)->base_strength, Color(253,130,43,155), Color(215,215,215,155), Color(215,215,215,155), false);
            new_graphic.setScale(for_sale_card_scale);
            new_graphic.setCenter(Vector2f(bottom_left_hand.x + (250*for_sale_card_scale) + (((for_sale_card_scale*500)+20)*index),for_sale_card_scale*345));
            tiles_for_sale.push_back(new_graphic);
        }
    }
   /* 
    if(is_deck_open){
        for(int index = 0; index < current_round->deck.size(); index++){
            Ship* ship = current_round->deck[index];
            tiles_deck.push_back(Tile(bottom_left_deck.x + ((index % tiles_per_row)*(side_length_other+20)), bottom_left_deck.y - ((side_length_other+20)*floor(double(index)/tiles_per_row)), side_length_other, *getProperties(ship->type_id)->thumb_tex, Color(253,130,43,155)));
        }

        deck_bounds.left = bottom_left_deck.x;
        deck_bounds.height = ((side_length_other+20)*ceil(double(current_round->deck.size())/tiles_per_row));
        deck_bounds.top = bottom_left_deck.y - deck_bounds.height;
        deck_bounds.width = tiles_per_row * (side_length_other+20);
        deck_bounds.width += 130;
        deck_bounds.height += 100;
        deck_bounds.top -= 50;
        deck_bounds.left -= 50;
    }
    else{
        tiles_deck.push_back(Tile(bottom_left_deck.x, bottom_left_deck.y, side_length_other, asString(double(current_round->deck.size())), Color(253,130,43,155)));
        
        deck_bounds.left = bottom_left_deck.x;
        deck_bounds.height = side_length_other;
        deck_bounds.top = bottom_left_deck.y - deck_bounds.height;
        deck_bounds.width = side_length_other;
    }
    
    if(is_dead_open){
        for(int index = 0; index < current_round->discarded.size(); index++){
            Ship* ship = current_round->discarded[index];
            tiles_dead.push_back(Tile(bottom_left_dead.x + ((index % tiles_per_row)*(side_length_other+20)), bottom_left_dead.y - ((side_length_other+20)*floor(double(index)/tiles_per_row)), side_length_other, *getProperties(ship->type_id)->thumb_tex, Color(40,40,40,155)));
        }

        dead_bounds.left = bottom_left_dead.x;
        dead_bounds.height = ((side_length_other+20)*ceil(double(current_round->discarded.size())/tiles_per_row));
        dead_bounds.top = bottom_left_dead.y - dead_bounds.height;
        dead_bounds.width = tiles_per_row * (side_length_other+20);
        dead_bounds.width += 130;
        dead_bounds.height += 100;
        dead_bounds.top -= 50;
        dead_bounds.left -= 50;
    }
    else{
        tiles_dead.push_back(Tile(bottom_left_dead.x, bottom_left_dead.y, side_length_other, asString(double(current_round->discarded.size())), Color(40,40,40,155)));

        dead_bounds.left = bottom_left_dead.x;
        dead_bounds.height = side_length_other;
        dead_bounds.top = bottom_left_dead.y - dead_bounds.height;
        dead_bounds.width = side_length_other;
    }*/
    //attack_button = Tile(window.getSize().x - 120, window.getSize().y - 10, 110, "ATTACK", Color(0,155,155,155));
    //

    //determine tiles being hovered over
    int selected_hand = -1;
    int selected_for_sale = -1;
  //  int selected_deck = -1;
  //  int selected_dead = -1;

    for(int index = 0; index < tiles_hand.size(); index++){
        if(tiles_hand[index].frame_sprite.getGlobalBounds().contains(fixed_mouse.x,fixed_mouse.y)){ 
            tiles_hand[index].setScale(card_scale * 1.1); 
            selected_hand = index;
        }
    }
    for(int index = 0; index < tiles_for_sale.size(); index++){
        if(tiles_for_sale[index].frame_sprite.getGlobalBounds().contains(fixed_mouse.x,fixed_mouse.y)){ 
            tiles_for_sale[index].setScale(for_sale_card_scale * 1.1); 
            selected_for_sale = index;
        }
        if(current_round->cooldown > 0){
            int time_display = ceil(current_round->cooldown);
            Text time_text;
            time_text.setFont(*getFont("font1"));
            time_text.setCharacterSize(72);
            time_text.setColor(Color(255,200,200,225));
            time_text.setString(asString(time_display));
            time_text.setOrigin(30,30);
            time_text.setPosition(tiles_for_sale[index].main_sprite.getPosition());
            Color frame_colour = tiles_for_sale[index].frame_sprite.getColor();
            Color sprite_colour = tiles_for_sale[index].main_sprite.getColor();
            frame_colour.a -= 80;
            sprite_colour.a -= 80;
            tiles_for_sale[index].frame_sprite.setColor(frame_colour);
            tiles_for_sale[index].main_sprite.setColor(sprite_colour);
            texts.push_back(time_text);
        }
    }
 /*   for(int index = 0; index < tiles_deck.size(); index++){
        if(tiles_deck[index].contains(fixed_mouse.x,fixed_mouse.y)){ 
            tiles_deck[index].highlight(); 
            selected_deck = index;
        }
    }
    for(int index = 0; index < tiles_dead.size(); index++){
        if(tiles_dead[index].contains(fixed_mouse.x,fixed_mouse.y)){ 
            tiles_dead[index].highlight(); 
            selected_dead = index;
        }
    }
    if(attack_button.contains(fixed_mouse.x,fixed_mouse.y)){
        attack_button.highlight();
    }*/
    //

    //determine if a socket is being hovered over
    string selected_socket = "";
    double socket_tolerance = 150;
    double closest_distance = 0;
    for(map<string, CardSocket*>::iterator i = current_round->sockets.begin(); i != current_round->sockets.end(); i++){
        
        double distance = hypot(getMouse().x-i->second->getCenter().x,getMouse().y-i->second->getCenter().y);
        if(distance < socket_tolerance && (selected_socket == "" || distance < closest_distance)){
            closest_distance = distance;
            selected_socket = i->first;
        }
        
    }
    //

    //if clicked with lmb process the swapping of round objects according to current round attributes
    if(keyWasReleased("lmb") && current_round->win_lose_tie == 0){

       // if(attack_button.contains(fixed_mouse.x,fixed_mouse.y)){
        //    current_round->engage();
        //}
        if(current_round->selected_ship){
            //check if it activated a socket
            if(selected_hand >= 0 && current_round->hand[selected_hand] != NULL){ //swap if hovering over slot
                Ship* temp = current_round->selected_ship;
                current_round->selected_ship = current_round->hand[selected_hand];
                current_round->hand[selected_hand] = temp;
            }
            else if(selected_socket != "" && current_round->ships[selected_socket] == NULL){
                current_round->placeShip(selected_socket, current_round->selected_ship);
                current_round->selected_ship = NULL;
            }
        }
        else{
            //check if it activated a hand_a slot
            if(selected_for_sale >= 0 && current_round->cooldown <= 0){
                Packet command;
                command << "buy ship" << selected_for_sale;
                current_round->server_socket.send(command);
            }
            if(selected_hand >= 0 && current_round->hand[selected_hand] != NULL){
                current_round->selected_ship = current_round->hand[selected_hand];
                current_round->hand[selected_hand] = current_round->hand.back();
                current_round->hand.pop_back();
            }   
        }
    }

    if(current_round->selected_ship){
        selected_ship_thumb.setTexture(*getProperties(current_round->selected_ship->type_id)->thumb_tex, true);
        IntRect selected_ship_thumb_rect = selected_ship_thumb.getTextureRect();
        selected_ship_thumb.setColor(Color(255,255,255,115));
        selected_ship_thumb.setOrigin(selected_ship_thumb_rect.width/2.0,selected_ship_thumb_rect.height/2.0);
        selected_ship_thumb.setPosition(fixed_mouse.x,fixed_mouse.y);
        //HERE ALL POSSIBLE PLACEMENTS FOR THIS SHIP SHOULD BE DETERMINED AND DISPLAYED
    }
    else{
        selected_ship_thumb.setTexture(*getTexture("blank"), true);
        IntRect selected_ship_thumb_rect = selected_ship_thumb.getTextureRect();
        selected_ship_thumb.setOrigin(selected_ship_thumb_rect.width/2.0,selected_ship_thumb_rect.height/2.0);
        selected_ship_thumb.setPosition(fixed_mouse.x,fixed_mouse.y);
    }

    //update score
    score_text_a.setString("");
    score_text_b.setString("");
    total_score_text_a.setString(asString(current_round->strength_1));
    total_score_text_b.setString(asString(current_round->strength_2));

    ship_info_text.setString("");
   /* for(map<int, map<int, Ship*> >::iterator j = current_round->ships.begin(); j != current_round->ships.end(); j++){
        for(map<int, Ship*>::iterator i = j->second.begin(); i != j->second.end(); i++){
            if(i->second != NULL){
                //if ship exists
                if(i->second->sprite.getGlobalBounds().contains(mouse.x,mouse.y)){
                    ship_info_text.setString(asString(i->second->strength));
                    if(i->second->strength > getProperties(i->second->type_id)->base_strength){
                        ship_info_text.setColor(Color(0,155,0,200));
                    }
                    if(i->second->strength < getProperties(i->second->type_id)->base_strength){
                        ship_info_text.setColor(Color(155,0,0,200));
                    }
                    if(i->second->strength == getProperties(i->second->type_id)->base_strength){
                        ship_info_text.setColor(Color(155,155,155,200));
                    }
                }
            }
        }
    }*/
    ship_info_text.setPosition(fixed_mouse.x + 10,fixed_mouse.y - 10);

    if(current_round->win_lose_tie != 0){

        Text victory_text;
        victory_text.setFont(*getFont("font1"));
        victory_text.setCharacterSize(100);
        victory_text.setColor(Color(185,255,185,200));
        victory_text.setStyle(Text::Bold);

        if(current_round->win_lose_tie == 1){
            victory_text.setString("You Win!");
        }
        else if(current_round->win_lose_tie == 2){
            victory_text.setString("You Lose");
        }
        else if(current_round->win_lose_tie == 3){
            victory_text.setString("It's a Tie!");
        }

        FloatRect bounds = victory_text.getLocalBounds();
        victory_text.setOrigin(bounds.width/2.0, bounds.height/2.0);
        victory_text.setPosition(window.getSize().x/2.0,window.getSize().y/2.0);
        texts.push_back(victory_text);
    }
}

void UI::clearInput(){

	keys_released.clear();
    keys_pressed.clear();
    mouse = Vector2f(0,0);
    fixed_mouse = Vector2i(0,0);
    mmb_delta = 0;
    text_entered.clear();
}

void UI::draw(){

    //draw tiles
   /* for(vector<Tile>::iterator i = tiles_dead.begin(); i != tiles_dead.end(); i++){
        i->draw();
    }
    for(vector<Tile>::iterator i = tiles_deck.begin(); i != tiles_deck.end(); i++){
        i->draw();
    }*/
    for(vector<CardGraphic>::iterator i = tiles_hand.begin(); i != tiles_hand.end(); i++){
        i->draw();
    }
    for(vector<CardGraphic>::iterator i = tiles_for_sale.begin(); i != tiles_for_sale.end(); i++){
        i->draw();
    }
    //
    for(vector<Text>::iterator i = texts.begin(); i != texts.end(); i++){
        window.draw(*i);
    }
   // attack_button.draw();

    //draw score
    window.draw(score_text_a);
    window.draw(score_text_b);

    window.draw(total_score_text_a);
    window.draw(total_score_text_b);

    window.draw(ship_info_text);
    //draw hovering ship icon
    window.draw(selected_ship_thumb);
}

bool UI::keyWasReleased(string key_id){

	if(keys_released.count(key_id) == 0){
		return false;
	}
	return true;
}

bool UI::keyIsPressed(string key_id){

	if(keys_pressed.count(key_id) == 0){
		return false;
	}
	return true;
}