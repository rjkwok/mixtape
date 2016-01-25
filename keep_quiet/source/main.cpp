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
    View background_view = view; //background layer (renders fourth and shifts at unity speed on the y-axis)
    View clouds_view = view; //1st clouds layer (renders third)
    View sky_view = view; //sky layer (renders second, and never shifts on the x axis)
    View stars_view = view; //stars layer (renders first)
    double background_speed = 0.75; //background layer speed scroll fraction (for parallax effect)
    double clouds_speed = 0.5; //clouds layer scroll speed fraction (for parallax effect)
    double sky_speed = 0.25; //sky layer scroll speed fraction (for parallax effect) 
    double stars_speed = 0.1; //stars layer scroll speed fraction (for parallax effect)
    double scale = 1.0; //the scale is the same for all views except the window view
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
    vector<Sprite> foreground; //stores all the decorations that render in the foreground
    vector<Sprite> background; //stores all the decorations that render in the background layer
    vector<Sprite> clouds; //stores all the decorations that will render on the 1st clouds layer
    vector<RectangleShape> sky;
    VertexArray stars(Points, 0);
    //

    //drop in a character for the player to puppet
    characters["fern1"] = new Character("fern1", "Player", Vector2f(200,-1500));
    //

    //assign the player pointer, which is used to send movement commands to the player as well as provide something for the cameras to track
    Character* player = characters["fern1"];
    view.setCenter(player->sprite.getPosition().x,player->sprite.getPosition().y);

    //drop in a ship for testing
    ships["Serenity"] = new Ship("Transport", Vector2f(400, -2600));
    ships["Serenity"]->fuel = 10000;
    //

    //sky generation
    double atmosphere_height = 30000; //generation configuration property
    int atmosphere_resolution = 100; //generation configuration property
    Color atmosphere_colour = Color(181,255,255,255); //generation configuration property
    
    double atmosphere_opacity = 255; 
    int total_segments = ceil(atmosphere_height/atmosphere_resolution);
    for(int index = 0; index < total_segments; index++){
        sky.push_back(createRectangle(Vector2f(window.getSize().x/2.0,((window.getSize().y*4.0) - (atmosphere_resolution/2.0)) - (index*atmosphere_resolution)), Vector2f(window.getSize().x*8.0, atmosphere_resolution), 0, atmosphere_colour, Color(0,0,0,0)));
        atmosphere_opacity -= 255.0/total_segments;
        if(atmosphere_opacity < 0){ atmosphere_opacity = 0; }
        atmosphere_colour.a = atmosphere_opacity;
    }
    //

    //star generation
    int stars_height = 40000; //generation configuration property //upper bound that stars will be generated to
    int total_constellations = 8000; //generation configuration property //total groups of stars that will be randomly generated
    int max_stars_per_constellation = 16; //generation configuration property
    int max_star_spread = 1024; //generation configuration property //radius of each constellation 

    for(int index = 0; index < total_constellations; index++){

        int total_stars = randInt(max_stars_per_constellation);
        Vector2f constellation_origin = Vector2f(randInt(terrain.max_x)*terrain.tile_size,-randInt(1000)*(stars_height/1000.0));

        for(int star_index = 0; star_index < total_stars; star_index++){
            Vector2f star_position = constellation_origin + Vector2f(randSign()*randInt(max_star_spread),randSign()*randInt(max_star_spread));
            stars.append(Vertex(star_position, Color(255,255,255,240)));
        }
    }
   //

    //cloud generation
    int total_clouds = 200; //generation configuration property
    double clouds_height = 10000; //generation configuration property

    for(int index = 0; index < total_clouds; index++){

        clouds.push_back(createSprite("cloud", Vector2f(randInt(terrain.max_x)*terrain.tile_size,-1000-randInt(clouds_height-1000)),"middle"));
        clouds[clouds.size()-1].setScale(4.0,4.0);
    }
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

    //tree generation
    int total_trees = 800; //generation configuration property

    for(int index = 0; index < total_trees; index++){

        int tile_x = randInt(terrain.max_x-20)+10;

        Vector2f tree_position = Vector2f(terrain.grid_ref.x + (tile_x*terrain.tile_size),terrain.getSurfaceY(terrain.grid_ref.x + (tile_x*terrain.tile_size)) + 200);
        if(randInt(2) == 2){
            foreground.push_back(createSprite("tree", tree_position, "bottom middle"));
        }
        else{
            foreground.push_back(createSprite("tree_2", tree_position, "bottom middle"));
        }
    }
    for(int index = 0; index < total_trees; index++){

        int tile_x = randInt(terrain.max_x-20)+10;

        Vector2f tree_position = Vector2f(terrain.grid_ref.x + (tile_x*terrain.tile_size),400);
        if(randInt(2) == 2){
            background.push_back(createSprite("tree", tree_position, "bottom middle"));
        }
        else{
            background.push_back(createSprite("tree_2", tree_position, "bottom middle"));
        }
    }
    //TREES ON MULTIPLE PARALLAX LAYERS WOULD GIVE THE FOREST DEPTH
    //

    //spawn a base
    double command_center_y = terrain.getSurfaceY(1000);
    structures["Command Center"] = new Structure("Command Center", "Command Center", Vector2f(1000, command_center_y));
    for(int flat_x = structures["Command Center"]->exterior_sprite.getGlobalBounds().left; flat_x <= structures["Command Center"]->exterior_sprite.getGlobalBounds().left + structures["Command Center"]->exterior_sprite.getGlobalBounds().width; flat_x++){
        terrain.setSurfaceY(flat_x, command_center_y);
    }
    terrain.updateTiles();
    //

    timer.restart(); //timer must be restarted just before the game loop starts, or else dt will have a huge value for the first iteration (from the time it took to run the rest of main() above))
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
        if(player->ship_id == "" && (ui_input.keys_released.count("w") != 0 || ui_input.keys_held.count("w") != 0)){
            //check if the player is in front of a door, and if so, enter the building
            for(map<string,Structure*>::iterator i = structures.begin(); i != structures.end(); i++){
                if(i->second->door.getGlobalBounds().intersects(player->sprite.getGlobalBounds())){
                    player->structure_id = i->first;
                    break;
                }
            }
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

        //update HUD
        ui_visuals.clear();
    	//select ship to pilot and/or eject from ship
        if(player->ship_id == ""){
            for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){
                if(i->second->bounds.intersects(player->sprite.getGlobalBounds())){
                    ui_visuals.rectangles.push_back(createBoundingRectangle(i->second->bounds,Color(253,130,43,205)));
                    ui_visuals.captions.push_back(Caption("[ENTER] TO PILOT SHIP", "font1", Vector2f(i->second->bounds.left, i->second->bounds.top - 36), 36, Color(253,130,43,205), "left"));
                    if(ui_input.keys_released.count("enter")){
                        player->ship_id = i->first;     
                    }
                }
            }
        }
        else if(ui_input.keys_released.count("space") != 0){
            //this executes if player ship id is not blank and the user pressed the key to eject
            ships[player->ship_id]->antigrav_enabled = false;
            player->ship_id = "";
        }
        //

        //scale the view appropriately depending on wheher or not the player is inside a ship
        double target_scale = 0.8;
        if(player->ship_id != ""){
            target_scale = 0.25;
        }
        scale = scale + ((target_scale - scale)*4.0*dt); //instead of switching directly between scales, gradually scale towards the desired scale
        //update view scales
        view.setSize(window_view.getSize().x/scale,window_view.getSize().y/scale);
        background_view.setSize(window_view.getSize().x/scale,window_view.getSize().y/scale);
        clouds_view.setSize(window_view.getSize().x/scale,window_view.getSize().y/scale);
        sky_view.setSize(window_view.getSize().x/scale,window_view.getSize().y/scale);
        stars_view.setSize(window_view.getSize().x/scale,window_view.getSize().y/scale);
        //

        //move layers and camera to follow player
        view.setCenter(player->sprite.getPosition().x,player->sprite.getPosition().y);
        background_view.setCenter(player->sprite.getPosition().x*background_speed,player->sprite.getPosition().y);
        clouds_view.setCenter(player->sprite.getPosition().x*clouds_speed,player->sprite.getPosition().y*clouds_speed);
        sky_view.setCenter(window.getSize().x/2.0,player->sprite.getPosition().y*sky_speed);
        stars_view.setCenter(player->sprite.getPosition().x*stars_speed,player->sprite.getPosition().y*stars_speed);

        window.setView(view);
        if(window.mapCoordsToPixel(terrain.grid_ref).y <= window.getSize().y){ //if the botttom of the terrain is above the bottom of the screen

            double y_correction = (window.getSize().y - window.mapCoordsToPixel(terrain.grid_ref).y)/scale; //apply a displacement to the center of each view so as not to reveal below the bottom of the terrain
            view.setCenter(player->sprite.getPosition().x,player->sprite.getPosition().y - y_correction);
            background_view.setCenter(player->sprite.getPosition().x*background_speed,player->sprite.getPosition().y - y_correction);
            clouds_view.setCenter(player->sprite.getPosition().x*clouds_speed,(player->sprite.getPosition().y - y_correction)*clouds_speed);
            sky_view.setCenter(window.getSize().x/2.0,player->sprite.getPosition().y*sky_speed);
            stars_view.setCenter(player->sprite.getPosition().x*stars_speed,(player->sprite.getPosition().y - y_correction)*stars_speed);
        }
        //

    	//draw layers
        double window_left = view.getCenter().x - (view.getSize().x/2.0);
        double window_right = window_left + view.getSize().x;

        if(window_left < 0){
            stars_view.move(terrain.max_x*terrain.tile_size,0);
            window.setView(stars_view);
            window.draw(stars);
            stars_view.move(terrain.max_x*-terrain.tile_size,0);
            window.setView(stars_view);
        }
        if(window_right > terrain.max_x*terrain.tile_size){
            stars_view.move(terrain.max_x*-terrain.tile_size,0);
            window.setView(stars_view);
            window.draw(stars);
            stars_view.move(terrain.max_x*terrain.tile_size,0);
            window.setView(stars_view);
        }
        window.setView(stars_view);
    	window.draw(stars);

    	window.setView(sky_view);
    	for(vector<RectangleShape>::iterator i = sky.begin(); i != sky.end(); i++){
            window.draw(*i);
        }

        if(window_left < 0){ //if the view is past the left end of the map, jump the camera to the far right end of the map, take a picture and then jump back so that the right end of the map fills in the blank space past the left end.
            clouds_view.move(terrain.max_x*terrain.tile_size,0);
            window.setView(clouds_view);
            for(vector<Sprite>::iterator i = clouds.begin(); i != clouds.end(); i++){
                window.draw(*i);
            }
            clouds_view.move(terrain.max_x*-terrain.tile_size,0);
            window.setView(clouds_view);
        }
        if(window_right > terrain.max_x*terrain.tile_size){ //same as just above, but this fills in the blank space past the far right end of the map with the stuff at the left end
            clouds_view.move(terrain.max_x*-terrain.tile_size,0);
            window.setView(clouds_view);
            for(vector<Sprite>::iterator i = clouds.begin(); i != clouds.end(); i++){
                window.draw(*i);
            }
            clouds_view.move(terrain.max_x*terrain.tile_size,0);
            window.setView(clouds_view);
        }
    	window.setView(clouds_view);
    	for(vector<Sprite>::iterator i = clouds.begin(); i != clouds.end(); i++){
    		window.draw(*i);
    	}

        if(window_left < 0){
            view.move(terrain.max_x*terrain.tile_size,0);
            window.setView(background_view);
             for(vector<Sprite>::iterator i = background.begin(); i != background.end(); i++){
                window.draw(*i);
            }
            view.move(terrain.max_x*-terrain.tile_size,0);
            window.setView(background_view);
        }
        if(window_right > terrain.max_x*terrain.tile_size){
            view.move(terrain.max_x*-terrain.tile_size,0);
            window.setView(background_view);
             for(vector<Sprite>::iterator i = background.begin(); i != background.end(); i++){
                window.draw(*i);
            }
            view.move(terrain.max_x*terrain.tile_size,0);
            window.setView(background_view);
        }
        window.setView(background_view);
        for(vector<Sprite>::iterator i = background.begin(); i != background.end(); i++){
            window.draw(*i);
        }

        if(window_left < 0){
            view.move(terrain.max_x*terrain.tile_size,0);
            window.setView(view);
             for(vector<Sprite>::iterator i = foreground.begin(); i != foreground.end(); i++){
                window.draw(*i);
            }
            view.move(terrain.max_x*-terrain.tile_size,0);
            window.setView(view);
        }
        if(window_right > terrain.max_x*terrain.tile_size){
            view.move(terrain.max_x*-terrain.tile_size,0);
            window.setView(view);
             for(vector<Sprite>::iterator i = foreground.begin(); i != foreground.end(); i++){
                window.draw(*i);
            }
            view.move(terrain.max_x*terrain.tile_size,0);
            window.setView(view);
        }
        window.setView(view);
        for(vector<Sprite>::iterator i = foreground.begin(); i != foreground.end(); i++){
            window.draw(*i);
        }
        
    	if(window_left < 0){
    		view.move(terrain.max_x*terrain.tile_size,0);
    		window.setView(view);
    		terrain.draw(window);
    		view.move(terrain.max_x*-terrain.tile_size,0);
    		window.setView(view);
    	}
    	if(window_right > terrain.max_x*terrain.tile_size){
    		view.move(terrain.max_x*-terrain.tile_size,0);
    		window.setView(view);
    		terrain.draw(window);
    		view.move(terrain.max_x*terrain.tile_size,0);
    		window.setView(view);
    	}
        window.setView(view);
    	terrain.draw(window); 

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
        window.setView(view);
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
        window.setView(view);
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
        window.setView(view);
    	for(map<string,Character*>::iterator i = characters.begin(); i != characters.end(); i++){
            i->second->draw(window);
        }
        //

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
    destroyTextures();
    //

	return 1;
}