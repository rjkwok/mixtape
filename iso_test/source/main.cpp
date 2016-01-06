#include "main.h"

using namespace std;
using namespace sf;

RenderWindow window;
View view; //scales up and down, moves side-to-side at player command
string view_direction;
View window_view; //doesn't change

map<string, Texture*> textures;
map<string, IntRect> texture_rects;
map<string, double> iso_heights;

map<string, Building*> buildings; //a building or construct wrapper struct references groups of square sprites to give them special properties and interactions
//map<string, Construct*> constructs;
//map<string, Entity*> entities; //dyn ents (ie people, only ever single squares) are a class extension in their own structure, to provide extra functionality, with reduced pointers in the square sprites structure to keep them in the proper draw order 
map<string, SquareSprite*> square_sprites; //contains the basic decorations, dyn sprites, and sprites that buildings and constructs reference - used for draw iteration/collision.
//basic decoration entities that cannot be selected but still get drawn and factored into collision can be added just directly to the square sprites map
vector<SquareSprite*> ground; //still just basic decorations, but need to be in separate structure for separate drawing. no keys since no wrappers will ever reference ground squares

int main(){

	//seed the random
	srand(time(0));
	//

	//load textures, iso heights, rects from index file
	loadConfigs();
	//

	//open a window and initialize the starting views
	window.create(VideoMode::getFullscreenModes()[0], "ASHES", Style::Fullscreen);
    window.setFramerateLimit(70);
    view.reset(FloatRect(0,0,window.getSize().x,window.getSize().y));
    view_direction = "_ne";
    window_view = view;
    //

    //setup background
    Sprite background;
    background.setTextureRect(IntRect(0,0,20000,20000));
    background.setOrigin(10000,10000);
    background.setPosition(0,0);
    string tx_background = "Urban";

    //initialize ui modules
    InputStruct ui_input = InputStruct();
    VisualsStruct ui_visuals = VisualsStruct();
    //

  	///TESTING SPRITES
    square_sprites["test1"] = new SquareSprite("Import Shop", 0, 30, 0);
    square_sprites["test2"] = new SquareSprite("Mini Market", 128, 30, 0);
    square_sprites["test3"] = new SquareSprite("Import Shop", 256, 30, 0);

    square_sprites["test4"] = new SquareSprite("Mini Market", 0, 384, 0);
    square_sprites["test5"] = new SquareSprite("Mini Market", 128, 384, 0);
    square_sprites["test6"] = new SquareSprite("Import Shop", 256, 384, 0);

    buildings["International Imports"] = new Building();
    buildings["International Imports"]->addMember("test6");

    buildings["International Imports A"] = new Building();
    buildings["International Imports A"]->addMember("test5");
    buildings["International Imports A"]->addMember("test4");

    buildings["International Imports B"] = new Building();
    buildings["International Imports B"]->addMember("test1");
    buildings["International Imports B"]->addMember("test2");
    buildings["International Imports B"]->addMember("test3");

    ground.push_back(new SquareSprite("Road",64,192,0));
    ground.push_back(new SquareSprite("Road",320,192,0));
    ////////////

    //initialize loop timer
    Clock timer;
    double dt = 0;
   	//

   	//main program loop
    while(window.isOpen()){

    	//loop header
    	window.clear();
    	dt = timer.getElapsedTime().asSeconds();
    	if(dt == 0){ dt = 0.000000001; }
	    timer.restart();
	    //

	    // collect input
    	ui_input.collect(); 
    	//

    	//act on input
    	scaleView(4*ui_input.mmb_delta*dt);
    	if(ui_input.keys_released.count("esc") != 0){
    		window.close();
    	}
    	if(ui_input.keys_released.count("q") != 0){
    		rotateViewClockwise();
    	}
    	if(ui_input.keys_released.count("e") != 0){
    		rotateViewCounterClockwise();
    	}
    	if(ui_input.keys_held.count("a") != 0){
    		translateView(-500*dt,0);
    	}
    	if(ui_input.keys_held.count("d") != 0){
    		translateView(500*dt,0);
    	}
    	if(ui_input.keys_held.count("w") != 0){
    		translateView(0,-500*dt);
    	}
    	if(ui_input.keys_held.count("s") != 0){
    		translateView(0,500*dt);
    	}
    	//

    	//update the background
    	background.setTexture(*getTexture(tx_background + view_direction), false);
    	//

    	//update, and calculate uppermost sprite y-values to set the lower bound for the draw iteration
    	double min_y = 0;
    	for(map<string, SquareSprite*>::iterator i = square_sprites.begin(); i != square_sprites.end(); i++){
    		i->second->update(dt);
    		if(i->second->getScreenY() < min_y){
    			min_y = i->second->getScreenY();
    		}
    		
    	}
    	double ground_min_y = 0;
    	for(vector<SquareSprite*>::iterator i = ground.begin(); i != ground.end(); i++){
    		SquareSprite* s = *i;
    		s->update(dt);
    		if(s->getScreenY() < ground_min_y){
    			ground_min_y = s->getScreenY();
    		}
    		
    	}
    	//

    	//update the ui visuals
    	ui_visuals.update(ui_input);
    	//

    	window.setView(view);

    	//draw ground layers
    	window.draw(background);
    	double current_ground_max_y = ground_min_y;
    	double ground_step_size = 100;
    	set<int> ground_already_drawn;
    	while(ground_already_drawn.size() != ground.size()){
    		for(int index = 0; index < ground.size(); index++){
    			if(ground_already_drawn.count(index) == 0 && ground[index]->getScreenY() <= current_ground_max_y){
    				ground[index]->draw();
    				ground_already_drawn.insert(index);
    			}
	    	}
	    	current_ground_max_y += ground_step_size;
    	}
    	
    	//

    	//draw iso sprites
    	double current_max_y = min_y;
    	double step_size = 100;
    	set<string> already_drawn;
    	while(already_drawn.size() != square_sprites.size()){

    		for(map<string, SquareSprite*>::iterator i = square_sprites.begin(); i != square_sprites.end(); i++){
    			if(already_drawn.count(i->first) == 0 && i->second->getScreenY() <= current_max_y){
    				i->second->draw();
    				already_drawn.insert(i->first);
    			}
    		}
    		current_max_y += step_size;
    	}
		//

		//draw ui
    	ui_visuals.draw();
    	//

    	//flush to screen
    	window.display();
    	//
    }
    //

    //cleanup memory before exit
    for(map<string, SquareSprite*>::iterator i = square_sprites.begin(); i != square_sprites.end(); i++){
    	delete i->second;
    }
    for(map<string, Building*>::iterator i = buildings.begin(); i != buildings.end(); i++){
    	delete i->second;
    }
    for(vector<SquareSprite*>::iterator i = ground.begin(); i != ground.end(); i++){
    	delete *i;
    }
    destroyTextures();
    //

	return 1;
}