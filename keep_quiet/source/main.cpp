#include "main.h"

using namespace std;
using namespace sf;

RenderWindow window;
View view;
View window_view;
View back_view_1;
View back_view_2;
View back_view_3;

map<string, Texture*> textures;
map<string, IntRect> texture_rects;
map<int, TileProperties> tile_properties;
map<string, Sprite*> entities;
map<int,map<int,int> > terrain; //0 is no terrain, 1 is dirt, 2 is tunnel support, etc.
set<int> collidable_terrain_types;
int terrain_max_x;
int terrain_max_y;
Vector2f grid_ref;

int main(){

	//seed the random
	srand(time(0));
	//

	//load textures, iso heights, rects from index file
	loadConfigs();
	//

	//open a window and initialize the starting views
	window.create(VideoMode::getFullscreenModes()[0], "DEBUG", Style::Fullscreen);
    window.setFramerateLimit(70);
    view.reset(FloatRect(0,0,window.getSize().x,window.getSize().y));
    window_view = view;
    //

    //initialize ui modules
    InputStruct ui_input = InputStruct();
    VisualsStruct ui_visuals = VisualsStruct();
    //

    //initialize loop timer
    Clock timer;
    double dt = 0;
   	//

    entities["fern1"] = formatSprite(new Sprite(*textures["fern"]),0,0);
    entities["fern2"] = formatSprite(new Sprite(*textures["fern"]),500,0);
    entities["fern3"] = formatSprite(new Sprite(*textures["fern"]),1000,0);

    Sprite* player = entities["fern1"];

    view.setCenter(player->getPosition().x,player->getPosition().y);

    back_view_1 = view;
	back_view_2 = view;
	back_view_3 = view;
	vector<Sprite*> back_1;
	vector<Sprite*> back_2;
	vector<Sprite*> back_3;
	double back_speed_1 = 1.0;
	double back_speed_2 = 0.5;
	double back_speed_3 = 0.25;

	
	terrain_max_x = 360;
	terrain_max_y = 24;
	grid_ref = Vector2f(0,window.getSize().y); //terrain grid bottom left origin

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
    	ui_input.collect(); 
    	//

    	//act on input
    	scaleView(4*ui_input.mmb_delta*dt);
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

    
    	for(map<string,Sprite*>::iterator i = entities.begin(); i != entities.end(); i++){

    		if(ceil(i->second->getPosition().x) > (terrain_max_x*64.0)){ i->second->setPosition(0,i->second->getPosition().y); }
    		if(floor(i->second->getPosition().x) < 0){ i->second->setPosition((terrain_max_x*64.0),i->second->getPosition().y); }
    		for(map<string,Sprite*>::iterator j = i; j != entities.end(); j++){
				if(j==i){continue;}

				if(spritesIntersecting(*i->second,*j->second,0)){
					untangleSprites(*i->second,*j->second);
				}
    		}
    		
    	}
    	for(map<string,Sprite*>::iterator i = entities.begin(); i != entities.end(); i++){
    		keepSpriteOutOfTerrain(*i->second);
    	}

    	Vector2f player_disp = player->getPosition()-last_player_pos; //store last tick's player position in a variable outside the loop. Use after player is processed to determine how far the player moved and set this as that.
    	last_player_pos = player->getPosition();

    	view.move(player_disp.x,player_disp.y);
    	back_view_1.move(player_disp.x*back_speed_1,player_disp.y*back_speed_1);
    	back_view_2.move(player_disp.x*back_speed_2,player_disp.y*back_speed_2);
    	back_view_3.move(player_disp.x*back_speed_3,player_disp.y*back_speed_3);

    	ui_visuals.update(ui_input);

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
    		for(map<string,Sprite*>::iterator i = entities.begin(); i != entities.end(); i++){
	    		window.draw(*i->second);
	    	}
    		view.move(terrain_max_x*-64.0,0);
    		window.setView(view);
    	}
    	if(window_right > terrain_max_x*64.0){
    		view.move(terrain_max_x*-64.0,0);
    		window.setView(view);
    		for(map<string,Sprite*>::iterator i = entities.begin(); i != entities.end(); i++){
	    		window.draw(*i->second);
	    	}
    		view.move(terrain_max_x*64.0,0);
    		window.setView(view);
    	}
    	for(map<string,Sprite*>::iterator i = entities.begin(); i != entities.end(); i++){
    		window.draw(*i->second);
    	}
    	window.setView(window_view);
    	//draw ui
    	ui_visuals.draw();
    	//
    	window.setView(view);

    	//flush to screen
    	window.display();
    	//
    }
    //

    //cleanup memory before exit
    delete player;
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