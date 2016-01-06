#include "main.h"

using namespace std;
using namespace sf;


extern World world;

ChatEntry::ChatEntry(){}

ChatEntry::ChatEntry(string new_source, string new_contents){

    source = new_source;
    contents = new_contents;

    text.setString(source + ": \"" + contents + "\"");
    text.setCharacterSize(16);
   // text.setFont(*fonts["font1"]);
    text.setColor(Color(200,200,200,215));

    seconds_remaining = 30;
}

Chat::Chat(){}

Chat::Chat(Vector2f new_coords, int new_max_entries){

    fixed_coords = new_coords;
    max_entries = new_max_entries;

    click_box = FloatRect(fixed_coords.x,fixed_coords.y,700,50);

}

void Chat::add(string source, string contents){

    ChatEntry new_entry = ChatEntry(source, contents);

    for(int i = max_entries; i > 1; i--){
        entries[i] = entries[i-1];
    }
    entries[1] = new_entry;
}

void Chat::update(double dt){



    double spacing = 10;
    double sum = 15;

    double opacity_boost = 0;
    if(accepting_input){opacity_boost=100;}

    set<int> to_delete;
    for(int i = 1; i <= max_entries; i++){
        sum += spacing;
        sum += entries[i].text.getGlobalBounds().height;
        entries[i].text.setPosition(fixed_coords.x,fixed_coords.y - sum);
        entries[i].text.setColor(Color(200,200,225,5+opacity_boost+(75*(entries[i].seconds_remaining/30))));
        entries[i].seconds_remaining -= dt;
        if(entries[i].seconds_remaining<=0){
            to_delete.insert(i);
        }
    }
    for(set<int>::iterator parser = to_delete.begin(); parser!=to_delete.end(); parser++){
        entries.erase(*parser);
    }

//    if(accepting_input){
//
//        string id = "CONSOLE";
//        string spacer = string(id.size()+3,' ');
//
//        fixedOutline(click_box, Color(200,200,225,200));
//        if(sin(world.date.second*5.5)>0){
//            fixedText(fixed_coords + Vector2f(10,5), Color(200,200,225,200), input_string + "|");
//        }
//        else{
//            fixedText(fixed_coords + Vector2f(10,5), Color(200,200,225,200), input_string);
//        }
//
//        for(set<char>::iterator parser = server.keys_released.begin(); parser!=server.keys_released.end(); parser++){
//            if(input_string.size()==75){
//                input_string += '\n';
//            }
//            if(*parser>=32 and *parser<=126 and input_string.size()<=150){
//                //typable text
//                input_string += *parser;
//            }
//            if(*parser==8){
//                //backspace
//                if(input_string.size()==0){
//                    input_string = "";
//                    accepting_input = false;
//                }
//                else{
//                    input_string.pop_back();
//                }
//            }
//            if(*parser==27){
//                //esc
//                input_string = "";
//                accepting_input = false;
//            }
//            if(*parser==13){
//                //carriage return
//                if(input_string!=""){
//                    if(input_string.size()>75){
//                        input_string.insert(76,spacer);
//                    }
//                    add(server.player_id, input_string);
//                }
//                input_string = "";
//                accepting_input = false;
//            }
//        }
//
//        server.keys_released.clear();
//        server.access = Access_Input;
//    }

//    Vector2f m = window.mapPixelToCoords(Mouse::getPosition(window),server.fixedview); //mouse location
//
//    if(server.keys_released.count(13)!=0){
//        accepting_input = true;
//    }
//    if(click_box.contains(m)){
//        fixedOutline(click_box, Color(200,200,200,200));
//        if(server.lmb_released){
//            accepting_input = true;
//        }
//    }
}
