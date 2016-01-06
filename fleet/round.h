#ifndef FLEET_ROUND
#define FLEET_ROUND

#include "main.h"

using namespace std;
using namespace sf;

//need a structure to initialize and store the sockets, socketed ships, and calculate the sum strengths of said ships.
//server can handle this
//client-side should be able to query the server with a ship argument to retrieve possible socket locations
//actual ship structs need not be sent, just the type_id and maybe the owner...although the server might be able to implicitly determine owner based off of sending address
//client maintains a map of ships for the purposes of sprite drawing and ui. this map is entirely slaved to the server's master copy 
//periodically the server sends its map data over. If the type_ids at [i,j] do not match for the client the client will correct itself
//the server stores attack and total scores, and recalculates them periodically based on the board layout
//score recalculationa and consistency check are cued when the server recieves a new ship placement
//if server recieves engage command, do engage. trash all board ships (clients will alter own discard piles based on local boards), and order consistency checks on both
//clients own their own decks, discarded piles, and hands. the ui can meddle with these as it pleases.
//the server loop listens for a placeShip/engage command from the player whose turn it is.
//depending on response, server will either initialize a new Ship() in memory based on the sent type_id, and assign the owner as the sender whose turn it currently is
//or the server will engage
//both clients are listening for consistency info, perform check as soon as info is sent by the server after the server places/engages
//server switches turn


class MatchClient{

public:

	MatchClient();
	MatchClient(string c_username);
	~MatchClient();

	int player_id;

	string username;
	string core_tx;

	bool match_in_progress;
	int score_1;
	int score_2;
	int strength_1;
	int strength_2;
	string enemy_username;

	Ship* for_sale[5];
    vector<Ship*> hand;
    vector<Ship*> discarded;

    Ship* selected_ship; //when a ship is "picked up" from the hand and is waiting to be placed on the board, this is where it lives
    int turn;

    int win_lose_tie;
    //graphics, initialized from a special server packet
    Sprite core_sprite_1;
    Sprite core_sprite_2;
    Sprite background;
    vector<CardGraphic> card_graphics;
    vector<Sprite> anchors;
    vector<Text> texts;
    
    TcpSocket server_socket;

    map<string, CardSocket*> sockets;
    map<string, Ship*> ships;

    double cooldown;

    void process(double dt);

    void drawBackground();
	void draw();
	bool joinMatch(string ip); //assumes the user already knows the ip the game is hosted by, sends user info
	void initalizeMatch(string background_tx, string other_core_tx);
	void listenForServerCommand();

	void sendPlayerInfo();
	void removeShip(string id);
	void removeSocket(string id);
	void addShip(string id, int owner, string type_id);
	void addSocket(string id, double x, double y, double spin);
	void placeShip(string id, Ship* ship, bool should_notify = true);
	void engage();
	void updateNumbers(double c, int sc1, int sc2, int st1, int st2);


};

struct StreamNode{

	StreamNode();
	StreamNode(StreamNode* c_parent, string c_key);

	StreamNode* parent;
	string key;
};

class MatchServer{

public:

	MatchServer();
	~MatchServer();

	bool active;

	vector<Ship*> stock;
	Ship* for_sale[5];

	map<string, Ship*> ships;
	Vector2f core_socket_1; //mount point for the core graphic for 1
	Vector2f core_socket_2; //same as above for 2
	map<string, CardSocket*> sockets; //function generated between core socket 1 and core socket 2

	int strength_1;
    int strength_2;

    int score_1;
    int score_2;

    int turn;

    double time_left;
    bool match_finished;
    double cooldown_1;
    double cooldown_2;

    string background_tx;

    bool match_in_progress;
    TcpSocket client_1;
    string username_1;
    string core_ship_1_tx;
    TcpSocket client_2;
    string username_2;
    string core_ship_2_tx;
    TcpListener listener;

    //tools for managing match
    void changeTurn();
    void buyShip(int slot_id, int c_owner);
    void process(double dt);
    void engage();
    void placeShip(string id, Ship* ship); //the new ship is constructed within the call from process, using information sent from the client
    void initalizeMatch();
    string doesSocketExist(Vector2f point, double threshold = 40.0);
    void branchSocket(string id);

	//tools for applying strength modifications
	//vector<Ship*> getFilteredShips(set<string> types, int affiliation);
	//vector<Ship*> getFilteredShipsExcept(set<string> types, int affiliation);
	//

	//tools for summing attack strength
	
    void determineModifications(string ship_id);
	void calculateStrengths();

	//server-client relationship tools
	void listenForClientCommand(int client);
	void updateStrengths();

};
#endif