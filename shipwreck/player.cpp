#include "main.h"

using namespace std;
using namespace sf;


extern World world;
extern RenderWindow window;

//Module::Module(){}
//void Module::evaluateInput(InputData input){}
//void Module::updateGUI(){}
//void Module::updateHUD(){}
//void Module::drawGUI(){
//
//    for(vector<RectangleShape>::iterator parser = gui_rectangles.begin(); parser != gui_rectangles.end(); parser++){
//        window.draw(*parser);
//    }
//    for(vector<Caption>::iterator parser = gui_captions.begin(); parser != gui_captions.end(); parser++){
//        parser->draw();
//    }
//}
//void Module::drawHUD(Vector2f top_left){
//
//    for(vector<RectangleShape>::iterator parser = hud_rectangles.begin(); parser != hud_rectangles.end(); parser++){
//        RectangleShape adjusted_shape = *parser;
//        adjusted_shape.setPosition(adjusted_shape.getPosition() + top_left);
//        window.draw(adjusted_shape);
//    }
//    for(vector<Caption>::iterator parser = hud_captions.begin(); parser != hud_captions.end(); parser++){
//        Caption adjusted_caption = *parser;
//        adjusted_caption.text.setPosition(adjusted_caption.text.getPosition() + top_left);
//        adjusted_caption.draw();
//    }
//}
//
//WeldingModule::WeldingModule(){
//
//    var_str["current_ent_id"] = "";
//    var_str["sector_id"] = "";
//    var_str["player_id"] = "";
//    var_str["ent_id_a"] = "";
//    var_str["ent_id_b"] = "";
//}
//
//void WeldingModule::evaluateInput(InputData input){
//
//    var_str["current_ent_id"] = input.hovered_over_id;
//    var_str["sector_id"] = input.sector_id;
//    var_str["player_id"] = input.player_id;
//
//    if(var_str["ent_id_a"] == ""){
//
//        if(var_str["current_ent_id"] != ""){
//
//            if(input.keys_released.count("t") != 0){
//
//                var_str["ent_id_a"] = var_str["current_ent_id"];
//            }
//        }
//    }
//    else if(var_str["ent_id_b"] == ""){
//
//        if(var_str["current_ent_id"] != ""){
//
//            if(input.keys_released.count("t") != 0){
//
//                var_str["ent_id_b"] = var_str["current_ent_id"];
//            }
//        }
//    }
//    else{
//
//        makeNewConnector(input.sector_id, createUniqueId(), "Weld", var_str["ent_id_a"], var_str["ent_id_b"]);
//        var_str["ent_id_a"] = "";
//        var_str["ent_id_b"] = "";
//    }
//}
//
//void WeldingModule::updateGUI(){
//
//    gui_rectangles.clear();
//    gui_captions.clear();
//
//    if(var_str["ent_id_a"] != ""){
//        gui_rectangles.push_back(createBoundingRectangle(getSector(var_str["sector_id"])->getEnt(var_str["ent_id_a"])->sprite, Color(25,255,255,155)));
//    }
//    if(var_str["current_ent_id"] != ""){
//        gui_rectangles.push_back(createBoundingRectangle(getSector(var_str["sector_id"])->getEnt(var_str["current_ent_id"])->sprite, Color(25,255,255,155)));
//    }
//
//}
//
//void WeldingModule::updateHUD(){
//
//    hud_rectangles.clear();
//    hud_captions.clear();
//
//    RectangleShape main_box;
//    main_box.setSize(Vector2f(200,50));
//    main_box.setFillColor(Color(0,155,155,25));
//    main_box.setOutlineColor(Color(0,155,155,155));
//    main_box.setOutlineThickness(3);
//    main_box.setPosition(0,0);
//    hud_rectangles.push_back(main_box);
//
//
//    if(sin(world.date.second*6) > 0){
//        //blink on
//        RectangleShape light;
//        light.setSize(Vector2f(5,5));
//        light.setFillColor(Color(200,255,200,255));
//        light.setOutlineThickness(0);
//        light.setPosition(10,10);
//        hud_rectangles.push_back(light);
//
//        hud_captions.push_back(Caption("Press T to weld.", "font1", Vector2f(100,10), 24));
//    }
//    else{
//        //blink off
//    }
//}
//
//JetpackModule::JetpackModule(){
//
//    var_str["status"] = "on";
//    var_str["sector_id"] = "";
//    var_str["player_id"] = "";
//}
//
//void JetpackModule::evaluateInput(InputData input){
//
//    var_str["sector_id"] = input.sector_id;
//    var_str["player_id"] = input.player_id;
//
//
//    if(input.keys_released.count("j") != 0){
//        if(var_str["status"] == "off"){ var_str["status"] = "on"; }
//        else{ var_str["status"] = "off"; }cc
//    }
//
//    if(var_str["status"] == "on"){
//         Vector2f axis = getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getMyParallelAxis();
//
//         double neg_x = 1;
//         double neg_y = 1;
//         if(rand() % 100<50){neg_x=-1;}
//         if(rand() % 100<50){neg_y=-1;}
//
//         double x_excitement = neg_x*(rand()%50);
//         double y_excitement = neg_y*(rand()%50);
//
//         if(input.keys_held.count("w") != 0){
//             getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->applyImpulse(input.dt*300*getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getMass()*axis.x,input.dt*800*getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getMass()*axis.y);
//             createEffect(var_str["sector_id"],getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getPosition() - (axis*getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->collision_radius),getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getVelocity() + Vector2f(x_excitement+(-100*axis.x),y_excitement+(-100*axis.y)),"Smoke");
//         }
//         if(input.keys_held.count("s") != 0){
//             getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->applyImpulse(input.dt*-300*getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getMass()*axis.x,input.dt*-800*getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getMass()*axis.y);
//             createEffect(var_str["sector_id"],getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getPosition() - (axis*getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->collision_radius),getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getVelocity() + Vector2f(x_excitement+(25*axis.x),y_excitement+(25*axis.y)) + (getPerpendicularAxis(axis)*300),"Smoke");
//             createEffect(var_str["sector_id"],getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getPosition() - (axis*getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->collision_radius),getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getVelocity() + Vector2f(x_excitement+(25*axis.x),y_excitement+(25*axis.y)) + (getPerpendicularAxis(axis)*-300),"Smoke");
//         }
//         if(input.keys_held.count("q") != 0){
//             getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->applyRotationalImpulse(input.dt*-180*getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getMass());
//             createEffect(var_str["sector_id"],getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getPosition() - (axis*getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->collision_radius),getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getVelocity() + Vector2f(x_excitement,y_excitement),"Smoke");
//         }
//         if(input.keys_held.count("e") != 0){
//             getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->applyRotationalImpulse(input.dt*180*getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getMass());
//             createEffect(var_str["sector_id"],getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getPosition() - (axis*getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->collision_radius),getSector(var_str["sector_id"])->getEnt(var_str["player_id"])->getVelocity() + Vector2f(x_excitement,y_excitement),"Smoke");
//         }
//    }
//}
//
//GrabbingModule::GrabbingModule(){
//
//    var_str["current_ent_id"] = "";
//    var_str["sector_id"] = "";
//    var_str["player_id"] = "";
//}
//
//void GrabbingModule::evaluateInput(InputData input){
//
//    var_str["current_ent_id"] = input.hovered_over_id;
//    var_str["sector_id"] = input.sector_id;
//    var_str["player_id"] = input.player_id;
//
//
//    if(input.keys_released.count("g") != 0){
//        getSector(var_str["sector_id"])->people[var_str["player_id"]].lock()->grab(var_str["current_ent_id"]);
//    }
//}
//
//void GrabbingModule::updateGUI(){
//
//    gui_rectangles.clear();
//    gui_captions.clear();
//
//    if(var_str["current_ent_id"] != ""){
//        gui_rectangles.push_back(createBoundingRectangle(getSector(var_str["sector_id"])->getEnt(var_str["current_ent_id"])->sprite, Color(25,255,255,155)));
//    }
//
//}
//
//void GrabbingModule::updateHUD(){
//
//    hud_rectangles.clear();
//    hud_captions.clear();
//
//    RectangleShape main_box;
//    main_box.setSize(Vector2f(200,50));
//    main_box.setFillColor(Color(0,155,155,25));
//    main_box.setOutlineColor(Color(0,155,155,155));
//    main_box.setOutlineThickness(3);
//    main_box.setPosition(0,0);
//    hud_rectangles.push_back(main_box);
//
//
//    if(sin(world.date.second*6) > 0){
//        //blink on
//        RectangleShape light;
//        light.setSize(Vector2f(5,5));
//        light.setFillColor(Color(200,255,200,255));
//        light.setOutlineThickness(0);
//        light.setPosition(10,10);
//        hud_rectangles.push_back(light);
//
//        hud_captions.push_back(Caption("Press G to grab.", "font1", Vector2f(100,10), 24));
//    }
//    else{
//        //blink off
//    }
//}

Player::Player()
{

}

Player::Player(string new_sector_id, string e_id, double x, double y, string type)
{

    constructEntity(new_sector_id, e_id, x, y, type);
    setSlots(9);

    range = 225;
}


void Player::walk(Direction d,double dt)
{
    if(slaved){return;}

    double walk_speed = 400;
    double jetpack_speed = 200;

    if(jetpack_on){
        Vector2f axis = getMyParallelAxis();
        Vector2f perp_axis = getMyPerpendicularAxis();

        double neg_x = 1;
        double neg_y = 1;
        if(rand() % 100<50){neg_x=-1;}
        if(rand() % 100<50){neg_y=-1;}

        double x_excitement = neg_x*(rand()%50);
        double y_excitement = neg_y*(rand()%50);

        if(d==Forward){
            Vector2f v_difference = (axis*-1*jetpack_speed);
          //  if(v_difference.x/(axis.x) > 0){v_difference.x = 0;}
           // if(v_difference.y/(axis.y) > 0){v_difference.y = 0;}

            Vector2f p_to_add = v_difference*getMyMass()*5*dt;

            applyImpulse(-p_to_add.x,-p_to_add.y,getPosition(),false,true);
            if(sin(world.date.second*60) - 1 > -0.05){
                createEffect(sector_id,getPosition() - (axis*collision_radius),getVelocity() + Vector2f(x_excitement+(-100*axis.x),y_excitement+(-100*axis.y)),"Smoke");
            }
        }
        else if(d==Backward){
            Vector2f v_difference = (axis*jetpack_speed);
         //   if(v_difference.x/(-axis.x) > 0){v_difference.x = 0;}
         //   if(v_difference.y/(-axis.y) > 0){v_difference.y = 0;}

            Vector2f p_to_add = v_difference*getMyMass()*5*dt;

            applyImpulse(-p_to_add.x,-p_to_add.y,getPosition(),false,true);
            if(sin(world.date.second*60) - 1 > -0.05){
                createEffect(sector_id,getPosition() - (axis*collision_radius),getVelocity() + Vector2f(x_excitement+(125*axis.x),y_excitement+(125*axis.y)) + (getPerpendicularAxis(axis)*300),"Smoke");
                createEffect(sector_id,getPosition() - (axis*collision_radius),getVelocity() + Vector2f(x_excitement+(125*axis.x),y_excitement+(125*axis.y)) + (getPerpendicularAxis(axis)*-300),"Smoke");
            }
        }
        else if(d==Right){
            Vector2f v_difference = (perp_axis*-1*jetpack_speed);
         //   if(v_difference.x/(perp_axis.x) > 0){v_difference.x = 0;}
         //   if(v_difference.y/(perp_axis.y) > 0){v_difference.y = 0;}

            Vector2f p_to_add = v_difference*getMyMass()*5*dt;

            applyImpulse(-p_to_add.x,-p_to_add.y,getPosition(),false,true);
            if(sin(world.date.second*60) - 1 > -0.05){
                createEffect(sector_id,getPosition() - (perp_axis*collision_radius),getVelocity() + Vector2f(x_excitement+(-100*perp_axis.x),y_excitement+(-100*perp_axis.y)),"Smoke");
            }
        }
        else if(d==Left){
            Vector2f v_difference = (perp_axis*jetpack_speed);
         //   if(v_difference.x/(-perp_axis.x) > 0){v_difference.x = 0;}
         //   if(v_difference.y/(-perp_axis.y) > 0){v_difference.y = 0;}

            Vector2f p_to_add = v_difference*getMyMass()*5*dt;

            applyImpulse(-p_to_add.x,-p_to_add.y,getPosition(),false,true);
            if(sin(world.date.second*60) - 1 > -0.05){
                createEffect(sector_id,getPosition() + (perp_axis*collision_radius),getVelocity() + Vector2f(x_excitement+(100*perp_axis.x),y_excitement+(100*perp_axis.y)),"Smoke");
            }
        }
    }
    else{
        animator.play(16,31);//play walking animation here
        int modifier = 1;
        if(d==Backward or d==Left){modifier=-1;}

        Vector2f axis;
        if(d==Left or d==Right){axis=getMyPerpendicularAxis();}
        else{axis=getMyParallelAxis();}

        //THIS PREVENTS WALKING WHEN ON A HIGH-SPEED PLATFORM!!! BAD!!!
        Vector2f floor_velocity = Vector2f(0,0);
        if(floors.size()!=0){

            Vector2f relative_point = getPosition() - getSector(sector_id)->getEnt(*floors.begin())->getPosition();
            floor_velocity = getSector(sector_id)->getEnt(*floors.begin())->getVelocity() + (getPerpendicularAxis(relative_point)*hypot(relative_point.x,relative_point.y)*(getSector(sector_id)->getEnt(*floors.begin())->getRotationalVelocity()*(M_PI/180.0)));
        }
        Vector2f v_difference = getVelocity()-(floor_velocity+(axis*modifier*walk_speed));
        if(v_difference.x/(axis.x*modifier) > 0){v_difference.x = 0;}
        if(v_difference.y/(axis.y*modifier) > 0){v_difference.y = 0;}

        Vector2f p_to_add = v_difference*getMyMass()*10*dt;

        if((getSector(sector_id)->planetside || floors.size()!=0) and (not bonded or floor_bonds.count(bond)==0))
        {
            applyImpulse(-p_to_add.x,-p_to_add.y,getPosition(),false,true);
            for(set<string>::iterator parser = floors.begin(); parser != floors.end(); parser++){
                if(shared_ptr<Entity> floor_ent = getSector(sector_id)->getEnt(*parser)){
                    if(!ghost){
                        floor_ent->applyImpulse(p_to_add.x/floors.size(),p_to_add.y/floors.size(),getPosition(),false,false);
                    }
                    
                }
            }
        }
    }
}

void Player::turn(Direction d,double dt)
{
    if(slaved){return;}

    double jetpack_speed = 90;
    double turn_speed = 160;

    if(jetpack_on){

        Vector2f axis = getMyParallelAxis();
        Vector2f perp_axis = getMyPerpendicularAxis();

        double neg_x = 1;
        double neg_y = 1;
        if(rand() % 100<50){neg_x=-1;}
        if(rand() % 100<50){neg_y=-1;}

        double x_excitement = neg_x*(rand()%50);
        double y_excitement = neg_y*(rand()%50);

        if(d==Counterclockwise){
            double difference = getRotationalVelocity() - (-jetpack_speed);
            double p_to_add = difference*getMyInertia()*5*dt;
            applyRotationalImpulse(-p_to_add);
            if(sin(world.date.second*60) - 1 > -0.05){
                createEffect(sector_id,getPosition() - (axis*collision_radius),getVelocity() + Vector2f(x_excitement,y_excitement),"Smoke");
            }
        }
        else if(d==Clockwise){
            double difference = getRotationalVelocity() - (jetpack_speed);
            double p_to_add = difference*getMyInertia()*5*dt;
            applyRotationalImpulse(-p_to_add);
            if(sin(world.date.second*60) - 1 > -0.05){
                createEffect(sector_id,getPosition() - (axis*collision_radius),getVelocity() + Vector2f(x_excitement,y_excitement),"Smoke");
            }
        }
    }
    else{

        animator.play(16,31);

        int modifier = 1;
        if(d==Counterclockwise){modifier=-1;}

        double difference = getRotationalVelocity() - (modifier*turn_speed);
        double p_to_add = difference*getMyInertia()*10*dt;

        if((getSector(sector_id)->planetside || floors.size()!=0) and (not bonded or floor_bonds.count(bond)==0))
        {
            applyRotationalImpulse(-p_to_add);
        }
    }
}

void Player::grab(string ent_id){

    for(set<string>::iterator parser = connectors.begin(); parser!=connectors.end(); parser++){
        if(getSector(sector_id)->connectors[*parser]->type=="GRAB"){
            ent_id = "";
            getSector(sector_id)->trash(*parser);
        }
    }
    if(ent_id!=""){
        makeNewConnector(sector_id, createUniqueId(), "GRAB",id,ent_id);
    }

}

void Player::run(double dt)
{
    runPhysicsAndGraphics(dt);
    if(flashlight_on){
        castLight(sector_id, getPosition(), 0.8, 155);
    }
    if(jetpack_on && not slaved){
        if(sin(world.date.second*60) - 1 > -0.005){
            createEffect(sector_id,getPosition() - (getMyParallelAxis()*20),getVelocity(),"Smoke");
        }
    }
    animator.play(0,0);
}

string makeNewPlayer(string sector_id, string ent_id, double x, double y, double rotation, string type){

    shared_ptr<Player> new_player = make_shared<Player>(sector_id, ent_id, x, y, type);
    getSector(sector_id)->people[ent_id] = weak_ptr<Player>(new_player);
    getSector(sector_id)->ents[ent_id] = new_player;
    getSector(sector_id)->ents[ent_id]->setRotation(rotation);
    return ent_id;
}

