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

            case sf::Event::KeyReleased:
                if(event.key.code == Keyboard::Up){
                    keys_released.insert("up");
                }
                else if(event.key.code == Keyboard::Down){
                    keys_released.insert("down");
                }
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

}
void VisualsStruct::clear(){

    //reset the visuals
    captions.clear();
    rectangles.clear();
    sprites.clear();
    window_captions.clear();
    window_rectangles.clear();
    window_sprites.clear();
    //
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
