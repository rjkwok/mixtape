#include "main.h"

using namespace std;
using namespace sf;

extern World world;

extern RenderWindow window;

ExternalUser::ExternalUser(){}

bool ExternalUser::initialize(Vector2f spawn_point){

    //sendEntireWorld(socket);
  //  name = "Richard";
   // Packet packet;
   // if(socket.receive(packet) == TcpSocket::Done){

    //    packet >> name >> id;

        //ObData ob_data;
        //ob_data.ob_id = id;
        //ob_data.ob_type = "Human";
       // ob_data.ob_coords = spawn_point;
       // ob_data.ob_rotation = 0;

        //world.loadFromObData(server.sector_id, ob_data);

        return true;
  //  }
  //  else{
   //     return false;
   // }
}

bool ExternalUser::receiveFromUser(){

    input = InputData();
    obs_to_add.clear();

    Packet header;
    if(socket.receive(header) == TcpSocket::Done){
        int number_of_new_obs;
        header >> number_of_new_obs;

        Packet main_input;
        socket.receive(main_input);
        main_input >> input;

        for(int i = 0; i < number_of_new_obs; i++){
            Packet new_ob_packet;
            socket.receive(new_ob_packet);
            ObData new_ob_data;
            new_ob_packet >> new_ob_data;
            obs_to_add.push_back(new_ob_data);
        }
        return true;
    }
    else{
        return false;
    }

}

bool ExternalUser::sendToUser(Packet &packet){

    if(socket.send(packet) == TcpSocket::Done){
        return true;
    }
    else{
        return false;
    }
}

Server::Server(){

    window.create(VideoMode(100,100), "Deep Server");
    window.setFramerateLimit(30);
}

void Server::processGame()
{
    dt = timer.getElapsedTime().asSeconds();
    if(dt>1){dt = 0.001;}
    timer.restart();

    world.date.update(dt);
    getSector(sector_id)->process(dt);

}
