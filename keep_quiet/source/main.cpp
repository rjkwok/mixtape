#include "main.h"

using namespace std;
using namespace sf;

//GLOBAL DATA STRUCTURES currently declared globally as they are collections that are never altered or overwritten after initialization, and global declaration makes using things like texture resources easier
//A better, non-global solution could be used here
map<string, Texture*> textures; 
map<string, IntRect> texture_rects;
map<int, TileProperties> tile_properties;
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
	window.create(VideoMode::getFullscreenModes()[0], "DEBUG", Style::Fullscreen);
    window.setFramerateLimit(70);
    view.reset(FloatRect(0,0,window.getSize().x,window.getSize().y));
    View window_view = view;
    View back_view_1 = view;
    View back_view_2 = view;
    View back_view_3 = view;
    //

    //initialize ui modules
    InputStruct ui_input = InputStruct();
    VisualsStruct ui_visuals = VisualsStruct();
    //

    //initialize loop timer
    Clock timer;
    double dt = 0;
   	//

    //initialize base variables
    int total_ammunition = 0;
    int total_fuel = 0;
    int total_cash = 0;

    int total_power = 0;
    int total_supply = 1;
    int total_construction = 1;
    //total workers can just be pulled from workers.size() when necessary (workers is defined below)
    
    int used_power = 0;
    int used_workers = 0;
    int used_supply = 0;
    //

    

    //initialize world structures
    map<int,map<int,int> > terrain; //0 is no terrain, 1 is dirt, 2 is tunnel support, etc.
    map<string, Sprite*> character_sprites;
    map<string, Worker*> workers; //worker struct contains a pointer to a sprite in character_sprites
    map<string, Structure*> structures;

    //

    character_sprites["fern1"] = formatSprite(new Sprite(*textures["fern"]),0,0);
    character_sprites["fern2"] = formatSprite(new Sprite(*textures["fern"]),500,0);
    character_sprites["fern3"] = formatSprite(new Sprite(*textures["fern"]),1000,0);

    Sprite* player = character_sprites["fern1"];

    view.setCenter(player->getPosition().x,player->getPosition().y);

    
	vector<Sprite*> back_1;
	vector<Sprite*> back_2;
	vector<Sprite*> back_3;
	double back_speed_1 = 1.0;
	double back_speed_2 = 0.5;
	double back_speed_3 = 0.25;

	
	int terrain_max_x = 1000;
	int terrain_max_y = 24;
	Vector2f grid_ref = Vector2f(0,window.getSize().y); //terrain grid bottom left origin

	//terrain generation
    // Grass Layer
   for(int index_y = 14; index_y < 15; index_y++){
       for(int index_x = 0; index_x < terrain_max_x; index_x++){
            terrain[index_x][index_y] = 2;
        }
    }
    // Dirt
	for(int index_y = 10; index_y < 14; index_y++){
		for(int index_x = 0; index_x < terrain_max_x; index_x++){
			terrain[index_x][index_y] = 14;
		}
	}
    // Stone
	for(int index_y = 2; index_y < 10; index_y++){
		for(int index_x =0; index_x < terrain_max_x; index_x++){
			terrain[index_x][index_y] = 28;
		}
	}
    // Bedrock
    for(int index_y = 0; index_y < 2; index_y++){
        for(int index_x = 0; index_x < terrain_max_x; index_x++){
            terrain[index_x][index_y] = 32;
        }
    }
    // Restart Block
  for(int index_y = 0; index_y < 14; index_y++){
        for(int index_x = 0; index_x < 1; index_x++){
            terrain[index_x][index_y] = 1;
        }
    }
	//

	VertexArray terrain_tiles(Quads,4); //only update on possible change to terrain

	terrain_tiles.resize(terrain_max_x*terrain_max_y*4); //resize to buildable grid (needs to be equal to total area in tiles, not pixels, and then multiplied by 4 to have 4 vertexes per tile)
	
	//terrain update loop
	for(int index_x = 0; index_x < terrain_max_x; index_x++){
		for(int index_y = 0; index_y < terrain_max_y; index_y++){
			//iterate over every tile
			int tile_type_index = terrain[index_x][index_y]; //used here to determine which part of the loaded texture sheet to show
			int texture_index = tile_properties[tile_type_index].texture_indexes[randInt(tile_properties[tile_type_index].texture_indexes.size())-1];
			int tiles_across = floor(textures["terrain"]->getSize().x/64.0);
			int u = texture_index % tiles_across;
			int v = floor(texture_index/tiles_across);

			sf::Vertex* tile = &terrain_tiles[4*(index_x + (index_y*terrain_max_x))]; //get a pointer to a location in the array defined by this tile's sequential position iterated left-to-right and then bottom-to-top
			//take this and the next 3 array elements as the corners of the tile, texture and position them accordingly

            tile[0].position = sf::Vector2f(grid_ref.x+(index_x*64.0), grid_ref.y-(index_y*64.0));
            tile[1].position = sf::Vector2f(grid_ref.x+((index_x + 1)*64.0), grid_ref.y-(index_y*64.0));
            tile[2].position = sf::Vector2f(grid_ref.x+((index_x + 1)*64.0), grid_ref.y-((index_y + 1)*64.0));
            tile[3].position = sf::Vector2f(grid_ref.x+(index_x*64.0), grid_ref.y-((index_y + 1)*64.0));

            tile[3].texCoords = sf::Vector2f(u*64.0, v*64.0);
            tile[2].texCoords = sf::Vector2f((u + 1)*64.0, v*64.0);
            tile[1].texCoords = sf::Vector2f((u + 1)*64.0, (v + 1)*64.0);
            tile[0].texCoords = sf::Vector2f(u*64.0, (v + 1)*64.0);
		}
	}
	//

	back_3.push_back(formatSprite(new Sprite(*getTexture("sky")),0,1080,15000,15000));

	Vector2f last_player_pos = player->getPosition();

   	//main program loop
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
    	scaleView(view, window_view, 4*ui_input.mmb_delta*dt);
        scaleView(back_view_1, window_view, 4*ui_input.mmb_delta*dt);
        scaleView(back_view_2, window_view, 4*ui_input.mmb_delta*dt);
        scaleView(back_view_3, window_view, 4*ui_input.mmb_delta*dt);
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
                if(i->second->power_contribution > 0){
                    total_power += i->second->power_contribution;
                }
                else{
                    used_power += abs(i->second->power_contribution);
                }
                if(i->second->supply_contribution > 0){
                    total_supply += i->second->supply_contribution;
                }
                else{
                    used_supply += abs(i->second->supply_contribution);
                }
                total_construction += i->second->power_contribution;
            }
            used_workers += i->second->tasked_workers.size();
        }
        //

        //process movement and collisions
    	for(map<string,Sprite*>::iterator i = character_sprites.begin(); i != character_sprites.end(); i++){

    		if(ceil(i->second->getPosition().x) > (terrain_max_x*64.0)){ i->second->setPosition(0,i->second->getPosition().y); }
    		if(floor(i->second->getPosition().x) < 0){ i->second->setPosition((terrain_max_x*64.0),i->second->getPosition().y); }
    		for(map<string,Sprite*>::iterator j = i; j != character_sprites.end(); j++){
				if(j==i){continue;}

				if(spritesIntersecting(*i->second,*j->second,0)){
					untangleSprites(*i->second,*j->second);
				}
    		}
    		
    	}
    	for(map<string,Sprite*>::iterator i = character_sprites.begin(); i != character_sprites.end(); i++){
    		keepSpriteOutOfTerrain(*i->second, grid_ref, terrain, terrain_max_x, terrain_max_y);
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
    	ui_visuals.update(window, ui_input, total_ammunition, total_fuel, total_cash, total_power, total_supply, total_construction, workers.size(), used_power, used_workers, used_supply);
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

    	if(window_left < 0){
    		view.move(terrain_max_x*64.0,0);
    		window.setView(view);
    		window.draw(terrain_tiles, textures["terrain"]);
    		view.move(terrain_max_x*-64.0,0);
    		window.setView(view);
    	}
    	if(window_right > terrain_max_x*64.0){
    		view.move(terrain_max_x*-64.0,0);
    		window.setView(view);
    		window.draw(terrain_tiles, textures["terrain"]);
    		view.move(terrain_max_x*64.0,0);
    		window.setView(view);
    	}
    	window.draw(terrain_tiles, textures["terrain"]);

        if(window_left < 0){
            view.move(terrain_max_x*64.0,0);
            window.setView(view);
            for(map<string, Structure*>::iterator i = structures.begin(); i != structures.end(); i++){
                i->second->draw(window);
            }
            view.move(terrain_max_x*-64.0,0);
            window.setView(view);
        }
        if(window_right > terrain_max_x*64.0){
            view.move(terrain_max_x*-64.0,0);
            window.setView(view);
            for(map<string, Structure*>::iterator i = structures.begin(); i != structures.end(); i++){
                i->second->draw(window);
            }
            view.move(terrain_max_x*64.0,0);
            window.setView(view);
        }
        for(map<string, Structure*>::iterator i = structures.begin(); i != structures.end(); i++){
            i->second->draw(window);
        }
    	
    	if(window_left < 0){
    		view.move(terrain_max_x*64.0,0);
    		window.setView(view);
    		for(map<string,Sprite*>::iterator i = character_sprites.begin(); i != character_sprites.end(); i++){
	    		window.draw(*i->second);
	    	}
    		view.move(terrain_max_x*-64.0,0);
    		window.setView(view);
    	}
    	if(window_right > terrain_max_x*64.0){
    		view.move(terrain_max_x*-64.0,0);
    		window.setView(view);
    		for(map<string,Sprite*>::iterator i = character_sprites.begin(); i != character_sprites.end(); i++){
	    		window.draw(*i->second);
	    	}
    		view.move(terrain_max_x*64.0,0);
    		window.setView(view);
    	}
    	for(map<string,Sprite*>::iterator i = character_sprites.begin(); i != character_sprites.end(); i++){
    		window.draw(*i->second);
    	}

    	window.setView(window_view);
    	//draw ui
    	ui_visuals.draw(window);
    	//
    	window.setView(view);

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