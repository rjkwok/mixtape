#include "main.h"

using namespace std;
using namespace sf;

extern RenderWindow window;
extern UI ui;

CardSocket::CardSocket(){}
CardSocket::CardSocket(string c_id, Vector2f c_point, double c_spin){

	id = c_id;
	point = c_point;
	spin = c_spin;

	linked[0] = "";
	linked[1] = "";
	linked[2] = "";
}
Vector2f CardSocket::getCenter(){

	return point;
}
int CardSocket::firstFree(){

	if(linked[0] == ""){
		return 0;
	}
	if(linked[1] == ""){
		return 1;
	}
	if(linked[2] == ""){
		return 2;
	}
}

MatchClient::MatchClient(){}

MatchClient::MatchClient(string c_username){

	username = c_username;
	match_in_progress = false;
	score_1 = 0;
	score_2 = 0;
	strength_1 = 0;
	strength_2 = 0;
	turn = 1;

	cooldown = 0;

	for(int i = 0; i < 5; i++){
		for_sale[i] = NULL;
	}
	selected_ship = NULL;
	//these SHOULD be parameters, not constants
	core_tx = "core_junker";

	win_lose_tie = 0;
}

MatchClient::~MatchClient(){

	for(int index = 0; index < 5; index++){
		delete for_sale[index];
	}
	for(vector<Ship*>::iterator i = hand.begin(); i != hand.end(); i++){
		delete *i;
	}
	for(vector<Ship*>::iterator i = discarded.begin(); i != discarded.end(); i++){
		delete *i;
	}
	for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){
		delete i->second;
	}
	for(map<string, CardSocket*>::iterator i = sockets.begin(); i != sockets.end(); i++){
		delete i->second;
	}
	delete selected_ship;
}

void MatchClient::process(double dt){

	if(win_lose_tie != 0){

		return;
	}

	if(server_socket.getRemoteAddress() != IpAddress::None){
		if(match_in_progress){
			//auto-draw whenever one's hand has a gap
			//for(int index = 0; index < 5; index++){
			//	if(hand[index] == NULL){
					//replace empty with another ship if any are left
			//		if(deck.size() > 0){
			//			int card_to_draw = rand() % deck.size();
			//			hand[index] = deck[card_to_draw];
			//			deck[card_to_draw] = deck[deck.size()-1];
			//			deck.pop_back();
			//		}
			//	}
			//}

			cooldown -= dt;
			if(cooldown < 0){ cooldown = 0; }
	
			card_graphics.clear();
			anchors.clear();
			texts.clear();
			//animate and position sprite according to ship map
			for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){

				Sprite anchor_sprite;
				anchor_sprite.setTexture(*getTexture("anchor"),true);
				anchor_sprite.setOrigin(250,312);
				anchor_sprite.setPosition(sockets[i->first]->getCenter().x,sockets[i->first]->getCenter().y);
				anchors.push_back(anchor_sprite);

			    if(i->second){

			    	Color card_colour = Color(215,215,215,255);
					Color name_colour = Color(215,215,215,155);
					Color strength_colour = Color(215,215,215,155);

					if(i->second->getOwner() != 0 && i->second->getOwner() == player_id){
						card_colour = Color(115,225,115,255);
				    }
				    else if(i->second->getOwner() != 0){
				    	card_colour = Color(225,115,115,255);
				    }

				    if(i->second->strength > i->second->getBase()){
				    	strength_colour = Color(115,225,115,155);
				    }
				    if(i->second->strength < i->second->getBase()){
				    	strength_colour = Color(225,115,115,155);
				    }

					CardGraphic new_graphic = CardGraphic(i->second->type_id, i->second->strength, card_colour, strength_colour, name_colour);
					new_graphic.setCenter(sockets[i->first]->getCenter());
					card_graphics.push_back(new_graphic);

				    if(new_graphic.frame_sprite.getGlobalBounds().contains(ui.getMouse().x,ui.getMouse().y)){
				    	double displace = 0;
				    	for(set<string>::iterator j = i->second->getAbilities()->begin(); j != i->second->getAbilities()->end(); j++){
				    		Text contrib;
						    contrib.setFont(*getFont("font1"));
						    contrib.setCharacterSize(36);
						    contrib.setColor(Color(185,215,185,185));
						    contrib.setString(*j);
						    contrib.setOrigin(0,18);
						    contrib.setPosition(ui.getMouse().x,ui.getMouse().y + displace);
						    displace += 42.0;
						    texts.push_back(contrib);
				    	}
				    	
				    }
			
			    }
			    
				/*if(i->second){

					
					Vector2f target = Vector2f(sockets[i->first]->getCenter().x,sockets[i->first]->getCenter().y);
					Vector2f origin = i->second->sprite.getPosition();
					Vector2f dist = target - origin;
					double h = hypot(dist.x,dist.y);
					if(h < 100.0){
						i->second->sprite.setPosition(target);
						continue;
					}
					Vector2f unit_vector = Vector2f(dist.x/h,dist.y/h);
					double speed = 3000;
					Vector2f disp = Vector2f(unit_vector.x*speed*dt,unit_vector.y*speed*dt);
					i->second->sprite.setPosition(i->second->sprite.getPosition() + disp);
					
				}	*/
			
			}
		}
		
		listenForServerCommand();
	}

}

void MatchClient::drawBackground(){
	window.draw(background);
}

void MatchClient::draw(){

	/*
	window.draw(core_sprite_1);
	window.draw(core_sprite_2);
*/
	for(vector<Sprite>::iterator i = anchors.begin(); i != anchors.end(); i++){
		window.draw(*i);
	}
	for(vector<CardGraphic>::iterator i = card_graphics.begin(); i != card_graphics.end(); i++){
		i->draw();
	}
	for(vector<Text>::iterator i = texts.begin(); i != texts.end(); i++){
		window.draw(*i);
	}
	/*for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){
		
		if(i->second){
			window.draw(i->second->sprite);
		}	
		
	}*/
	
}

bool MatchClient::joinMatch(string ip){

	server_socket.setBlocking(true);
	server_socket.connect(ip, 53000);
	Packet reply;
	server_socket.receive(reply);
	reply >> player_id;
	server_socket.setBlocking(false);
}

void MatchClient::initalizeMatch(string background_tx, string other_core_tx){ //called on command by server

	//setup basic graphic things
	getTexture(background_tx)->setRepeated(true);
	background.setTexture(*getTexture(background_tx),false);
	IntRect window_bounds;
	window_bounds.width = window.getSize().x;
	window_bounds.height = window.getSize().y;
	background.setTextureRect(window_bounds);
	background.setPosition(0,0);
/*
	core_sprite_1.setTexture(*getTexture(core_tx),true);
	IntRect core_sprite_1_rect = core_sprite_1.getTextureRect();
	core_sprite_1.setOrigin(core_sprite_1_rect.width,core_sprite_1_rect.height/2.0);
	core_sprite_1.setPosition(0,450);

	core_sprite_2.setTexture(*getTexture(other_core_tx),true);
	IntRect core_sprite_2_rect = core_sprite_2.getTextureRect();
	core_sprite_2_rect.left = core_sprite_2_rect.width;
	core_sprite_2_rect.width *= -1;
	core_sprite_2.setTextureRect(core_sprite_2_rect);
	core_sprite_2.setOrigin(0,core_sprite_2_rect.height/2.0);
	core_sprite_2.setPosition(5500,450);
*/
	//reset hand and deck
	for(int index = 0; index < 5; index++){
		delete for_sale[index];
	}
	for(vector<Ship*>::iterator i = hand.begin(); i != hand.end(); i++){
		delete *i;
	}
	for(vector<Ship*>::iterator i = discarded.begin(); i != discarded.end(); i++){
		delete *i;
	}
	for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){
		delete i->second;
	}
	for(map<string, CardSocket*>::iterator i = sockets.begin(); i != sockets.end(); i++){
		delete i->second;
	}
	delete selected_ship;

	for(int index = 0; index < 5; index++){
		for_sale[index] = NULL;
	}
	hand.clear();
	discarded.clear();
	selected_ship = NULL;

	//load deck and hand (from some sort of server or file, in future)
	//for(int i = 0; i < 20; i++){
	//	deck.push_back(randomNewCard(player_id));
	//}

	//generate blank map
	sockets["start_1"] = new CardSocket("start_1", Vector2f(0,0), 0);
	sockets["start_2"] = new CardSocket("start_2", Vector2f(5196,0), 0);
	for(map<string, CardSocket*>::iterator i = sockets.begin(); i != sockets.end(); i++){

		ships[i->first] = NULL;
		
	}

	//tell the server I'm ready
	Packet ready_packet;
	ready_packet << 1;
	server_socket.send(ready_packet);
	//cout << "Sent a packet to say I'm ready.\n";
}

void MatchClient::listenForServerCommand(){

	server_socket.setBlocking(false);
	Packet command;
	while(server_socket.receive(command) == Socket::Done){

		//if command received
		string command_str;
		command >> command_str;
		if(command_str == "send player info"){
			//send username + core txid
			sendPlayerInfo();
		}
		else if(command_str == "final update"){
			int winner;
			command >> winner;
			if(winner == player_id){
				//win
				win_lose_tie = 1;
			}
			else if(winner == 0){
				//tie
				win_lose_tie = 3;
			}
			else{
				//lost
				win_lose_tie = 2;
			}
		}
		else if(command_str == "add ship"){
			//add a new ship if the correct one isn't already there
			//cout << "Was told to add ship.\n";
			string id;
			int owner;
			string type_id;
			command >> id >> owner >> type_id;
			addShip(id,owner,type_id);
		}
		else if(command_str == "remove ship"){
			//cout << "Was told to remove ship.\n";
			string id;
			command >> id;
			removeShip(id);
		}
		else if(command_str == "change owner"){
			string id;
			int new_owner;
			command >> id >> new_owner;
			ships[id]->owner = new_owner;
		}
		else if(command_str == "remove socket"){

			string id;
			command >> id;
			removeSocket(id);
		}
		else if(command_str == "take card"){

			int slot_id;
			command >> slot_id;
			hand.push_back(for_sale[slot_id]);
			for_sale[slot_id]->owner = player_id;
		}
		else if(command_str == "add socket"){
			string id;
			double x;
			double y;
			double spin;
			command >> id >> x >> y >> spin;
			addSocket(id,x,y,spin);
		}
		else if(command_str == "unlink socket"){
			string id;
			command >> id;
			sockets[id]->linked[0] = "";
			sockets[id]->linked[1] = "";
			sockets[id]->linked[2] = "";
		}
		else if(command_str == "link sockets"){
			string a;
			string b;
			command >> a >> b;
			sockets[a]->linked[sockets[a]->firstFree()] = b;
			sockets[b]->linked[sockets[b]->firstFree()] = a;
		}
		else if(command_str == "initialize"){

			string background_tx;
			string other_core_tx;
			command >> background_tx >> enemy_username >> other_core_tx;
			initalizeMatch(background_tx, other_core_tx);
		}
		else if(command_str == "post for sale"){
			int slot_id;
			string type_id;
			command >> slot_id >> type_id;

			if(type_id != ""){
				for_sale[slot_id] = new Ship(0, type_id);
			}
			else{
				for_sale[slot_id] = NULL;
			}
			
		}
		else if(command_str == "begin"){
			//cout << "Was told to begin.\n";
			match_in_progress = true;
		}
		else if(command_str == "update numbers"){
			//cout << "Was told to update numbers.\n";
			int sc1;
			int sc2;
			int st1;
			int st2;
			double c;
			command >> c >> sc1 >> sc2 >> st1 >> st2;
			updateNumbers(c, sc1, sc2, st1, st2);
		}
		else if(command_str == "update strengths"){
			//cout << "Was told to update strengths.\n";
			string id;
			while(command >> id){
				command >> ships[id]->strength;
			}
		}
	}
}


void MatchClient::sendPlayerInfo(){

	Packet player_info;
	player_info << username << core_tx;
	server_socket.send(player_info);
	//cout << "Sent my info.\n";
}

void MatchClient::removeShip(string id){

	if(ships[id]->owner == player_id){
		discarded.push_back(ships[id]);
	}
	else{
		delete ships[id];
	}
	ships[id] = NULL;
}

void MatchClient::removeSocket(string id){

	delete sockets[id];
	if(ships[id] != NULL){
		removeShip(id);
	}
	sockets.erase(id);
	ships.erase(id);
}

void MatchClient::addShip(string id, int owner, string type_id){

	if(ships[id] != NULL){
		if(ships[id]->type_id != type_id || ships[id]->owner != owner){
			//if wrong ship exists
			removeShip(id);
			placeShip(id, new Ship(owner, type_id));
		}
	}
	else{
		placeShip(id, new Ship(owner, type_id), false);
	}

}


void MatchClient::addSocket(string id, double x, double y, double spin){

	sockets[id] = new CardSocket(id, Vector2f(x,y), spin);
	ships[id] = NULL;
}


void MatchClient::placeShip(string id, Ship* ship, bool should_notify){

	//basic
	ships[id] = ship;
	
	/*if(ship->owner == 1){
		ships[id]->sprite.setPosition(0 - 1000, 0);
		ships[id]->sprite.setScale(1.0,1.0);
	}
	if(ship->owner == 2){
		ships[id]->sprite.setPosition(5100 + 1000, 0);
		ships[id]->sprite.setScale(-1.0,1.0);
	}*/
	//

	if(should_notify){

		Packet command;
		command << "place ship" << id << ship->type_id;
		server_socket.send(command);
		//cout << "Sent a command to the server to place a ship at " << i << " " << j << " of type " << ship->type_id << endl;
	}
}

void MatchClient::engage(){
	Packet command;
	command << "engage";
	server_socket.send(command);
	//cout << "Sent a command to the server to engage.\n";
}

void MatchClient::updateNumbers(double c, int sc1, int sc2, int st1, int st2){

	cooldown = c;
	score_1 = sc1;
	score_2 = sc2;
	strength_1 = st1;
	strength_2 = st2;

}


///
/// SERVER
///


MatchServer::MatchServer(){

	//setup network ports
	listener.setBlocking(false);
	client_1.setBlocking(false);
	client_2.setBlocking(false);
	match_in_progress = false;

	for(int i = 0; i < 5; i++){

		for_sale[i] = NULL;
	}

	match_finished = false;
}

MatchServer::~MatchServer(){

	for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){
		delete i->second;
	}
	for(vector<Ship*>::iterator i = stock.begin(); i != stock.end(); i++){
		delete *i;
	}
	for(int i = 0; i < 5; i++){

		delete for_sale[i];
	}
	for(map<string, CardSocket*>::iterator i = sockets.begin(); i != sockets.end(); i++){
		delete i->second;
	}
}

void MatchServer::changeTurn(){

	if(turn == 1){
		turn = 2;
	}
	else if(turn == 2){
		turn = 1;
	}


}

void MatchServer::buyShip(int slot_id, int c_owner){

	if(c_owner == 1){

		Packet command;
		command << "take card" << slot_id;
		client_1.send(command);
		for_sale[slot_id] = NULL;
		cooldown_1 = 6;
	}
	else if(c_owner == 2){

		Packet command;
		command << "take card" << slot_id;
		client_2.send(command);
		for_sale[slot_id] = NULL;
		cooldown_2 = 6;
	}

	Packet command;
	command << "post for sale" << slot_id << "";
	client_1.send(command);
	client_2.send(command);

	Packet score_update_1;
	score_update_1 << "update numbers" << cooldown_1 << score_1 << score_2 << strength_1 << strength_2;
	client_1.send(score_update_1);
	//cout << "Sent a command to client 1 to update its turn and score values.\n";
	Packet score_update_2;
	score_update_2 << "update numbers" << cooldown_2 << score_1 << score_2 << strength_1 << strength_2;
	client_2.send(score_update_2);
	//cout << "Sent a command to client 1 to update its turn and score values.\n";
}
void MatchServer::updateStrengths(){

	//for each ship placed, calculate individual strength and add to attack total
	strength_1 = 0;
	strength_2 = 0;
	Packet strengths;
	strengths << "update strengths";
	for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){
		if(i->second){
			double sum_ship_strength = i->second->strength;
			if(i->second->getOwner() == 1){
				strength_1 += sum_ship_strength;
			}
			if(i->second->getOwner() == 2){
				strength_2 += sum_ship_strength;
			}
			strengths << i->first << i->second->strength;
		}	
	}

	client_1.send(strengths);
	//cout << "Sent a command to client 1 to update its strengths in its map.\n";
	client_2.send(strengths);
	//cout << "Sent a command to client 2 to update its strengths in its map.\n";
}

void MatchServer::process(double dt){

	if(match_in_progress){
		//assumes a map of all possible ship positions has been initialized
		//whose turn? switch
		
		if(time_left > 0){
			for(int i = 0; i < 5; i++){

				if(for_sale[i] == NULL && stock.size() != 0){
					//attempt to replace
					int random_index = rand() % stock.size();
					for_sale[i] = stock[random_index];
					Packet command;
					command << "post for sale" << i << for_sale[i]->type_id;
					client_1.send(command);
					client_2.send(command);
					stock[random_index] = stock.back();
					stock.pop_back();
				}
			}

			cooldown_1 -= dt;
			cooldown_2 -= dt;
			time_left -= dt;

			if(cooldown_1 < 0){ cooldown_1 = 0;}
			if(cooldown_2 < 0){ cooldown_2 = 0;}

			listenForClientCommand(1);
			listenForClientCommand(2);
			calculateStrengths(); //does this even need to be called here?

			if(client_1.getRemoteAddress() == IpAddress::None || client_2.getRemoteAddress() == IpAddress::None){
				active = false; //cleanly crash the server if one of the clients disappears
			}
		}
		else if(!match_finished){
			if(strength_1 > strength_2){
				Packet final_update;
				final_update << "final update" << 1;
				client_1.send(final_update);
				client_2.send(final_update);
			}
			else if(strength_2 > strength_1){
				Packet final_update;
				final_update << "final update" << 2;
				client_1.send(final_update);
				client_2.send(final_update);
			}
			else if(strength_1 == strength_2){
				Packet final_update;
				final_update << "final update" << 0;
				client_1.send(final_update);
				client_2.send(final_update);
			}
			match_finished = true;
		}
		
	}
	else{ //if match is not in progress

		//AFTER INITIAL HANDSHAKES, THERE SHOULD NEVER BE UNEXPECTED PACKETS SENT BACK AND FORTH
		if(client_1.getRemoteAddress() != IpAddress::None && client_2.getRemoteAddress() != IpAddress::None){
			cout << "match is being initialized...\n";
			initalizeMatch();
			cout << "match is initialized.\n";
		}
		else{
			listener.listen(53000);
			if(client_1.getRemoteAddress() == IpAddress::None){
				if(listener.accept(client_1) == Socket::Done){
					//if successfully connects
					client_1.setBlocking(true);
					Packet player_id;
					player_id << 1;
					client_1.send(player_id);
					client_1.setBlocking(false);
					cout << "Player 1 is connected.\n";
				}
			}
			else if(client_2.getRemoteAddress() == IpAddress::None){
				if(listener.accept(client_2) == Socket::Done){
					//if successfully connects
					client_2.setBlocking(true);
					Packet player_id;
					player_id << 2;
					client_2.send(player_id);
					client_2.setBlocking(false);
					cout << "Player 2 is connected\n";
				}
			}

		}
	}
}

void MatchServer::calculateStrengths(){


	for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){

		if(i->second){ //if a ship exists here

			/* neutral ships
			if(i->second->owner == 0){
				i->second->strengths[0] = 0;
				i->second->strengths[1] = 0;
				i->second->strengths[2] = 0;
				if(ships.count(sockets[i->first]->linked[0]) != 0 && ships[sockets[i->first]->linked[0]] != NULL && ships[sockets[i->first]->linked[0]]->getOwner() != 0){
					i->second->owner = ships[sockets[i->first]->linked[0]]->getOwner();
					Packet command;
					command << "change owner" << i->first << ships[sockets[i->first]->linked[0]]->getOwner();
					client_1.send(command);
					client_2.send(command);
				}
				else if(ships.count(sockets[i->first]->linked[1]) != 0 && ships[sockets[i->first]->linked[1]] != NULL && ships[sockets[i->first]->linked[1]]->getOwner() != 0){
					i->second->owner = ships[sockets[i->first]->linked[1]]->getOwner();
					Packet command;
					command << "change owner" << i->first << ships[sockets[i->first]->linked[1]]->getOwner();
					client_1.send(command);
					client_2.send(command);
				}
				else if(ships.count(sockets[i->first]->linked[2]) != 0 && ships[sockets[i->first]->linked[2]] != NULL && ships[sockets[i->first]->linked[2]]->getOwner() != 0){
					i->second->owner = ships[sockets[i->first]->linked[2]]->getOwner();
					Packet command;
					command << "change owner" << i->first << ships[sockets[i->first]->linked[2]]->getOwner();
					client_1.send(command);
					client_2.send(command);
				}
				continue;
			}*/
			
			determineModifications(i->first);

			if(i->second->nullify_strength == false){
				i->second->strength = i->second->getBase();
				for(vector<double>::iterator j = i->second->additions.begin(); j != i->second->additions.end(); j++){
					i->second->strength += *j;
				}
				if(i->second->support_blocked == false){
					for(vector<double>::iterator j = i->second->support_additions.begin(); j != i->second->support_additions.end(); j++){
						i->second->strength += *j;
					}
				}
				for(vector<double>::iterator j = i->second->multiplications.begin(); j != i->second->multiplications.end(); j++){
					i->second->strength *= *j;
				}
				if(i->second->support_blocked == false){
					for(vector<double>::iterator j = i->second->support_multiplications.begin(); j != i->second->support_multiplications.end(); j++){
						i->second->strength *= *j;
					}
				}

				if(i->second->strength < 0){
					i->second->strength = 0;
				}
			}
			else{
				i->second->strength = 0;
			}
		}
	}	
}

void MatchServer::engage(){

	//execute a "battle"
	//play lots of pretty battle animations
	//all ships removed from board and discarded
	//player with higher score has the difference added to his or her total score
	/*
	if(strength_1 > strength_2){
		score_1 += (strength_1-strength_2);
	}
	if(strength_2 > strength_1){
		score_2 += (strength_2-strength_1);
	}

	set<string> to_remove;
	set<string> to_branch;
	for(map<string, CardSocket*>::iterator i = sockets.begin(); i != sockets.end(); i++){


		if(i->first == "origin" || (ships[i->first] != NULL && getProperties(ships[i->first]->type_id)->special)){
			
			i->second->linked[0] = "";
			i->second->linked[1] = "";
			i->second->linked[2] = "";
			
			Packet command;
			command << "unlink socket" << i->first;
			client_1.send(command);
			client_2.send(command);
			if(ships[i->first] != NULL && getProperties(ships[i->first]->type_id)->special){
				to_branch.insert(i->first);
			}
			continue;
		}
		delete i->second;
		to_remove.insert(i->first);
	}
	for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){
		if(i->second && !getProperties(i->second->type_id)->special){ 
			//command clients to update with the removal of this ship
			Packet command;
			command << "remove ship" << i->first;
			client_1.send(command);
			//cout << "Sent a command to client 1 to remove a ship.\n";
			client_2.send(command);
			//cout << "Sent a command to client 2 to remove a ship.\n";

			delete i->second;
			i->second = NULL;
		}
		else if(i->second){
			//is station
			i->second->owner = 0;
			Packet command;
			command << "change owner" << i->first << 0;
			client_1.send(command);
			client_2.send(command);
		}	
	}
	for(set<string>::iterator i = to_remove.begin(); i != to_remove.end(); i++){
		Packet command;
		command << "remove socket" << *i;
		client_1.send(command);
		client_2.send(command);
		sockets.erase(*i);
		ships.erase(*i);
	}
	for(set<string>::iterator i = to_branch.begin(); i != to_branch.end(); i++){
		branchSocket(*i);
	}


	calculateStrengths();
	updateStrengths();
	*/
}

void MatchServer::placeShip(string id, Ship* ship){

	//basic
	ships[id] = ship;
	//command clients to update with this ship added
	Packet command;
	command << "add ship" << id << ship->owner << ship->type_id;

	client_1.send(command);
	//cout << "Sent a command to client 1 to place a ship at " << i << " " << j << " of type " << ship->type_id << endl;
	client_2.send(command);
	//cout << "Sent a command to client 2 to place a ship at " << i << " " << j << " of type " << ship->type_id << endl;
	branchSocket(id);
	calculateStrengths();
	updateStrengths();

	Packet score_update_1;
	score_update_1 << "update numbers" << cooldown_1 << score_1 << score_2 << strength_1 << strength_2;
	client_1.send(score_update_1);
	//cout << "Sent a command to client 1 to update its turn and score values.\n";
	Packet score_update_2;
	score_update_2 << "update numbers" << cooldown_2 << score_1 << score_2 << strength_1 << strength_2;
	client_2.send(score_update_2);
	//cout << "Sent a command to client 1 to update its turn and score values.\n";
}


string MatchServer::doesSocketExist(Vector2f point, double threshold){

	for(map<string, CardSocket*>::iterator i = sockets.begin(); i != sockets.end(); i++){

		Vector2f dist = point - i->second->getCenter();
		double h = hypot(dist.x,dist.y);
		if(h <= threshold){
			return i->first;
		}
	}
	return "";
}

void MatchServer::branchSocket(string id){

	CardSocket* socket = sockets[id];

	Vector2f a = Vector2f(0,-1000);
	Vector2f b = Vector2f(-866,500);
	Vector2f c = Vector2f(866,500);

	double spin = socket->spin;
	//while(spin > 90){ spin -= 90; }
	//cout << spin << endl;
	spin = spin * (3.14159/180.0); //convert to radians

	Vector2f a1 = Vector2f((a.x*cos(spin))+(a.y*sin(spin)),(-a.x*sin(spin))+(a.y*cos(spin)));
	Vector2f b1 = Vector2f((b.x*cos(spin))+(b.y*sin(spin)),(-b.x*sin(spin))+(b.y*cos(spin)));
	Vector2f c1 = Vector2f((c.x*cos(spin))+(c.y*sin(spin)),(-c.x*sin(spin))+(c.y*cos(spin)));

	a1 = a1 + socket->getCenter();
	b1 = b1 + socket->getCenter();
	c1 = c1 + socket->getCenter();

	string a_id = doesSocketExist(a1);
	if(a_id == ""){
		a_id = asString(a1.x) + "_" + asString(a1.y);
		sockets[a_id] = new CardSocket(a_id, a1, socket->spin + 60.0);
		ships[a_id] = NULL;
		Packet command;
		command << "add socket" << a_id << double(a1.x) << double(a1.y) << double(socket->spin + 60.0);
		client_1.send(command);
		client_2.send(command);
	}
	if(socket->linked[0] != a_id && socket->linked[1] != a_id && socket->linked[2] != a_id){ //if this socket has been linked already in any way, DO NOT DOUBLE LINK
		socket->linked[socket->firstFree()] = a_id;
		sockets[a_id]->linked[sockets[a_id]->firstFree()] = socket->id;
		Packet link_command_a;
		link_command_a << "link sockets" << socket->id << a_id;
		client_1.send(link_command_a);
		client_2.send(link_command_a);
	}
	
	
	string b_id = doesSocketExist(b1);
	if(b_id == ""){
		b_id = asString(b1.x) + "_" + asString(b1.y);
		sockets[b_id] = new CardSocket(b_id, b1, socket->spin + 60.0);
		ships[b_id] = NULL;
		Packet command;
		command << "add socket" << b_id << double(b1.x) << double(b1.y) << double(socket->spin + 60.0);
		client_1.send(command);
		client_2.send(command);
	}
	if(socket->linked[0] != b_id && socket->linked[1] != b_id && socket->linked[2] != b_id){ //if this socket has been linked already in any way, DO NOT DOUBLE LINK
		socket->linked[socket->firstFree()] = b_id;
		sockets[b_id]->linked[sockets[b_id]->firstFree()] = socket->id;
		Packet link_command_b;
		link_command_b << "link sockets" << socket->id << b_id;
		client_1.send(link_command_b);
		client_2.send(link_command_b);
	}

	string c_id = doesSocketExist(c1);
	if(c_id == ""){
		c_id = asString(c1.x) + "_" + asString(c1.y);
		sockets[c_id] = new CardSocket(c_id, c1, socket->spin + 60.0);
		ships[c_id] = NULL;
		Packet command;
		command << "add socket" << c_id << double(c1.x) << double(c1.y) << double(socket->spin + 60.0);
		client_1.send(command);
		client_2.send(command);
	}
	if(socket->linked[0] != c_id && socket->linked[1] != c_id && socket->linked[2] != c_id){ //if this socket has been linked already in any way, DO NOT DOUBLE LINK
		socket->linked[socket->firstFree()] = c_id;
		sockets[c_id]->linked[sockets[c_id]->firstFree()] = socket->id;
		Packet link_command_c;
		link_command_c << "link sockets" << socket->id << c_id;
		client_1.send(link_command_c);
		client_2.send(link_command_c);
	}
}

void MatchServer::initalizeMatch(){

	//get usernames and core ship txids from each client
	client_1.setBlocking(true);
	client_2.setBlocking(true);
	Packet command;
	command << "send player info";
	client_1.send(command);
	Packet info_1;
	client_1.receive(info_1);
	info_1 >> username_1 >> core_ship_1_tx;
	client_2.send(command);
	Packet info_2;
	client_2.receive(info_2);
	info_2 >> username_2 >> core_ship_2_tx;
	
	for(int i = 0; i < 64; i++){
		stock.push_back(randomNewCard(0));
	}
	//load socket map and spawn matching NULL ship map, and set background to be used
	background_tx = "space_tile";
	sockets["start_1"] = new CardSocket("start_1", Vector2f(0,0), 0);
	sockets["start_2"] = new CardSocket("start_2", Vector2f(5196,0), 0);
	for(map<string, CardSocket*>::iterator i = sockets.begin(); i != sockets.end(); i++){
		ships[i->first] = NULL;
	}

	score_1 = 0;
	score_2 = 0;
	strength_1 = 0;
	strength_2 = 0;
	

	cooldown_1 = 0;
	cooldown_2 = 0;
	time_left = 300;
	turn = 1;


	Packet initialize_1; //send missing pieces to each client
	initialize_1 << "initialize";
	initialize_1 << background_tx << username_2 << core_ship_2_tx;
	client_1.send(initialize_1);

	Packet initialize_2;
	initialize_2 << "initialize";
	initialize_2 << background_tx << username_1 << core_ship_1_tx;
	client_2.send(initialize_2);

	Packet ready_1;
	Packet ready_2;
	client_1.receive(ready_1); //wait for clients to respond with ready confirmation
	client_2.receive(ready_2);

	client_1.setBlocking(false);
	client_2.setBlocking(false);

	match_in_progress = true;
	Packet begin;
	begin << "begin";
	client_1.send(begin);
	client_2.send(begin);

	active = true;

	//placeShip("start_1", new Ship(1, "Capital Ship"));
	//placeShip("start_2", new Ship(2, "Capital Ship"));
}

void MatchServer::determineModifications(string ship_id){

	Ship* ship = ships[ship_id];
	
	int you = ship->getOwner();
	
	ship->additions.clear();
	ship->multiplications.clear();
	ship->support_additions.clear();
	ship->support_multiplications.clear();
	ship->support_blocked = false;
	ship->nullify_strength = false;

	//need to add checks to ensure we're not calling functions off of null ships

	bool ship_exists[3] = {false, false, false};
	if(ships[sockets[ship_id]->linked[0]] != NULL){ ship_exists[0] = true; }
	if(ships[sockets[ship_id]->linked[1]] != NULL){ ship_exists[1] = true; }
	if(ships[sockets[ship_id]->linked[2]] != NULL){ ship_exists[2] = true; }

	if(ship_exists[0] && ship_exists[1] && ship_exists[2] && ships[sockets[ship_id]->linked[0]]->getAbilities()->count("Flanking") != 0 && ships[sockets[ship_id]->linked[1]]->getAbilities()->count("Flanking") != 0 && ships[sockets[ship_id]->linked[2]]->getAbilities()->count("Flanking") != 0 && ships[sockets[ship_id]->linked[0]]->getOwner() != you && ships[sockets[ship_id]->linked[1]]->getOwner() != you && ships[sockets[ship_id]->linked[2]]->getOwner() != you){
		ship->nullify_strength = true;
	}

	for(int index = 0; index < 3; index++){

		if(!ship_exists[index]){ continue; }
		Ship* other_ship = ships[sockets[ship_id]->linked[index]];
		if(other_ship->getOwner() == you){
			for(set<string>::iterator i = other_ship->getAbilities()->begin(); i != other_ship->getAbilities()->end(); i++){
				string ability = *i;
				if(ability == "Underworld Contacts"){
					if(ship->type_id == "Smuggler Galleon" || ship->type_id == "Io Mercenary Warship" || ship->type_id == "Io Mercenary Warship" || ship->type_id == "Io Mercenary Boarding Crew" || ship->type_id == "Smuggler Caravel" || ship->type_id == "Smuggler Drekkar" || ship->type_id == "Smuggler Slave Transport" || ship->type_id == "Smuggler Frigate"){
						ship->support_additions.push_back(1.0);
					}
				}
				if(ability == "Battle Formation"){
					//military units give each other a +1 boost when adjacent
					if(ship->getClass() == "Military" && other_ship->getClass() == "Military"){
						ship->support_additions.push_back(1.0);
					}
				}
				if(ability == "Collective Computing"){
					if(ship->type_id == other_ship->type_id){
						ship->support_multiplications.push_back(2.0);
					}
				}
				if(ability == "Software Update"){
					if(ship->getClass() == "Robot" && ship->getBase() < other_ship->getBase()){
						ship->support_additions.push_back(other_ship->getBase() - ship->getBase());
					}
				}
				if(ability == "Battlefield Intelligence"){
					//intel hubs give boost equal to # of friendly intel hubs
					double number_of_intel_hubs = 0.0;
					for(map<string, Ship*>::iterator j = ships.begin(); j != ships.end(); j++){
						if(j->second != NULL && j->second->getOwner() == you && j->second->type_id == "UGA Intel Hub"){
							number_of_intel_hubs++;
						}
					}
					ship->support_additions.push_back(number_of_intel_hubs);
				}
				if(ability == "Essential Supplies"){
					ship->support_additions.push_back(1.0);
				}
				if(ability == "Quality Healthcare"){
					ship->support_additions.push_back(3.0);
				}
				if(ability == "Spare Equipment"){
					ship->support_additions.push_back(2.0);
				}
				if(ability == "Jam Communications"){
					ship->support_blocked = false;
				}
				if(ability == "Electromagnetic Pulse"){
					if(ship->getClass() == "robot"){
						ship->nullify_strength = true;
					}
				}
			}
		}
		else{ //other ship is opponent
			
			for(set<string>::iterator i = other_ship->getAbilities()->begin(); i != other_ship->getAbilities()->end(); i++){
				string ability = *i;
				if(ability == "Shock and Awe"){
					if(other_ship->getBase() > ship->getBase()){
						ship->additions.push_back(-2.0);
					}
				}
				if(ability == "Catch Me If You Can"){
					if(other_ship->getBase() < ship->getBase()){
						ship->additions.push_back(-2.0);
					}
				}
				if(ability == "Heavy Armour I"){
					ship->additions.push_back(-1.0);
				}
				if(ability == "Heavy Armour II"){
					ship->additions.push_back(-2.0);
				}
				if(ability == "Heavy Armour III"){
					ship->additions.push_back(-3.0);
				}
				if(ability == "Decoy Flares"){
					ship->multiplications.push_back(0.67);
				}
				if(ability == "Amorphous Structure"){
					ship->multiplications.push_back(0.25);
				}
			}
		}
	}

	for(set<string>::iterator i = ship->getAbilities()->begin(); i != ship->getAbilities()->end(); i++){
		
		string ability = *i;
		if(ability == "Field Commander"){
			if(ship_exists[0] && ship_exists[1] && ship_exists[2] && ships[sockets[ship_id]->linked[0]]->getOwner() == you && ships[sockets[ship_id]->linked[1]]->getOwner() == you && ships[sockets[ship_id]->linked[2]]->getOwner() == you){
				ship->multiplications.push_back(4.0);
			}
		}
		if(ability == "Unpopular"){
			for(int index = 0; index < 3; index++){
				if(!ship_exists[index]){continue;}
				if(ships[sockets[ship_id]->linked[index]]->getOwner() == you && ships[sockets[ship_id]->linked[index]]->getClass() == "Merchant"){
					ship->additions.push_back(-1.0);
				}
			}
		}
		if(ability == "Traitor"){
			for(int index = 0; index < 3; index++){
				if(!ship_exists[index]){continue;}
				if(ships[sockets[ship_id]->linked[index]]->getOwner() == you && ships[sockets[ship_id]->linked[index]]->getClass() == "Military"){
					ship->additions.push_back(-2.0);
				}
			}
		}
		if(ability == "Lifelong Thieves"){
			for(int index = 0; index < 3; index++){
				if(!ship_exists[index]){continue;}
				if(ships[sockets[ship_id]->linked[index]]->getOwner() != you && ships[sockets[ship_id]->linked[index]]->getClass() == "Merchant"){
					ship->additions.push_back(2.0);
				}
			}
		}
		if(ability == "Food Chain"){
			for(int index = 0; index < 3; index++){
				if(!ship_exists[index]){continue;}
				if(ships[sockets[ship_id]->linked[index]]->getOwner() != you && ships[sockets[ship_id]->linked[index]]->getBase() < ship->getBase()){
					ship->additions.push_back(1.0);
				}
				else if(ships[sockets[ship_id]->linked[index]]->getOwner() != you && ships[sockets[ship_id]->linked[index]]->getBase() > ship->getBase()){
					ship->additions.push_back(-1.0);
				}
			}
		}
		if(ability == "Non-Apex Predator"){
			for(int index = 0; index < 3; index++){
				if(!ship_exists[index]){continue;}
				if(ships[sockets[ship_id]->linked[index]]->getOwner() != you && ships[sockets[ship_id]->linked[index]]->getBase() < ship->getBase()){
					ship->multiplications.push_back(2.0);
				}
				else if(ships[sockets[ship_id]->linked[index]]->getOwner() != you && ships[sockets[ship_id]->linked[index]]->getBase() > ship->getBase()){
					ship->multiplications.push_back(0.5);
				}
			}
		}
		if(ability == "Blind Spot"){
			for(int index = 0; index < 3; index++){
				if(!ship_exists[index]){continue;}
				if(ships[sockets[ship_id]->linked[index]]->getOwner() != you && ships[sockets[ship_id]->linked[index]]->getBase() < ship->getBase()){
					ship->additions.push_back(-1.0);
				}
			}
		}
		if(ability == "Big and Slow"){
			for(int index = 0; index < 3; index++){
				if(!ship_exists[index]){continue;}
				if(ships[sockets[ship_id]->linked[index]]->getOwner() != you && ships[sockets[ship_id]->linked[index]]->getBase() < ship->getBase()){
					ship->additions.push_back(-2.0);
				}
			}
		}
		if(ability == "Replicate Parts"){
			double lowest = 0;
			for(int index = 0; index < 3; index++){
				if(!ship_exists[index]){continue;}
				if(ships[sockets[ship_id]->linked[index]]->getOwner() == you && (ships[sockets[ship_id]->linked[index]]->getBase() < lowest || lowest == 0)){
					lowest = ships[sockets[ship_id]->linked[index]]->getBase();
				}
			}
			ship->additions.push_back(lowest);
		}
		if(ability == "Reverse-Engineer"){
			double highest = 0;
			for(int index = 0; index < 3; index++){
				if(!ship_exists[index]){continue;}
				if(ships[sockets[ship_id]->linked[index]]->getOwner() != you && ships[sockets[ship_id]->linked[index]]->getBase() > highest){
					highest = ships[sockets[ship_id]->linked[index]]->getBase();
				}
			}
			ship->additions.push_back(highest);
		}
	}

}

StreamNode::StreamNode(){}
StreamNode::StreamNode(StreamNode* c_parent, string c_key){

	parent = c_parent;
	key = c_key;
}
/*

void exploreStream(StreamNode* this_node, vector<StreamNode*> &created_nodes, vector<StreamNode*> &end_nodes){

	Ship* ship = ships[this_node->key];
	bool none_explored = true;
	for(int index = 0; index < 3; index++){
		if(this->node->parent != NULL && this_node->parent->key == sockets[this_node->key]->linked[index]){ continue; }
		if(ship->getAbilities()->count("Strengthen Communications") != 0 || ships[sockets[this_node->key]->linked[index]] ->getAbilities()->count("Strengthen Communications") != 0){
			created_nodes.push_back(new StreamNode(this_node, sockets[this_node->key]->linked[index]));
			none_explored = false;
			exploreStream(created_nodes[created_nodes.size()-1], created_nodes, end_nodes);
		}
	}
	if(none_explored){
		end_nodes.push_back(this_node);
	}
}

void MatchServer::transmitSupport(){

	set<string> ships_that_transmit;
	for(map<string, Ship*>::iterator i = ships.begin(); i != ships.end(); i++){

		if(i->second->getAbilities()->count("Strengthen Communications") && i->second->support_blocked == false){
			ships_that_transmit.insert(i->first);
		}
	}
	set<string> support_roots;
	for(set<string>::iterator i = ships_that_transmit.begin(); i != ships_that_transmit.end(); i++){
		for(int index = 0; index < 3; index++){
			if(ships_that_transmit.count(sockets[ship_id]->linked[index]) != 0){ continue; }
			support_roots.insert(sockets[ship_id]->linked[index]);
		}
	}

	vector<StreamNode*> created_nodes;
	vector<StreamNode*> end_nodes;

	for(set<string>::iterator i = support_roots.begin(); i != support_roots.end(); i++){

		created_nodes.push_back(new StreamNode(NULL, *i));
		exploreStream(created_nodes[created_nodes.size()-1], created_nodes, end_nodes);
	}

	//what happens if a node is traversed with support from the same node more than once?
	for(vector<StreamNode*>::iterator i = end_nodes.begin(); i != end_nodes.end(); i++){
		

	}
	for(vector<StreamNode*>::iterator i = created_nodes.begin(); i != created_nodes.end(); i++){
		delete *i;
	}
	created_nodes.clear();
	end_nodes.clear();
}
*/
void MatchServer::listenForClientCommand(int client_id){

	if(client_id == 1){
		Packet command;
		while(client_1.receive(command) == Socket::Done){
			//if a packet got through this tick
			string command_str;
			command >> command_str;
			if(command_str == "place ship"){
				//cout << "Was told to add ship.\n";
				//pull more data that should have been included in the packet
				string id;
				string type_id;
				command >> id >> type_id;
				placeShip(id, new Ship(1, type_id));
				changeTurn();
			}
			else if(command_str == "buy ship"){

				if(cooldown_1 <= 0){
					int slot_id;
					command >> slot_id;
					buyShip(slot_id, 1);
					//changeTurn();
				}
				
			}
			else if(command_str == "engage"){
				//cout << "Was told to add engage.\n";
				engage();
				//changeTurn();
			}
		}
	}
	else if(client_id == 2){
		Packet command;
		while(client_2.receive(command) == Socket::Done){
			//if a packet got through this tick
			string command_str;
			command >> command_str;
			if(command_str == "place ship"){
				//cout << "Was told to add ship.\n";
				//pull more data that should have been included in the packet
				string id;
				string type_id;
				command >> id >> type_id;
				placeShip(id, new Ship(2, type_id));
				changeTurn();
			}
			else if(command_str == "buy ship"){
				if(cooldown_2 <= 0){
					int slot_id;
					command >> slot_id;
					buyShip(slot_id, 2);
					//changeTurn();
				}
				
			}
			else if(command_str == "engage"){
				//cout << "Was told to add engage.\n";
				engage();
				//changeTurn();
			}
		}
	}
	
}