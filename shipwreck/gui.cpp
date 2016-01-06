#include "main.h"

using namespace std;
using namespace sf;

extern World world;
extern RenderWindow window;



//FileMenuGUI::FileMenuGUI(){ gui_type = "file menu"; }
//
//void FileMenuGUI::update(double dt){
//
//    mouse_alerts.clear();
//
//    if(input.keys_released.count("esc") != 0){
//        toggle();
//    }
//
//    if(enabled){
//
//        fixed_rectangles.clear();
//        captions.clear();
//
//        RectangleShape bounding_box;
//        bounding_box.setSize(Vector2f(400,400));
//        bounding_box.setFillColor(Color(0,155,155,105));
//        bounding_box.setOutlineColor(Color(0,155,155,205));
//        bounding_box.setOutlineThickness(5);
//        bounding_box.setOrigin(200,200);
//        bounding_box.setPosition(720,450);
//        fixed_rectangles.push_back(bounding_box);
//
//        if(input.lmb_released){
//            world.saveWorldToFile("world1");
//        }
//    }
//}
//
//void FileMenuGUI::drawFixed(){
//
//    if(enabled){
//        for(vector<RectangleShape>::iterator parser = fixed_rectangles.begin(); parser != fixed_rectangles.end(); parser++){
//            window.draw(*parser);
//        }
//        for(vector<Caption>::iterator parser = captions.begin(); parser != captions.end(); parser++){
//            parser->draw();
//        }
//    }
//}
//
//SuitMetricsGUI::SuitMetricsGUI(){
//
//    gui_type = "suit metrics";
//
//    heartbeat.setTexture(getProperties("Heartbeat")->sheet, false);
//    IntRect frame_rect(0,0,getProperties("Heartbeat")->get("width"),getProperties("Heartbeat")->get("height"));
//    heartbeat.setTextureRect(frame_rect);
//    heartbeat.setOrigin(frame_rect.width/2,frame_rect.height/2);
//    heartbeat.setScale(0.5,0.5);
//    heartbeat.setColor(Color(255,255,255,200));
//
//    center = Vector2f(55,55);
//    heartbeat.setPosition(center);
//}
//void SuitMetricsGUI::update(double dt){
//
//    if(enabled){
//        mouse_alerts.clear();
//        fixed_rectangles.clear();
//        fixed_circles.clear();
//        captions.clear();
//
//        shared_ptr<Player> player = getSector(input.sector_id)->people[input.player_id].lock();
//        double health = player->getHealthPercentage();
//
//        if(health > 75){
//            animator.fps = 20;
//            animator.play(1,20);
//        }
//        else if(health > 40){
//            animator.fps = 24;
//            animator.play(21,40);
//        }
//        else if(health > 10){
//            animator.fps = 28;
//            animator.play(41,60);
//        }
//        else if(health > 0){
//            animator.fps = 20;
//            animator.play(61,80);
//        }
//        else{
//            animator.fps = 24;
//            animator.play(0,0);
//        }
//
//        animator.update(dt, heartbeat);
//
//        //build hud
//        CircleShape beat_circle;
//        beat_circle.setFillColor(Color(0,155,155,25));
//        beat_circle.setOutlineColor(Color(0,155,155,155));
//        beat_circle.setOutlineThickness(6);
//        beat_circle.setRadius(37.25);
//        beat_circle.setOrigin(37.25,37.25);
//        beat_circle.setPosition(center);
//        fixed_circles.push_back(beat_circle);
//
//        int number_of_bars = 25;
//        double degrees_per_bar = 150/number_of_bars;
//        number_of_bars = ceil(number_of_bars*(health/100));
//
//        for(int i = 0; i < number_of_bars; i++){
//            RectangleShape bar;
//            bar.setSize(Vector2f(3,15));
//            bar.setOrigin(1.5,7.5);
//            bar.setRotation(210-degrees_per_bar*i);
//            double Q = bar.getRotation() * (M_PI/180);
//            Vector2f bar_position = center + Vector2f(57.25*sin(Q),-57.25*cos(Q));
//            bar.setPosition(bar_position);
//
//            bar.setFillColor(Color(255,255*(health/100),0,155));
//            bar.setOutlineThickness(0);
//
//            fixed_rectangles.push_back(bar);
//        }
//    }
//}
//void SuitMetricsGUI::drawFixed(){
//
//    if(enabled){
//        window.draw(heartbeat);
//        for(vector<RectangleShape>::iterator parser = fixed_rectangles.begin(); parser != fixed_rectangles.end(); parser++){
//            window.draw(*parser);
//        }
//        for(vector<CircleShape>::iterator parser = fixed_circles.begin(); parser != fixed_circles.end(); parser++){
//            window.draw(*parser);
//        }
//        for(vector<Caption>::iterator parser = captions.begin(); parser != captions.end(); parser++){
//            parser->draw();
//        }
//    }
//}
//
//
//ToolModuleGUI::ToolModuleGUI(){ gui_type = "tool module"; }
//void ToolModuleGUI::update(double dt){
//
//    mouse_alerts.clear();
//    rectangles.clear();
//    fixed_rectangles.clear();
//    fixed_sprites.clear();
//    captions.clear();
//
//    if(input.keys_released.count("1") != 0){ tool_type = "welder"; }
//    if(input.keys_released.count("2") != 0){ tool_type = "riveter"; }
//    if(input.keys_released.count("3") != 0){ tool_type = "epoxy"; }
//    if(input.keys_released.count("4") != 0){ tool_type = "jackhammer"; }
//
//    if(tool_type == "welder"){
//
//        if(input.keys_held.count("space") != 0){
//
//            shared_ptr<Player> player = getSector(input.sector_id)->people[input.player_id].lock();
//
//            double Q = player->getRotation()*(M_PI/180);
//            Vector2f effect_position = player->getPosition() + Vector2f(24*cos(Q)+18*sin(Q),24*sin(Q)-18*cos(Q));
//
//            castLight(player->sector_id, effect_position, 0.2, 45);
//
//            double neg_x = 1;
//            double neg_y = 1;
//            if(rand() % 100<50){neg_x=-1;}
//            if(rand() % 100<50){neg_y=-1;}
//            Vector2f excitement(neg_x*(rand()%5),neg_y*(rand()%5));
//
//            double flame_velocity = rand() % 70;
//            Vector2f player_axis = player->getMyParallelAxis();
//
//
//            createEffect(player->sector_id, effect_position, player->getVelocity()+(player_axis*flame_velocity)+excitement, "Sparks");
//
//
//
//
//           // applyHeat(player->sector_id, 25*dt, effect_position + (player_axis*25), 25);
//
//        }
//    }
//    if(tool_type == "riveter"){
//
//        if(input.keys_released.count("space") != 0){
//
//            shared_ptr<Player> player = getSector(input.sector_id)->people[input.player_id].lock();
//
//            double Q = player->getRotation()*(M_PI/180);
//            Vector2f effect_position = player->getPosition() + Vector2f(24*cos(Q)+18*sin(Q),24*sin(Q)-18*cos(Q));
//            Vector2f player_axis = player->getMyParallelAxis();
//            createEffect(input.sector_id, effect_position, player->getVelocity(), "Smoke");
//            makeNewProjectile(input.sector_id, createUniqueId(), effect_position, player->getVelocity()+(player_axis*250), "Rivet");
//            player->applyImpulse(-player_axis.x*250*getProperties("Rivet")->get("mass"), -player_axis.y*250*getProperties("Rivet")->get("mass"), false);
//        }
//    }
//    if(tool_type == "epoxy"){
//
//        if(input.keys_released.count("space") != 0){
//
//            shared_ptr<Player> player = getSector(input.sector_id)->people[input.player_id].lock();
//
//            double Q = player->getRotation()*(M_PI/180);
//            Vector2f effect_position = player->getPosition() + Vector2f(24*cos(Q)+18*sin(Q),24*sin(Q)-18*cos(Q));
//            Vector2f player_axis = player->getMyParallelAxis();
//            makeNewProjectile(input.sector_id, createUniqueId(), effect_position, player->getVelocity()+(player_axis*150), "Epoxy");
//            player->applyImpulse(-player_axis.x*250*getProperties("Epoxy")->get("mass"), -player_axis.y*150*getProperties("Epoxy")->get("mass"), false);
//        }
//    }
//
//}
//void ToolModuleGUI::draw(){
//
//    if(enabled){
//        for(vector<RectangleShape>::iterator parser = rectangles.begin(); parser != rectangles.end(); parser++){
//            window.draw(*parser);
//        }
//    }
//}
//void ToolModuleGUI::drawFixed(){
//
//    if(enabled){
//        for(vector<RectangleShape>::iterator parser = fixed_rectangles.begin(); parser != fixed_rectangles.end(); parser++){
//            window.draw(*parser);
//        }
//        for(vector<Sprite>::iterator parser = fixed_sprites.begin(); parser != fixed_sprites.end(); parser++){
//            window.draw(*parser);
//        }
//        for(vector<Caption>::iterator parser = captions.begin(); parser != captions.end(); parser++){
//            parser->draw();
//        }
//    }
//}
//
//
//
