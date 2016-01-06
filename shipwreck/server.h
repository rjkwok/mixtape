//a server controls the game loop and can be paused, saved, or loaded from file
#ifndef WEC_SESS
#include "main.h"

using namespace std;
using namespace sf;

struct ExternalUser{

    ExternalUser();
    bool initialize(Vector2f spawn_point);

    string name;
    string id;

    TcpSocket socket;

    InputData input;
    vector<ObData> obs_to_add;

    bool receiveFromUser();
    bool sendToUser(Packet &packet);
};

class Server{

public:

    Server();

    void processGame(); //main server operation function

    Clock timer;
    double dt = 1; //clocks speed of server

    //current sector the user is in, and the user id
    string sector_id = "A1";
    map<string, shared_ptr<ExternalUser> > connected_players;

};

#endif // WEC_SESS

