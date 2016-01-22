#include "main.h"

using namespace std;
using namespace sf;

//GLOBAL DATA STRUCTURES currently declared globally as they are collections that are never altered or overwritten after initialization, and global declaration makes using things like texture resources easier
//A better, non-global solution could be used here
map<string, Texture*> textures; 
map<string, IntRect> texture_rects;
map<int, TileProperties> tile_properties;
map<string, StructureProperties> structure_properties;
map<string, ShipProperties> ship_properties;
map<string, CharacterProperties> character_properties;
set<int> collidable_terrain_types;
map<string, Font> fonts;
//


int main(){

	//seed the random
	srand(time(0));
	//

	//load textures, iso heights, rects from index file
	loadConfigs();
    fonts["arial"] = Font();
    fonts["arial"].loadFromFile("fonts/arial.ttf");

    fonts["font1"] = Font();
    fonts["font1"].loadFromFile("fonts/font1.ttf");
	//

	//open a window and initialize the starting views
    RenderWindow window;
    View view;
	window.create(VideoMode::getFullscreenModes()[0], "DEBUG", Style::Fullscreen); //create a fullscreen window
    window.setFramerateLimit(70); //limit fps so we don't hog resources
    view.reset(FloatRect(0,0,window.getSize().x,window.getSize().y)); //initialize a view from the window size
    View window_view = view; //fixed "window" view that doesn't shift with the player, used for drawing UI, and also as a reference with which to calculate the current scale of the other transformed views
    View back_view_1 = view; //1st background layer (renders third)
    View back_view_2 = view; //2nd background layer (renders second)
    View back_view_3 = view; //3rd background layer (renders first)
    double back_speed_1 = 1.0; //1st layer scroll speed fraction (for parallax effect)
    double back_speed_2 = 0.5; //2nd layer scroll speed fraction (for parallax effect)
    double back_speed_3 = 0.25; //3rd layer scroll speed fraction (for parallax effect)
    //

    //initialize ui modules
    InputStruct ui_input = InputStruct();
    VisualsStruct ui_visuals = VisualsStruct();
    //

    //initialize loop timer
    Clock timer;
    double dt = 0; 
   	//

    //initialize world structures
    Terrain terrain = Terrain(Vector2f(0,window.getSize().y),32,8000,48); //Passed every time a function needs to work with the terrain. Encapsulates terrain limits, resolution, origin, mapping, and visuals.
    map<string, Character*> characters; //holds the sprite of any moving NPC or player in the game
    map<string, Structure*> structures; //holds all "structures" which are non-moving user-placed entities in the game world that contribute to the base variables each tick
    map<string, Ship*> ships;
    vector<Sprite*> back_1; //stores all the decorations that will render on the 1st background layer
    vector<Sprite*> back_2; //stores all the decorations that will render on the 2nd background layer
    vector<Sprite*> back_3; //stores all the decorations that will render on the 3rd background layer
    //

    //drop in a character for the player to puppet
    characters["fern1"] = new Character("fern1", "Player", Vector2f(200,-200));
    //

    //assign the player pointer, which is used to send movement commands to the player as well as provide something for the cameras to track
    Character* player = characters["fern1"];
    view.setCenter(player->sprite.getPosition().x,player->sprite.getPosition().y);

    //drop in a ship for testing
    ships["Serenity"] = new Ship("Transport", Vector2f(400, -2600));
    ships["Serenity"]->fuel = 10000;
    //

	//terrain generation
    int top_index = 24; //starting point for the algorithm
    //this simple algorithm just keeps track of a "top index" up to which each column is filled with stone and dirt and grass. For each column iterated over there is a chance that the "top index" can increase or decrease by one.
    for(int index_x = 0; index_x < terrain.max_x; index_x++){

        if(index_x % randInt(3) == 0){ top_index += randInt(2)*randSign(); } //random chance of having 1 added or subtracted from the "top index" for this column
        if(top_index > terrain.max_y){ top_index = terrain.max_y; } //don't let the top index exceed the bounds of the terrain
        if(top_index < 24){ top_index = 24; } //don't let the top index fall too low

        for(int index_y = top_index-14; index_y < top_index; index_y++){ //for the first 7 blocks under the top index, fill with grass and dirt
            if(index_y == top_index-1){ 
                terrain.grid[index_x][index_y] = 2; //grass only appears on the very top block
            }
            else{
                terrain.grid[index_x][index_y] = 14; //rest are dirt
            }
        }
        for(int index_y = 4; index_y < top_index-14; index_y++){ //for the remaining blocks deeper still, fill with stone (except the last two rows)
            terrain.grid[index_x][index_y] = 28;
        }
        for(int index_y = 0; index_y < 4; index_y++){ //last two rows are filled with "bedrock"
            terrain.grid[index_x][index_y] = 32;
        }
    }
	terrain.updateTiles(); //update the terrain visual quilt to reflect all the newly generated terrain
    //

    //generate the backdrop
	back_3.push_back(createNewSprite("sky",Vector2f(0,-7500.0 + (1.1*window.getSize().y))));
    back_3[back_3.size()-1]->setTextureRect(IntRect(0,0,15000,15000)); 
    //

   	//main program loop
    Vector2f last_player_pos = player->sprite.getPosition(); //used to determine player movement vector across ticks, so that the camera can follow
    while(window.isOpen()){

    	//loop header
    	window.clear();
    	dt = timer.getElapsedTime().asSeconds();
    	if(dt == 0){ dt = 0.000000001; }
	    timer.restart();
	    //

	    // collect input
    	ui_input.collect(window, view, window_view); 
    	//

    	//act on input
    	if(ui_input.keys_released.count("esc") != 0){
    		window.close();
    	}
        if(player->ship_id == "" && ui_input.keys_released.count("space") != 0 && distanceFromGround(player->sprite.getGlobalBounds(), terrain) < terrain.tile_size/2.0){ //jump, but only if the player is close enough to a ground surface
            player->dy -= 900;
        }
    	if(player->ship_id == "" && ui_input.keys_held.count("a") != 0){ //user directly controls player velocity on the x axis
    		player->sprite.move(-1000*dt,0);
    	}
    	if(player->ship_id == "" && ui_input.keys_held.count("d") != 0){ //user directly controls player velocity on the x axis
    		player->sprite.move(1000*dt,0);
    	}
        if(ui_input.keys_held.count("lshift") != 0){
            scaleView(view, window_view, 4*ui_input.mmb_delta*dt);
            scaleView(back_view_1, window_view, 4*ui_input.mmb_delta*dt);
            scaleView(back_view_2, window_view, 4*ui_input.mmb_delta*dt);
            scaleView(back_view_3, window_view, 4*ui_input.mmb_delta*dt);
        }
   		//

        //update ships
        for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){

            if(i->first == player->ship_id){
                i->second->controlFromInput(ui_input);
            }
            i->second->update(dt, terrain);
        }
        //
        
        //update characters
        for(map<string, Character*>::iterator i = characters.begin(); i != characters.end(); i++){

            i->second->update(dt, terrain);
        }
        //

        //process collisions ... ALMOST NONE OF THIS COLLISION PROCESSING HAS BEEN OPTIMIZED AND WILL NEED TO BE REVISITED AT SOME POINT
    	for(map<string,Character*>::iterator i = characters.begin(); i != characters.end(); i++){

            //check each character sprite against each other character sprite for collision
            for(map<string,Character*>::iterator j = i; j != characters.end(); j++){
				if(j==i){continue;}

				if(isIntersecting(i->second->sprite,j->second->sprite,0)){
					untangleSprites(i->second->sprite,j->second->sprite);
				}
    		}
            //

            keepSpriteOutOfTerrain(i->second->sprite, terrain);
    	}
        for(map<string,Ship*>::iterator j = ships.begin(); j != ships.end(); j++){
            keepShipOutOfTerrain(*j->second, terrain, dt);
        }
        //

        //update character positions for characters who are inside ships
        for(map<string, Character*>::iterator i = characters.begin(); i != characters.end(); i++){

            i->second->moveWithShip(ships);
        }
        //

        //move layers and camera to follow player
    	Vector2f player_disp = player->sprite.getPosition()-last_player_pos; //store last tick's player position in a variable outside the loop. Use after player is processed to determine how far the player moved and set this as that.
    	last_player_pos = player->sprite.getPosition();
    	view.move(player_disp.x,player_disp.y);
    	back_view_1.move(player_disp.x*back_speed_1,player_disp.y*back_speed_1);
    	back_view_2.move(player_disp.x*back_speed_2,player_disp.y*back_speed_2);
    	back_view_3.move(player_disp.x*back_speed_3,player_disp.y*back_speed_3);
        //

        //update HUD
        ui_visuals.clear();
    	//select ship to pilot and/or eject from ship
        for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){
            if(i->second->bounds.contains(ui_input.view_mouse)){
                if(ui_input.lmb_released){
                    player->ship_id = i->first;
                }
            }
        }
        if(player->ship_id != "" && ui_input.keys_released.count("space") != 0){
            ships[player->ship_id]->antigrav_enabled = false;
            player->ship_id = "";
        }
        //
        //

    	//draw layers
    	window.setView(back_view_3);
    	for(vector<Sprite*>::iterator i = back_3.begin(); i != back_3.end(); i++){
    		Sprite* p = *i;
    		window.draw(*p);
    	}
    	window.setView(back_view_2);
    	for(vector<Sprite*>::iterator i = back_2.begin(); i != back_2.end(); i++){
    		Sprite* p = *i;
    		window.draw(*p);
    	}
    	window.setView(back_view_1);
    	for(vector<Sprite*>::iterator i = back_1.begin(); i != back_1.end(); i++){
    		Sprite* p = *i;
    		window.draw(*p);
    	}
    	
    	window.setView(view);
    	//draw regular things
    	double window_left = view.getCenter().x - (view.getSize().x/2.0);
    	double window_right = window_left + view.getSize().x;

        //if the view is past the left end of the map, jump the camera to the far right end of the map, take a picture and then jump back so that the right end of the map fills in the blank space past the left end.
    	if(window_left < 0){
    		view.move(terrain.max_x*terrain.tile_size,0);
    		window.setView(view);
    		terrain.draw(window);
    		view.move(terrain.max_x*-terrain.tile_size,0);
    		window.setView(view);
    	}
        //same as just above, but this fills in the blank space past the far right end of the map with the stuff at the left end
    	if(window_right > terrain.max_x*terrain.tile_size){
    		view.move(terrain.max_x*-terrain.tile_size,0);
    		window.setView(view);
    		terrain.draw(window);
    		view.move(terrain.max_x*terrain.tile_size,0);
    		window.setView(view);
    	}
    	terrain.draw(window); //draw the parts of the terrain in regular view

        if(window_left < 0){
            view.move(terrain.max_x*terrain.tile_size,0);
            window.setView(view);
            for(map<string, Structure*>::iterator i = structures.begin(); i != structures.end(); i++){
                i->second->drawExterior(window);
            }
            view.move(terrain.max_x*-terrain.tile_size,0);
            window.setView(view);
        }
        if(window_right > terrain.max_x*terrain.tile_size){
            view.move(terrain.max_x*-terrain.tile_size,0);
            window.setView(view);
            for(map<string, Structure*>::iterator i = structures.begin(); i != structures.end(); i++){
                i->second->drawExterior(window);
            }
            view.move(terrain.max_x*terrain.tile_size,0);
            window.setView(view);
        }
        for(map<string, Structure*>::iterator i = structures.begin(); i != structures.end(); i++){
            i->second->drawExterior(window);
        }
    	
        if(window_left < 0){
            view.move(terrain.max_x*terrain.tile_size,0);
            window.setView(view);
            for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){
                i->second->draw(window);
            }
            view.move(terrain.max_x*-terrain.tile_size,0);
            window.setView(view);
        }
        if(window_right > terrain.max_x*terrain.tile_size){
            view.move(terrain.max_x*-terrain.tile_size,0);
            window.setView(view);
            for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){
                i->second->draw(window);
            }
            view.move(terrain.max_x*terrain.tile_size,0);
            window.setView(view);
        }
        for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){
            i->second->draw(window);
        }

    	if(window_left < 0){
    		view.move(terrain.max_x*terrain.tile_size,0);
    		window.setView(view);
    		for(map<string,Character*>::iterator i = characters.begin(); i != characters.end(); i++){
                i->second->draw(window);
	    	}
    		view.move(terrain.max_x*-terrain.tile_size,0);
    		window.setView(view);
    	}
    	if(window_right > terrain.max_x*terrain.tile_size){
    		view.move(terrain.max_x*-terrain.tile_size,0);
    		window.setView(view);
    		for(map<string,Character*>::iterator i = characters.begin(); i != characters.end(); i++){
                i->second->draw(window);
            }
    		view.move(terrain.max_x*terrain.tile_size,0);
    		window.setView(view);
    	}
    	for(map<string,Character*>::iterator i = characters.begin(); i != characters.end(); i++){
            i->second->draw(window);
        }

        //draw ui
    	ui_visuals.draw(window, view, window_view);
    	window.setView(view);
        //

    	//flush to screen
    	window.display();
    	//
    }
    //

    //cleanup memory before exit
    for(map<string, Character*>::iterator i = characters.begin(); i != characters.end(); i++){
        delete i->second;
    }
    for(map<string, Structure*>::iterator i = structures.begin(); i != structures.end(); i++){
        delete i->second;
    }
    for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){
        delete i->second;
    }
    for(vector<Sprite*>::iterator i = back_3.begin(); i != back_3.end(); i++){
		delete *i;
	}
	for(vector<Sprite*>::iterator i = back_2.begin(); i != back_2.end(); i++){
		delete *i;
	}
	for(vector<Sprite*>::iterator i = back_1.begin(); i != back_1.end(); i++){
		delete *i;
	}	
    destroyTextures();
    //

	return 1;
}