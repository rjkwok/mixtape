#include "main.h"

using namespace std;
using namespace sf;

//GLOBAL DATA STRUCTURES currently declared globally as they are collections that are never altered or overwritten after initialization, and global declaration makes using things like texture resources easier
//A better, non-global solution could be used here
map<string, Texture*> textures; 
map<string, IntRect> texture_rects;
map<int, TileProperties> tile_properties;
map<string, StructureProperties> structure_properties;
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

    //initialize base variables (total workers can just be pulled from workers.size() when necessary (workers is defined below))
    int total_ammunition = 0; //"currency" style resource that can be spent into structures that consume it
    int total_fuel = 0; //"currency" style resource that can be spent into structures that consume it
    int total_cash = 0; //"currency" style resource that can be spent into structures that consume it
    int total_power = 0; //this is a measure of the total POSITIVE power contributed by each Structure in the structures map
    int total_supply = 1; //this is a measure of the total POSITIVE supply contributed by each Structure in the structures map
    int total_construction = 1; //this is a measure of the construction rate, which is the sume of contributions from each Structure in the structures map + 1 elementary rate
    int used_power = 0; //this is a measure of the total NEGATIVE power contributed by each Structure in the structures map
    int used_workers = 0; //this is a count of all the workers currently engaged in a task
    int used_supply = 0; //this is a measure of the total NEGATIVE supply contributed by each Structure in the structures map
    //

    //initialize world structures
    Terrain terrain = Terrain(Vector2f(0,window.getSize().y),64,1000,24); //Passed every time a function needs to work with the terrain. Encapsulates terrain limits, resolution, origin, mapping, and visuals.
    map<string, Sprite*> character_sprites; //holds the sprite of any moving NPC or player in the game
    map<string, Worker*> workers; //worker struct contains a pointer to a sprite in character_sprites, worker NPCs stored in this structure to run AI processes and also for easy counting
    map<string, Structure*> structures; //holds all "structures" which are non-moving user-placed entities in the game world that contribute to the base variables each tick
    vector<Sprite*> back_1; //stores all the decorations that will render on the 1st background layer
    vector<Sprite*> back_2; //stores all the decorations that will render on the 2nd background layer
    vector<Sprite*> back_3; //stores all the decorations that will render on the 3rd background layer
    //

    //drop in some ferns as placeholder "characters" for collision testing
    character_sprites["fern1"] = formatSprite(new Sprite(*textures["fern"]),200,-200);
    character_sprites["fern2"] = formatSprite(new Sprite(*textures["fern"]),500,-200);
    character_sprites["fern3"] = formatSprite(new Sprite(*textures["fern"]),1000,-200);
    //

    //assign the player pointer, which is used to send movement commands to the player as well as provide something for the cameras to track
    Sprite* player = character_sprites["fern1"];
    view.setCenter(player->getPosition().x,player->getPosition().y);
    //

	//terrain generation
    int top_index = 12; //starting point for the algorithm
    //this simple algorithm just keeps track of a "top index" up to which each column is filled with stone and dirt and grass. For each column iterated over there is a chance that the "top index" can increase or decrease by one.
    for(int index_x = 0; index_x < terrain.max_x; index_x++){

        if(index_x % randInt(3) == 0){ top_index += randSign(); } //random chance of having 1 added or subtracted from the "top index" for this column
        if(top_index > terrain.max_y){ top_index = terrain.max_y; } //don't let the top index exceed the bounds of the terrain
        if(top_index < 12){ top_index = 12; } //don't let the top index fall too low

        for(int index_y = top_index-7; index_y < top_index; index_y++){ //for the first 7 blocks under the top index, fill with grass and dirt
            if(index_y == top_index-1){ 
                terrain.grid[index_x][index_y] = 2; //grass only appears on the very top block
            }
            else{
                terrain.grid[index_x][index_y] = 14; //rest are dirt
            }
        }
        for(int index_y = 2; index_y < top_index-7; index_y++){ //for the remaining blocks deeper still, fill with stone (except the last two rows)
            terrain.grid[index_x][index_y] = 28;
        }
        for(int index_y = 0; index_y < 2; index_y++){ //last two rows are filled with "bedrock"
            terrain.grid[index_x][index_y] = 32;
        }
    }
	terrain.updateTiles(); //update the terrain visual quilt to reflect all the newly generated terrain
    //

    //generate the backdrop
	back_3.push_back(formatSprite(new Sprite(*getTexture("sky")),0,-7500.0 + (1.1*window.getSize().y),15000,15000)); 
    //

   	//main program loop
    Vector2f last_player_pos = player->getPosition(); //used to determine player movement vector across ticks, so that the camera can follow
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
    	if(ui_input.keys_held.count("a") != 0){
    		player->move(-1000*dt,0);
    	}
    	if(ui_input.keys_held.count("d") != 0){
    		player->move(1000*dt,0);
    	}
    	if(ui_input.keys_held.count("s") != 0){
    		player->move(0,1000*dt);
    	}
    	if(ui_input.keys_held.count("w") != 0){
    		player->move(0,-1000*dt);
    	}
        if(ui_input.keys_held.count("lshift") != 0){
            scaleView(view, window_view, 4*ui_input.mmb_delta*dt);
            scaleView(back_view_1, window_view, 4*ui_input.mmb_delta*dt);
            scaleView(back_view_2, window_view, 4*ui_input.mmb_delta*dt);
            scaleView(back_view_3, window_view, 4*ui_input.mmb_delta*dt);
        }
   		//

        //update structures
        for(map<string, Structure*>::iterator i = structures.begin(); i != structures.end(); i++){

            i->second->update(dt, total_construction, total_power - used_power);
        }
        //

        //update base statistics dependent on structures
        used_power = 0;
        used_workers = 0;
        used_supply = 0;
        total_power = 0;
        total_supply = 1;
        total_construction = 1;
        for(map<string, Structure*>::iterator i = structures.begin(); i != structures.end(); i++){

            if(i->second->contributing){
                //add contributions to total
                if(i->second->getPowerContribution() > 0){
                    total_power += i->second->getPowerContribution();
                }
                else{
                    used_power += abs(i->second->getPowerContribution());
                }
                if(i->second->getSupplyContribution() > 0){
                    total_supply += i->second->getSupplyContribution();
                }
                else{
                    used_supply += abs(i->second->getSupplyContribution());
                }
                total_construction += i->second->getConstructionContribution();
            }
            used_workers += i->second->tasked_workers.size();
        }
        //

        //process movement and collisions ... ALMOST NONE OF THIS COLLISION PROCESSING HAS BEEN OPTIMIZED AND WILL NEED TO BE REVISITED AT SOME POINT
    	for(map<string,Sprite*>::iterator i = character_sprites.begin(); i != character_sprites.end(); i++){

            //if the sprites has crossed either edge of the map, teleport it to the other side accordingly
    		if(ceil(i->second->getPosition().x) > (terrain.max_x*terrain.tile_size)){ i->second->setPosition(0,i->second->getPosition().y); }
    		if(floor(i->second->getPosition().x) < 0){ i->second->setPosition((terrain.max_x*terrain.tile_size),i->second->getPosition().y); }
    		//

            //check each character sprite against each other character sprite for collision
            for(map<string,Sprite*>::iterator j = i; j != character_sprites.end(); j++){
				if(j==i){continue;}

				if(isIntersecting(*i->second,*j->second,0)){
					untangleSprites(*i->second,*j->second);
				}
    		}
            //

            for(map<string, Structure*>::iterator j = structures.begin(); j != structures.end(); j++){
                for(map<string, Sprite>::iterator k = j->second->sprite.begin(); k != j->second->sprite.end(); k++){
                    if(isIntersecting(*i->second,k->second,0)){
                        untangleSprite(*i->second,k->second.getGlobalBounds());
                    }
                }
            }
    		
            keepSpriteOutOfTerrain(*i->second, terrain);
    	}
        //

        //move layers and camera to follow player
    	Vector2f player_disp = player->getPosition()-last_player_pos; //store last tick's player position in a variable outside the loop. Use after player is processed to determine how far the player moved and set this as that.
    	last_player_pos = player->getPosition();

    	view.move(player_disp.x,player_disp.y);
    	back_view_1.move(player_disp.x*back_speed_1,player_disp.y*back_speed_1);
    	back_view_2.move(player_disp.x*back_speed_2,player_disp.y*back_speed_2);
    	back_view_3.move(player_disp.x*back_speed_3,player_disp.y*back_speed_3);
        //

        //update HUD
    	ui_visuals.update(window, ui_input, terrain, structures, workers, total_ammunition, total_fuel, total_cash, total_power, total_supply, total_construction, workers.size(), used_power, used_workers, used_supply);
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
                i->second->draw(window);
            }
            view.move(terrain.max_x*-terrain.tile_size,0);
            window.setView(view);
        }
        if(window_right > terrain.max_x*terrain.tile_size){
            view.move(terrain.max_x*-terrain.tile_size,0);
            window.setView(view);
            for(map<string, Structure*>::iterator i = structures.begin(); i != structures.end(); i++){
                i->second->draw(window);
            }
            view.move(terrain.max_x*terrain.tile_size,0);
            window.setView(view);
        }
        for(map<string, Structure*>::iterator i = structures.begin(); i != structures.end(); i++){
            i->second->draw(window);
        }
    	
    	if(window_left < 0){
    		view.move(terrain.max_x*terrain.tile_size,0);
    		window.setView(view);
    		for(map<string,Sprite*>::iterator i = character_sprites.begin(); i != character_sprites.end(); i++){
	    		window.draw(*i->second);
	    	}
    		view.move(terrain.max_x*-terrain.tile_size,0);
    		window.setView(view);
    	}
    	if(window_right > terrain.max_x*terrain.tile_size){
    		view.move(terrain.max_x*-terrain.tile_size,0);
    		window.setView(view);
    		for(map<string,Sprite*>::iterator i = character_sprites.begin(); i != character_sprites.end(); i++){
	    		window.draw(*i->second);
	    	}
    		view.move(terrain.max_x*terrain.tile_size,0);
    		window.setView(view);
    	}
    	for(map<string,Sprite*>::iterator i = character_sprites.begin(); i != character_sprites.end(); i++){
    		window.draw(*i->second);
    	}

        //draw ui
    	window.setView(window_view);
    	ui_visuals.draw(window);
    	window.setView(view);
        //

    	//flush to screen
    	window.display();
    	//
    }
    //

    //cleanup memory before exit
    for(map<string, Sprite*>::iterator i = character_sprites.begin(); i != character_sprites.end(); i++){
        delete i->second;
    }
    for(map<string, Worker*>::iterator i = workers.begin(); i != workers.end(); i++){
        delete i->second;
    }
    for(map<string, Structure*>::iterator i = structures.begin(); i != structures.end(); i++){
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