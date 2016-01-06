// 2015 Richard Kwok

#include "main.h"

using namespace std;
using namespace sf;

RenderWindow window;
View view;
View fixed_view;
UI ui;
map<string, Properties*> properties;
map<string, Texture*> textures;
map<string, Font*> fonts;
bool player_in_game;

void server_main(){

    double dt = 0.001;
    Clock timer;

    MatchServer* match_server = new MatchServer();
    while(window.isOpen() && player_in_game){

        //server utilities
        dt = timer.getElapsedTime().asSeconds();
        timer.restart();

        //run game updates, listen for client input and send out map and score updates to clients
        match_server->process(dt);
        if(match_server->match_finished){
            break;
        }
    }
    delete match_server; // when server is closed
    
    
    return;
}

void player_main(string ip){

    double dt = 0.001;
    Clock timer;

    double scale = 1;

    MatchClient* client = new MatchClient("player");
    client->joinMatch(ip);
    while(window.isOpen()){

        window.clear();
        dt = timer.getElapsedTime().asSeconds(); //controls visuals but no gameplay
        timer.restart();
        ui.collect();

        if(ui.keyWasReleased("esc")){ break; }

        view = window.getView();
        if(ui.keyIsPressed("rmb")){ view.move(scale*ui.mouse_delta.x, scale*ui.mouse_delta.y);}
        scale += ui.mmb_delta*-40*dt;
        if(scale < 1){scale = 1;}
        if(scale > 60.0){scale = 60.0;}
        view.setSize(fixed_view.getSize().x*scale,fixed_view.getSize().y*scale);
        window.setView(view);
        
        ui.updateGUI(client);

        client->process(dt);

        //draw graphics to screen
        window.setView(fixed_view);
        client->drawBackground();
        window.setView(view);
        client->draw();
        window.setView(fixed_view);
        ui.draw();
        window.setView(view);

        window.display();
    }
    delete client;

    view = fixed_view;
    window.setView(view);
}

void ai_main(){

    double dt = 0.001;
    Clock timer;

    MatchClient* ai_client = new MatchClient("ai");
    ai_client->joinMatch("127.0.0.1");
    while(window.isOpen() && player_in_game){

        dt = timer.getElapsedTime().asSeconds(); //controls visuals but no gameplay
        timer.restart();

        ai_client->process(dt);

        int selected_for_sale = rand() % 5;
        if(ai_client->for_sale[selected_for_sale] != NULL && ai_client->cooldown <= 0){
            Packet command;
            command << "buy ship" << selected_for_sale;
            ai_client->server_socket.send(command);
        }
    }
    delete ai_client;
}

int main(){

    loadTextures();
    loadFonts();
    loadProperties();

    double dt = 0.001;
    Clock timer;

    // Redirect to nothing
    srand (time(NULL));
    sf::err().rdbuf(NULL);

    player_in_game = false;
    
    Thread server_thread(&server_main);
    Thread ai_thread(&ai_main);

    //initialize global stuff
    window.create(sf::VideoMode(1240, 720), "Fleet");
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(200);
    view = window.getView();
    fixed_view = view;
    view.zoom(1.0);
    window.setView(view);
    ui.initialize();


    int menu_id = 0;
    string ip = "";
    Scroller menu_scroller = Scroller(Vector2f(3.0*(window.getSize().x/4.0), window.getSize().y/2.0), 500, 500);
    double scroll_countdown = 0;

    while (window.isOpen())
    {
        //client utilities
        window.clear();
        dt = timer.getElapsedTime().asSeconds(); // used for animations, server has its own clock
        timer.restart();
        ui.collect();


        //all options call the player_main() functions, stealing the flow from main. server option also opens a server thread in the background. singleplayer also opens an ai thread in the background.
        //which menu do we show?
        if(menu_id == 0){
            //main menu

            //decoration shows cards while menu is open
            scroll_countdown -= dt;
            if(scroll_countdown <= 0 || (menu_scroller.contains(ui.getFixedMouse().x,ui.getFixedMouse().y) && ui.keyWasReleased("lmb"))){
                scroll_countdown = 20.0;
                map<string, Properties*>::iterator p = properties.begin();
                advance(p, rand() % properties.size());
                menu_scroller.scrollUpTo(p->first);
            }
            menu_scroller.process(dt);
            //

            Vector2f sp_anchor = Vector2f(window.getSize().x/4.0, 1*(window.getSize().y/5.0));
            Vector2f mp_anchor = Vector2f(window.getSize().x/4.0, 2*(window.getSize().y/5.0));
            Vector2f mp_host_anchor = Vector2f(window.getSize().x/4.0, 3*(window.getSize().y/5.0));
            Vector2f quit_anchor = Vector2f(window.getSize().x/4.0, 4*(window.getSize().y/5.0));

            Color colour = Color(185,255,185,200);
            Color thinned_colour = colour;
            thinned_colour.a -= 100;

            RectangleShape sp;
            sp.setSize(Vector2f(window.getSize().x/3.0, window.getSize().y/7.0));
            sp.setOutlineThickness(3);
            sp.setOutlineColor(colour);
            sp.setFillColor(thinned_colour);
            sp.setOrigin((window.getSize().x/3.0)/2.0,(window.getSize().y/7.0)/2.0);
            sp.setPosition(sp_anchor);

            RectangleShape mp;
            mp.setSize(Vector2f(window.getSize().x/3.0, window.getSize().y/7.0));
            mp.setOutlineThickness(3);
            mp.setOutlineColor(colour);
            mp.setFillColor(thinned_colour);
            mp.setOrigin((window.getSize().x/3.0)/2.0,(window.getSize().y/7.0)/2.0);
            mp.setPosition(mp_anchor);

            RectangleShape mp_host;
            mp_host.setSize(Vector2f(window.getSize().x/3.0, window.getSize().y/7.0));
            mp_host.setOutlineThickness(3);
            mp_host.setOutlineColor(colour);
            mp_host.setFillColor(thinned_colour);
            mp_host.setOrigin((window.getSize().x/3.0)/2.0,(window.getSize().y/7.0)/2.0);
            mp_host.setPosition(mp_host_anchor);

            RectangleShape quit;
            quit.setSize(Vector2f(window.getSize().x/3.0, window.getSize().y/7.0));
            quit.setOutlineThickness(3);
            quit.setOutlineColor(colour);
            quit.setFillColor(thinned_colour);
            quit.setOrigin((window.getSize().x/3.0)/2.0,(window.getSize().y/7.0)/2.0);
            quit.setPosition(quit_anchor);

            Text sp_text;
            sp_text.setFont(*getFont("font1"));
            sp_text.setCharacterSize(36);
            sp_text.setColor(colour);
            sp_text.setString("Singleplayer");
            FloatRect sp_text_bounds = sp_text.getLocalBounds();
            sp_text.setOrigin(sp_text_bounds.width/2.0,sp_text_bounds.height/2.0);
            sp_text.setPosition(sp_anchor);

            Text mp_text;
            mp_text.setFont(*getFont("font1"));
            mp_text.setCharacterSize(36);
            mp_text.setColor(colour);
            mp_text.setString("Join Multiplayer");
            FloatRect mp_text_bounds = mp_text.getLocalBounds();
            mp_text.setOrigin(mp_text_bounds.width/2.0,mp_text_bounds.height/2.0);
            mp_text.setPosition(mp_anchor);

            Text mp_host_text;
            mp_host_text.setFont(*getFont("font1"));
            mp_host_text.setCharacterSize(36);
            mp_host_text.setColor(colour);
            mp_host_text.setString("Host Multiplayer");
            FloatRect mp_host_text_bounds = mp_host_text.getLocalBounds();
            mp_host_text.setOrigin(mp_host_text_bounds.width/2.0,mp_host_text_bounds.height/2.0);
            mp_host_text.setPosition(mp_host_anchor);

            Text quit_text;
            quit_text.setFont(*getFont("font1"));
            quit_text.setCharacterSize(36);
            quit_text.setColor(colour);
            quit_text.setString("Quit");
            FloatRect quit_text_bounds = quit_text.getLocalBounds();
            quit_text.setOrigin(quit_text_bounds.width/2.0,quit_text_bounds.height/2.0);
            quit_text.setPosition(quit_anchor);

            if(sp.getGlobalBounds().contains(ui.getFixedMouse().x,ui.getFixedMouse().y)){
                sp.setScale(1.1,1.1);
                if(ui.keyWasReleased("lmb")){
                    player_in_game = true;
                    server_thread.launch();
                    ai_thread.launch();
                    player_main("127.0.0.1");
                    player_in_game = false;
                }
            }
            else if(mp.getGlobalBounds().contains(ui.getFixedMouse().x,ui.getFixedMouse().y)){
                mp.setScale(1.1,1.1);
                if(ui.keyWasReleased("lmb")){
                    menu_id = 1;
                }
            }
            else if(mp_host.getGlobalBounds().contains(ui.getFixedMouse().x,ui.getFixedMouse().y)){
                mp_host.setScale(1.1,1.1);
                if(ui.keyWasReleased("lmb")){
                    player_in_game = true;
                    server_thread.launch();
                    player_main("127.0.0.1");
                    player_in_game = false;
                }
            }
            else if(quit.getGlobalBounds().contains(ui.getFixedMouse().x,ui.getFixedMouse().y)){
                quit.setScale(1.1,1.1);
                if(ui.keyWasReleased("lmb")){
                    window.close();
                }
            }

            window.setView(view);
            window.draw(sp);
            window.draw(mp);
            window.draw(mp_host);
            window.draw(quit);
            window.draw(sp_text);
            window.draw(mp_text);
            window.draw(mp_host_text);
            window.draw(quit_text);
            menu_scroller.draw();
        }
        else if(menu_id == 1){
            //join server menu: allows an ip to be typed in to specify a server to join
            RectangleShape text_box;
            text_box.setSize(Vector2f(400,60));
            text_box.setOutlineThickness(3);
            text_box.setOutlineColor(Color(155,155,155,155));
            text_box.setFillColor(Color(155,155,155,45));
            text_box.setOrigin(200,30);
            text_box.setPosition(window.getSize().x/2.0,window.getSize().y/2.0);
            
            if(ip != "" && ui.keyWasReleased("delete")){
                ip.pop_back();
            }
            else if(ip != "" && ui.keyWasReleased("enter")){
                //join server with this ip
                player_in_game = true;
                player_main(ip);
                player_in_game = false;
                ip == "";
                menu_id = 0;
            }
            else{
                for(set<char>::iterator i = ui.text_entered.begin(); i != ui.text_entered.end(); i++){
                    ip += *i;
                }
            }

            Text text_contents;
            text_contents.setFont(*getFont("font1"));
            text_contents.setCharacterSize(32);
            text_contents.setColor(Color(200,200,200,215));
            text_contents.setString(ip);
            FloatRect text_bounds = text_contents.getLocalBounds();
            text_contents.setOrigin(text_bounds.width/2.0,text_bounds.height/2.0);
            text_contents.setPosition(window.getSize().x/2.0,window.getSize().y/2.0);
            
            window.setView(view);
            window.draw(text_box);
            window.draw(text_contents);
        }
        
        window.display();
    }
    

    destroyProperties();
    destroyFonts();
    destroyTextures();
	return 1;
}