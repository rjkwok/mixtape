#include "main.h"

using namespace std;
using namespace sf;

//globally accessible objects
World world;
RenderWindow window;

map<string, Font> fonts;
Session session;

void loadAllFonts()
{
    fonts["arial"] = Font();
    fonts["arial"].loadFromFile("fonts/arial.ttf");

    fonts["font1"] = Font();
    fonts["font1"].loadFromFile("fonts/font1.ttf");
}


int main()
{
    srand(time(0));

    loadAllFonts();
    loadAllProperties();

   // generateScrapMaps();

//    cout << "Type \"start\" to start a new server, or \"join\" to join an existing one: ";
//    string starting_action;
//    cin >> starting_action;
//    if(starting_action == "start"){
//
//        cout << "\n";
//        cout << " //////////////////////////////////////////////\n";
//        cout << " //////////// DEEP SERVER CONSOLE /////////////\n";
//        cout << " //////////////////////////////////////////////\n";
//        cout << "\n";
//
//        cout << " Initializing...\n";
//
//        bool valid_index = world.loadWorldFromFile("world1");
//
//        if(not valid_index)
//        {
//            cout << "INVALID WORLD FILE." << endl;
//            return 1;
//        }
//
//        Server server = Server();
//        server.sector_id = "A1";
//
//        unsigned short listening_port = 55101;
//
//        cout << " Server successfully initialized at ip "<< IpAddress::getLocalAddress() <<".\n";
//        cout << " Now listening for users on port " << listening_port << ".\n";
//        cout << "\n";
//
//        TcpListener listener;
//        listener.listen(listening_port);
//        listener.setBlocking(false);
//        shared_ptr<ExternalUser> new_user = make_shared<ExternalUser>();
//
//        while(true){
//
//            if(listener.accept(new_user->socket) == TcpSocket::Done){
//                cout << " Connected with a new user on port " << new_user->socket.getLocalPort() << ". Initializing...\n";
//                while(!new_user->initialize(Vector2f(700,600))){
//                    //wait until fully initialized
//                }
//                cout << " New user \"" << new_user->name << "\" successfully initialized!\n\n";
//                server.connected_players[new_user->name] = new_user;
//                new_user = make_shared<ExternalUser>();
//            }
//
//            if(server.connected_players.size() > 0){
//                server.processGame();
//            }
//        }
//
//        return 0;
//    }
//    else if(starting_action == "join"){
//
//        cout << "\n";
//        cout << " //////////////////////////////////////////////\n";
//        cout << " ///////////////// DEEP CLIENT ////////////////\n";
//        cout << " //////////////////////////////////////////////\n";
//        cout << "\n";
//        cout << " Enter your username: ";
//        string user_name;
//        cin >> user_name;
//
//        cout << " Enter an ip address to connect to: ";
//
//        string ip_string;
//        cin >> ip_string;
//        cout << " Attempting to establish connection with server at " << ip_string << "...\n";
//
//        TcpSocket socket;
//        while(socket.connect(IpAddress(ip_string),55101) != TcpSocket::Done){}
//
//        cout << " Connection Successful! Initializing session.\n";
//
////        receiveEntireWorld(socket);
//        Session session("Deep");
//
//        Packet initial_packet;
//        initial_packet << user_name << session.player_id;
//        socket.send(initial_packet);
//
//        cout << " Initialization complete. Ready to play!\n";
//
//        while(true){
//
//            session.processGame();
//        }
//
//        return 0;
//    }
//    else if(starting_action == "single"){

        cout << "\n";
        cout << " //////////////////////////////////////////////\n";
        cout << " ///////////////// DEEP CLIENT ////////////////\n";
        cout << " //////////////////////////////////////////////\n";
        cout << "\n";

        cout << " Initializing...\n";

   /*     bool valid_index = world.loadWorldFromFile("world1");

        if(not valid_index)
        {
            cout << "INVALID WORLD FILE." << endl;
            return 1;
        }*/

        world.loadWorldFromFile("station_c");

        Session session("Deep");

        while(session.processGame()){}

        return 0;
    //}

}


