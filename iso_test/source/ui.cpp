#include "main.h"

using namespace std;
using namespace sf;

extern RenderWindow window;
extern View view;
extern View window_view;
extern string view_direction;
extern map<string, SquareSprite*> square_sprites;
extern map<string, Building*> buildings;

InputStruct::InputStruct(){

	view_mouse = Vector2f(0,0);
	window_mouse = Vector2f(0,0);
	
	mmb_delta = 0;
	lmb_released = false;
	rmb_released = false;
	lmb_held = false;
	rmb_held = false;
}

void InputStruct::collect(){

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

    selected_id = "";
    selected_building = "";
    selected_face = 0;
    double selected_y;
    for(map<string, Building*>::iterator i = buildings.begin(); i != buildings.end(); i++){
    	for(set<string>::iterator j = i->second->members.begin(); j != i->second->members.end(); j++){
    		SquareSprite* s = square_sprites[*j];
    		if(selected_id == "" || s->getScreenY() >= selected_y){
	    		//check if view mouse intersects a face
	    		int face = s->faceIntersecting(view_mouse);
	    		if(face != 0){
	    			selected_id = *j;
	    			selected_face = face;
	    			selected_building = i->first;
	    			selected_y = s->getScreenY();
	    		}
	    	}
    	}
    }
}

VisualsStruct::VisualsStruct(){

	diamond_hl = DiamondShape();
	diamond_hl.setFillColor(Color(0,0,0,0));
	diamond_hl.setOutlineColor(Color(155,220,155,150));

	iso_hl = IsoSideShape();
	iso_hl.setFillColor(Color(0,0,0,0));
	iso_hl.setOutlineColor(Color(155,220,155,150));

	hl_type = 0;
}

void VisualsStruct::update(InputStruct &input){

	hl_type = 0;
	if(input.selected_id != ""){

		for(set<string>::iterator i = buildings[input.selected_building]->members.begin(); i != buildings[input.selected_building]->members.end(); i++){
			SquareSprite* s = square_sprites[*i];
			s->sprite.setColor(Color(200,200,255,255));
		}

		SquareSprite* selected = square_sprites[input.selected_id];
		if(input.selected_face == 5){
			hl_type = 1;
			diamond_hl.setHeight(2*selected->a);
			diamond_hl.setWidth(4*selected->a);
			diamond_hl.setOrigin(2*selected->a,selected->a);
			diamond_hl.setPosition(selected->sprite.getPosition().x, selected->sprite.getPosition().y - selected->height);		
			diamond_hl.setOutlineThickness(-10);
		}
		else{
			hl_type = 2;
			iso_hl.setOrigin(0,0);
			if(view_direction == "_ne"){
				if(input.selected_face == 4){ //left
					iso_hl.setSize(2*selected->a,selected->height);
					iso_hl.setPosition(selected->sprite.getPosition().x - (2*selected->a), selected->sprite.getPosition().y - selected->height);
				}
				else if(input.selected_face == 3){ //right
					iso_hl.setSize(-2*selected->a,selected->height);
					iso_hl.setPosition(selected->sprite.getPosition().x + (2*selected->a), selected->sprite.getPosition().y - selected->height);
				}
			}
			else if(view_direction == "_se"){
				if(input.selected_face == 1){ //left
					iso_hl.setSize(2*selected->a,selected->height);
					iso_hl.setPosition(selected->sprite.getPosition().x - (2*selected->a), selected->sprite.getPosition().y - selected->height);
				}
				else if(input.selected_face == 4){ //right
					iso_hl.setSize(-2*selected->a,selected->height);
					iso_hl.setPosition(selected->sprite.getPosition().x + (2*selected->a), selected->sprite.getPosition().y - selected->height);
				}
			}
			else if(view_direction == "_sw"){
				if(input.selected_face == 2){ //left
					iso_hl.setSize(2*selected->a,selected->height);
					iso_hl.setPosition(selected->sprite.getPosition().x - (2*selected->a), selected->sprite.getPosition().y - selected->height);
				}
				else if(input.selected_face == 1){ //right
					iso_hl.setSize(-2*selected->a,selected->height);
					iso_hl.setPosition(selected->sprite.getPosition().x + (2*selected->a), selected->sprite.getPosition().y - selected->height);
				}
			}
			else if(view_direction == "_nw"){
				if(input.selected_face == 3){ //left
					iso_hl.setSize(2*selected->a,selected->height);
					iso_hl.setPosition(selected->sprite.getPosition().x - (2*selected->a), selected->sprite.getPosition().y - selected->height);
				}
				else if(input.selected_face == 2){ //right
					iso_hl.setSize(-2*selected->a,selected->height);
					iso_hl.setPosition(selected->sprite.getPosition().x + (2*selected->a), selected->sprite.getPosition().y - selected->height);
				}
			}
			iso_hl.setOutlineThickness(-10);
		}
	}
}

void VisualsStruct::draw(){

	window.setView(view);
	if(hl_type == 1){
		window.draw(diamond_hl);
	}
	else if(hl_type == 2){
		window.draw(iso_hl);
	}
}

void scaleView(double delta){

	double current_scale = window_view.getSize().x/view.getSize().x; //returns fraction of full size objects will appear as
	current_scale += delta;
	if(current_scale < 0.15){ current_scale = 0.15; }
	if(current_scale > 1){ current_scale = 1; }
	view.setSize(window_view.getSize().x/current_scale,window_view.getSize().y/current_scale);

}

void translateView(double x, double y){

	view.move(x,y);
}

void rotateViewClockwise(){

	if(view_direction == "_ne"){
		view_direction = "_se";
	}
	else if(view_direction == "_se"){
		view_direction = "_sw";
	}
	else if(view_direction == "_sw"){
		view_direction = "_nw";
	}
	else if(view_direction == "_nw"){
		view_direction = "_ne";
	}
}

void rotateViewCounterClockwise(){

	if(view_direction == "_ne"){
		view_direction = "_nw";
	}
	else if(view_direction == "_nw"){
		view_direction = "_sw";
	}
	else if(view_direction == "_sw"){
		view_direction = "_se";
	}
	else if(view_direction == "_se"){
		view_direction = "_ne";
	}
}