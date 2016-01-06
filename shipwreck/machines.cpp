#include "main.h"

using namespace std;
using namespace sf;


extern World world;
extern RenderWindow window;

GasTank::GasTank(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Gas Tank");

    internal_cells["Stored Gas"] = make_shared<GasCell>("Stored Gas", 100);
    addPort("Hose Fitting", Vector2f(43,-26), 90,"Hose Fitting");
    ports["Hose Fitting"]->tapCell("Stored Gas");

    addPort("Hose Fitting 2", Vector2f(43,12), 90,"Hose Fitting");
    ports["Hose Fitting 2"]->tapCell("Stored Gas");
}

GasVent::GasVent(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Gas Vent");

    internal_cells["Stored Gas"] = make_shared<GasCell>("Stored Gas", 1);
    addPort("Hose Fitting", Vector2f(43,0), 90,"Hose Fitting");
    ports["Hose Fitting"]->tapCell("Stored Gas");
}

bool GasVent::performFunction(double dt){

    internal_cells["Stored Gas"]->recalculate();

    if(internal_cells["Stored Gas"]->vars["total kilomoles"] > 0){
        animator.play(17,31);
    }
    else{
        animator.play(0,0);
    }

    if(internal_cells["Stored Gas"]->vars["total kilomoles"] > 0){
        internal_cells["Stored Gas"]->take(internal_cells["Stored Gas"]->vars["total kilograms"]);

        double neg_x = 1;
        double neg_y = 1;
        if(rand() % 100<50){neg_x=-1;}
        if(rand() % 100<50){neg_y=-1;}

        double x_excitement = neg_x*(rand()%200);
        double y_excitement = neg_y*(rand()%200);

        createEffect(sector_id, getPosition(), Vector2f(x_excitement, y_excitement), "Smoke");


    }

    return true;
}

WarpDrive::WarpDrive(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Warp Drive");

    voltage_rating = 12.0;
    wattage = 350;

    internal_cells["Buffered Power"] = make_shared<PowerCell>("Buffered Power",10);
    addPort("Neutral Rail", Vector2f(-90,53), 0,"Power Port");
    ports["Neutral Rail"]->tapCell("Buffered Power");
    ports["Neutral Rail"]->voltage = 0.0;

    internal_cells["Warp Target"] = make_shared<FlowCell>();
    internal_cells["Warp Target"]->id = "Warp Target";
    internal_cells["Warp Target"]->vars["x"] = 0;
    internal_cells["Warp Target"]->vars["y"] = 0;

    internal_cells["Cooldown"] = make_shared<FlowCell>();
    internal_cells["Cooldown"]->id = "Cooldown";
    internal_cells["Cooldown"]->vars["seconds"] = 0;

    internal_cells["Warp Trigger Input"] = make_shared<GPIOCell>("Warp Trigger Input");
    addPort("GPI Warp Trigger", Vector2f(-91,82), 180,"GPI Port");
    ports["GPI Warp Trigger"]->tapCell("Warp Trigger Input");
}

bool WarpDrive::WarpDrive::performFunction(double dt){

    double amps = wattage/voltage_rating;
    double milliamp_hours_needed = ((1000*amps/60)/60)*dt;

    if(internal_cells["Buffered Power"]->vars["milliamp hours"]<0.01){internal_cells["Buffered Power"]->vars["milliamp hours"]=0;}

    if(internal_cells["Buffered Power"]->vars["milliamp hours"] >= milliamp_hours_needed){

        internal_cells["Buffered Power"]->vars["milliamp hours"] -= milliamp_hours_needed;

        double v = hypot(getVelocity().x,getVelocity().y);
        double warp_speed = 1500;
        double cooldown_period = 60; //seconds

        double percent = (v/warp_speed)*100;

        double Q = getRotation()*(M_PI/180);
        Vector2f n_position = getPosition() + Vector2f(-65*sin(Q),65*cos(Q));
        Vector2f e_position = getPosition() + Vector2f(65*cos(Q),65*sin(Q));
        Vector2f s_position = getPosition() + Vector2f(-90*sin(Q),90*cos(Q));
        Vector2f w_position = getPosition() + Vector2f(-65*cos(Q),-65*sin(Q));

        castLight(sector_id, s_position, 2, 80);

        if(internal_cells["Cooldown"]->vars["seconds"] <= 0){
            if(percent >= 100){
                animator.play(29,29);
            }
            else if(percent >= 92){
                animator.play(28,28);
            }
            else if(percent >= 84){
                animator.play(27,27);
            }
            else if(percent >= 76){
                animator.play(26,26);
            }
            else if(percent >= 68){
                animator.play(25,25);
            }
            else if(percent >= 60){
                animator.play(24,24);
            }
            else if(percent >= 52){
                animator.play(23,23);
            }
            else if(percent >= 44){
                animator.play(22,22);
            }
            else if(percent >= 36){
                animator.play(21,21);
            }
            else if(percent >= 28){
                animator.play(20,20);
            }
            else if(percent >= 20){
                animator.play(19,19);
            }
            else if(percent >= 12){
                animator.play(18,18);
            }
            else if(percent >= 4){
                animator.play(17,17);
            }
            else{
                animator.play(16,16);

            }

            string target_sector = asString(internal_cells["Warp Target"]->vars["x"])+"_"+asString(internal_cells["Warp Target"]->vars["y"]);

            if(percent>=100){
                if(internal_cells["Warp Trigger Input"]->vars["value"] == 1){
                    //warp
                    if(sector_id != target_sector){
                        if(world.sectors.count(target_sector)==0){
                            //create new blank space sector
                            world.sectors[target_sector] =  make_shared<Sector>(target_sector, 10000, "backdrops/space_default.png", 55, false);
                        }
                        Vector2f spawn = getVelocity()*-12.0; //spawn 12 seconds away from the center of the target sector
                        world.transfer(sector_id, id, target_sector, spawn);
                        internal_cells["Cooldown"]->vars["seconds"] = cooldown_period;
                        createEffect(sector_id, getPosition(), getVelocity(), "Warp");
                        createEffect(target_sector, spawn, getVelocity(), "Warp");
                    }

                }
            }
        }
        else{

            double c_percent = (internal_cells["Cooldown"]->vars["seconds"]/cooldown_period)*100;
            if(c_percent >= 100){
                animator.play(32,32);
            }
            else if(c_percent >= 92){
                animator.play(33,33);
            }
            else if(c_percent >= 84){
                animator.play(34,34);
            }
            else if(c_percent >= 76){
                animator.play(35,35);
            }
            else if(c_percent >= 68){
                animator.play(36,36);
            }
            else if(c_percent >= 60){
                animator.play(37,37);
            }
            else if(c_percent >= 52){
                animator.play(38,38);
            }
            else if(c_percent >= 44){
                animator.play(39,39);
            }
            else if(c_percent >= 36){
                animator.play(40,40);
            }
            else if(c_percent >= 28){
                animator.play(41,41);
            }
            else if(c_percent >= 20){
                animator.play(42,42);
            }
            else if(c_percent > 12){
                animator.play(43,43);
            }
            else{
                animator.play(44,44);
            }

            double neg_x = 1;
            double neg_y = 1;
            if(rand() % 100<50){neg_x=-1;}
            if(rand() % 100<50){neg_y=-1;}

            double x_excitement = neg_x*(rand()%50);
            double y_excitement = neg_y*(rand()%50);

            internal_cells["Cooldown"]->vars["seconds"]-=dt;
            createEffect(sector_id, getPosition() + Vector2f(-133*cos(Q),-133*sin(Q)), getVelocity() + Vector2f(-90*sin(Q),90*cos(Q)) + Vector2f(x_excitement, y_excitement),"Smoke");
            if(internal_cells["Cooldown"]->vars["seconds"] <= 0){
                internal_cells["Cooldown"]->vars["seconds"] = 0;
            }
        }
    }
    else{
        animator.play(0,0);
    }

    return true;
}

GasValve::GasValve(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Gas Valve");

    internal_cells["Stored Gas"] = make_shared<GasCell>("Stored Gas", 1);
    addPort("Hose Fitting", Vector2f(19,2), 90,"Hose Fitting");
    ports["Hose Fitting"]->tapCell("Stored Gas");
    addPort("Hose Fitting 2", Vector2f(-19,3), -90,"Hose Fitting");
    ports["Hose Fitting 2"]->tapCell("Stored Gas");

    internal_cells["Valve State Input"] = make_shared<GPIOCell>("Valve State Input");
    addPort("GPI Valve State", Vector2f(0,15), 180,"GPI Port");
    ports["GPI Valve State"]->tapCell("Valve State Input");
}

bool GasValve::performFunction(double dt){

    internal_cells["Stored Gas"]->recalculate();

    if(internal_cells["Valve State Input"]->vars["value"] == 0){
        ports["Hose Fitting"]->closed = false;
        ports["Hose Fitting 2"]->closed = false;
    }
    else if(internal_cells["Valve State Input"]->vars["value"] == 1){
        ports["Hose Fitting"]->closed = true;
        ports["Hose Fitting 2"]->closed = true;
    }

    return true;
}

PipeJoint::PipeJoint(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Pipe Joint");

    internal_cells["Stored Gas"] = make_shared<GasCell>("Stored Gas", 1);
    addPort("Hose Fitting", Vector2f(-1,-17), 0,"Hose Fitting");
    ports["Hose Fitting"]->tapCell("Stored Gas");
    addPort("Hose Fitting 2", Vector2f(19,2), 90,"Hose Fitting");
    ports["Hose Fitting 2"]->tapCell("Stored Gas");
    addPort("Hose Fitting 3", Vector2f(-19,3), -90,"Hose Fitting");
    ports["Hose Fitting 3"]->tapCell("Stored Gas");
}

Switch::Switch(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Switch");

    voltage_rating = 0.0;

    internal_cells["Buffered Power 1"] = make_shared<PowerCell>("Buffered Power 1",0.25);
    addPort("Rail 1", Vector2f(-9,0), 0,"Power Port");
    ports["Rail 1"]->tapCell("Buffered Power 1");
    internal_cells["Buffered Power 2"] = make_shared<PowerCell>("Buffered Power 2",0.25);
    addPort("Rail 2", Vector2f(9,0), 180,"Power Port");
    ports["Rail 2"]->tapCell("Buffered Power 2");

    internal_cells["Switch State Input"] = make_shared<GPIOCell>("Switch State Input");
    addPort("GPI Switch State", Vector2f(0,18), 180,"GPI Port");
    ports["GPI Switch State"]->tapCell("Switch State Input");

    ports["Rail 1"]->voltage = 0.0;
    ports["Rail 2"]->voltage = 0.0;
}

bool Switch::performFunction(double dt){

    if(internal_cells["Switch State Input"]->vars["value"]==1){

        ports["Rail 1"]->voltage = 0;
        ports["Rail 2"]->voltage = ports["Rail 1"]->voltage_in;

        double milliamp_hours_to_transfer = internal_cells["Buffered Power 1"]->vars["milliamp hours"];
        if(internal_cells["Buffered Power 2"]->vars["milliamp hours"] + milliamp_hours_to_transfer > internal_cells["Buffered Power 2"]->vars["max milliamp hours"]){
            milliamp_hours_to_transfer = internal_cells["Buffered Power 2"]->vars["max milliamp hours"] - internal_cells["Buffered Power 2"]->vars["milliamp hours"];
        }
        internal_cells["Buffered Power 2"]->vars["milliamp hours"] += milliamp_hours_to_transfer;
        internal_cells["Buffered Power 1"]->vars["milliamp hours"] -= milliamp_hours_to_transfer;
    }
    else if(internal_cells["Switch State Input"]->vars["value"]==0){

        ports["Rail 2"]->voltage = 0;
        ports["Rail 1"]->voltage = ports["Rail 2"]->voltage_in;

        double milliamp_hours_to_transfer = internal_cells["Buffered Power 2"]->vars["milliamp hours"];
        if(internal_cells["Buffered Power 1"]->vars["milliamp hours"] + milliamp_hours_to_transfer > internal_cells["Buffered Power 1"]->vars["max milliamp hours"]){
            milliamp_hours_to_transfer = internal_cells["Buffered Power 1"]->vars["max milliamp hours"] - internal_cells["Buffered Power 1"]->vars["milliamp hours"];
        }
        internal_cells["Buffered Power 1"]->vars["milliamp hours"] += milliamp_hours_to_transfer;
        internal_cells["Buffered Power 2"]->vars["milliamp hours"] -= milliamp_hours_to_transfer;
    }

    return true;
}

Piston::Piston(string new_sector_id, string e_id, double x, double y, double rotation, bool loading){

    constructEntity(new_sector_id, e_id, x, y, "Piston");

    voltage_rating = 12;

    double Q = rotation * (M_PI/180);

    internal_cells["Sister ID"] = make_shared<GPIOCell>("Sister ID");

    if(!loading){
        internal_cells["Sister ID"]->data["id"] = createUniqueId();

        makeNewMachine(sector_id, internal_cells["Sister ID"]->data["id"],  x + (34*sin(Q)), y + (-34*cos(Q)), rotation, "Piston Head", loading);
        getSector(sector_id)->getMachine(internal_cells["Sister ID"]->data["id"])->internal_cells["Sister ID"]->data["id"] = id;
    }

    internal_cells["Piston State Input"] = make_shared<GPIOCell>("Piston State Input");
    addPort("GPI Piston State", Vector2f(-12,29), 180,"GPI Port");
    ports["GPI Piston State"]->tapCell("Piston State Input");

    internal_cells["Buffered Power"] = make_shared<PowerCell>("Buffered Power",10);
    addPort("Neutral Rail", Vector2f(-12,-3), 0,"Power Port");
    ports["Neutral Rail"]->tapCell("Buffered Power");
}


void Piston::run(double dt){

    runPhysicsAndGraphics(dt);

    for(map<string, shared_ptr<FlowCell> >::iterator parser = internal_cells.begin(); parser != internal_cells.end(); parser++){
        parser->second->recalculate();
    }
    for(map<string, shared_ptr<FlowPort> >::iterator parser = ports.begin(); parser != ports.end(); parser++){

        parser->second->run(dt);
        /*if(not parser->second->closed and not parser->second->isConnected()){
            shared_ptr<FlowCell> cell = parser->second->getCell();
            cell->recalculate();
            double pressure_difference = getSector(sector_id)->getPressure(getPosition(),300) - cell->vars["total pressure"];
            if(cell->vars["total kilograms"]>0 and pressure_difference<0){

                eject(parser->first, (abs(pressure_difference)*cell->vars["volume"])*dt);
            }
            else if(pressure_difference>0){

                inject(parser->first, (abs(pressure_difference)*cell->vars["volume"])*dt);
            }
        }*/
    }
    if(getSector(sector_id)->bonds.count(bond) != 0){
         set<string> own_group =  getSector(sector_id)->bonds[bond]->getGroupWith(id);
        set<string> sister_group = getSector(sector_id)->bonds[bond]->getGroupWith(internal_cells["Sister ID"]->data["id"]);

        bool own_moveable = true;
        double own_mass = 0;
        for(set<string>::iterator parser = own_group.begin(); parser != own_group.end(); parser++){
            own_mass += getSector(sector_id)->getEnt(*parser)->getMyMass();
            if(getSector(sector_id)->getEnt(*parser)->prop){
                own_moveable = false;
            }
        }

        bool sister_moveable = true;
        double sister_mass = 0;
        for(set<string>::iterator parser = sister_group.begin(); parser != sister_group.end(); parser++){
            sister_mass += getSector(sector_id)->getEnt(*parser)->getMyMass();
            if(getSector(sector_id)->getEnt(*parser)->prop){
                sister_moveable = false;
            }
        }

        double own_percent = own_mass/(own_mass+sister_mass);
        double sister_percent = sister_mass/(own_mass+sister_mass);
        Vector2f extension_axis = getMyParallelAxis();
        Vector2f perp_axis = getMyPerpendicularAxis();

        if(!own_moveable){ own_percent = 1; sister_percent = 0; }
        if(!sister_moveable){ sister_percent = 1; own_percent = 0; }
        if(!(!own_moveable && !sister_moveable)){
            Vector2f average_position = (coords*own_percent)+(getSector(sector_id)->getEnt(internal_cells["Sister ID"]->data["id"])->coords*sister_percent);
            Vector2f own_disp = (average_position - coords);
            Vector2f sister_disp = (average_position - getSector(sector_id)->getEnt(internal_cells["Sister ID"]->data["id"])->coords);


            own_disp = perp_axis*((own_disp.x*perp_axis.x)+(own_disp.y*perp_axis.y));
            sister_disp = perp_axis*((sister_disp.x*perp_axis.x)+(sister_disp.y*perp_axis.y));



            getSector(sector_id)->bonds[bond]->displaceGroupContaining(id, own_disp);
            getSector(sector_id)->bonds[bond]->displaceGroupContaining(internal_cells["Sister ID"]->data["id"], sister_disp);
        }

    }
  //  performFunction(dt);


}

bool Piston::performFunction(double dt){

    double target_seperation = 0;
    if(internal_cells["Piston State Input"]->vars["value"] == 1){
        target_seperation = 100;
    }

    Vector2f extension_axis = getMyParallelAxis();

    Vector2f seperation_vector = getSector(sector_id)->getEnt(internal_cells["Sister ID"]->data["id"])->coords - coords;
    double seperation = hypot(seperation_vector.x,seperation_vector.y) - ((sprite.getLocalBounds().height/2.0)+(getSector(sector_id)->getEnt(internal_cells["Sister ID"]->data["id"])->sprite.getLocalBounds().height/2.0));

    double sign = 0;
    if(abs(target_seperation-seperation)>=1){
        sign = (target_seperation - seperation)/abs(target_seperation - seperation);
    }



    double extension_rate = sign*100;
    Vector2f disp = extension_axis*dt*extension_rate;

    if(sign != 0){ animator.play(3,5);}
    else{ animator.play(0,0); }

    if(getSector(sector_id)->bonds.count(bond) != 0){

        set<string> own_group =  getSector(sector_id)->bonds[bond]->getGroupWith(id);
        set<string> sister_group = getSector(sector_id)->bonds[bond]->getGroupWith(internal_cells["Sister ID"]->data["id"]);

        bool own_moveable = true;
        double own_mass = 0;
        for(set<string>::iterator parser = own_group.begin(); parser != own_group.end(); parser++){
            own_mass += getSector(sector_id)->getEnt(*parser)->getMyMass();
            if(getSector(sector_id)->getEnt(*parser)->prop){
                own_moveable = false;
            }
        }

        bool sister_moveable = true;
        double sister_mass = 0;
        for(set<string>::iterator parser = sister_group.begin(); parser != sister_group.end(); parser++){
            sister_mass += getSector(sector_id)->getEnt(*parser)->getMyMass();
            if(getSector(sector_id)->getEnt(*parser)->prop){
                sister_moveable = false;
            }
        }

        double own_percent = own_mass/(own_mass+sister_mass);
        double sister_percent = sister_mass/(own_mass+sister_mass);

        if(!own_moveable){ own_percent = 1; sister_percent = 0; }
        if(!sister_moveable){ own_percent = 0; sister_percent = 1; }
        if(!(!own_moveable && !sister_moveable)){

            double ent_collisions_before = 0;
            for(set<string>::iterator parser = getSector(sector_id)->bonds[bond]->members.begin(); parser != getSector(sector_id)->bonds[bond]->members.end(); parser++){
                if(getSector(sector_id)->ents.count(*parser)==0){continue;}
                for(set<string>::iterator parser_2 = parser; parser_2 != getSector(sector_id)->bonds[bond]->members.end(); parser_2++){
                    if(parser==parser_2){continue;}
                    if(getSector(sector_id)->ents.count(*parser_2)==0){continue;}
                    if(spritesIntersecting(getSector(sector_id)->getEnt(*parser)->sprite, getSector(sector_id)->getEnt(*parser_2)->sprite,3)){
                        ent_collisions_before++;
                    }
                }
            }

            double floor_collisions_before = 0;
            for(set<string>::iterator parser = getSector(sector_id)->bonds[bond]->members.begin(); parser != getSector(sector_id)->bonds[bond]->members.end(); parser++){
                if(getSector(sector_id)->floors.count(*parser)==0){continue;}
                for(set<string>::iterator parser_2 = parser; parser_2 != getSector(sector_id)->bonds[bond]->members.end(); parser_2++){
                    if(parser==parser_2){continue;}
                    if(getSector(sector_id)->floors.count(*parser_2)==0){continue;}
                    if(spritesIntersecting(getSector(sector_id)->getEnt(*parser)->sprite, getSector(sector_id)->getEnt(*parser_2)->sprite,3)){
                        floor_collisions_before++;
                    }
                }
            }

            getSector(sector_id)->bonds[bond]->displaceGroupContaining(id, disp*(-sister_percent));
            getSector(sector_id)->bonds[bond]->displaceGroupContaining(internal_cells["Sister ID"]->data["id"], disp*(own_percent));

            for(set<string>::iterator parser = getSector(sector_id)->bonds[bond]->members.begin(); parser != getSector(sector_id)->bonds[bond]->members.end(); parser++){
                if(getSector(sector_id)->getEnt(*parser)->properties->name == "Piston"){
                    double light_level_save = getSector(sector_id)->getEnt(*parser)->light_level;
                    getSector(sector_id)->getEnt(*parser)->run(0.00000000000000001);
                    getSector(sector_id)->getEnt(*parser)->light_level = light_level_save;
                }
            }

            double ent_collisions_after = 0;
            for(set<string>::iterator parser = getSector(sector_id)->bonds[bond]->members.begin(); parser != getSector(sector_id)->bonds[bond]->members.end(); parser++){
                if(getSector(sector_id)->ents.count(*parser)==0){continue;}
                for(set<string>::iterator parser_2 = parser; parser_2 != getSector(sector_id)->bonds[bond]->members.end(); parser_2++){
                    if(parser==parser_2){continue;}
                    if(getSector(sector_id)->ents.count(*parser_2)==0){continue;}
                    if(spritesIntersecting(getSector(sector_id)->getEnt(*parser)->sprite, getSector(sector_id)->getEnt(*parser_2)->sprite,3)){
                        ent_collisions_after++;
                    }
                }
            }

            double floor_collisions_after = 0;
            for(set<string>::iterator parser = getSector(sector_id)->bonds[bond]->members.begin(); parser != getSector(sector_id)->bonds[bond]->members.end(); parser++){
                if(getSector(sector_id)->floors.count(*parser)==0){continue;}
                for(set<string>::iterator parser_2 = parser; parser_2 != getSector(sector_id)->bonds[bond]->members.end(); parser_2++){
                    if(parser==parser_2){continue;}
                    if((*parser == id && *parser_2 == internal_cells["Sister ID"]->data["id"])||(*parser_2 == id && *parser == internal_cells["Sister ID"]->data["id"])){continue;}
                    if(getSector(sector_id)->floors.count(*parser_2)==0){continue;}
                    if(spritesIntersecting(getSector(sector_id)->getEnt(*parser)->sprite, getSector(sector_id)->getEnt(*parser_2)->sprite, 3)){
                        floor_collisions_after++;
                    }
                }
            }

            if(ent_collisions_after > ent_collisions_before || floor_collisions_after > floor_collisions_before){
                getSector(sector_id)->bonds[bond]->displaceGroupContaining(id, disp*(2*sister_percent));
                getSector(sector_id)->bonds[bond]->displaceGroupContaining(internal_cells["Sister ID"]->data["id"], disp*(-2*own_percent));
            }
        }



    }

    scissor_bars.clear();

    Sprite scissor_template;
    scissor_template.setTexture(getProperties("SCISSOR BAR")->sheet,true);
    IntRect scissor_rect = scissor_template.getTextureRect();
    scissor_template.setOrigin(scissor_rect.width/2.0,scissor_rect.height/2.0);
    scissor_template.setColor(sprite.getColor());

    Vector2f sister_coords = getSector(sector_id)->getEnt(internal_cells["Sister ID"]->data["id"])->coords;

    double full_scissor_breadth = (scissor_rect.width*sin(0.436));
    int full_scissors = floor(seperation/full_scissor_breadth);

    double remainder = seperation;

    for(int i = 0; i < full_scissors; i++){
        Vector2f scissor_center = sister_coords + ((extension_axis*-1)*((i*full_scissor_breadth)+((full_scissor_breadth/2.0)+getSector(sector_id)->getEnt(internal_cells["Sister ID"]->data["id"])->sprite.getLocalBounds().height/2.0)));

        scissor_template.setPosition(scissor_center);
        scissor_template.setRotation(getRotation() + 30);
        scissor_bars.push_back(scissor_template);
        scissor_template.setRotation(getRotation() -30);
        scissor_bars.push_back(scissor_template);

        remainder -= full_scissor_breadth;
    }

    if(remainder >= 2){
        Vector2f filler_center = sister_coords + ((extension_axis*-1)*((getSector(sector_id)->getEnt(internal_cells["Sister ID"]->data["id"])->sprite.getLocalBounds().height/2.0)+(seperation-(remainder/2.0))));
        scissor_template.setPosition(filler_center);

        double filler_angle = (asinf(remainder/scissor_rect.width)*(180/M_PI));
        scissor_template.setRotation(getRotation() + filler_angle);
        scissor_bars.push_back(scissor_template);
        scissor_template.setRotation(getRotation() -filler_angle);
        scissor_bars.push_back(scissor_template);

    }

    return true;
}

void Piston::draw(){

    for(vector<Sprite>::iterator parser = scissor_bars.begin(); parser != scissor_bars.end(); parser++){
        window.draw(*parser);
    }
    sprite.setPosition(coords.x,coords.y);
    window.draw(sprite);

    for(map<string, shared_ptr<FlowPort> >::iterator parser = ports.begin(); parser!=ports.end(); parser++)
    {
        parser->second->updatePosition(getPosition(),getRotation());
        window.draw(parser->second->sprite);
    }

}

PistonHead::PistonHead(string new_sector_id, string e_id, double x, double y, double rotation){

    constructEntity(new_sector_id, e_id, x, y, "Piston Head");
    voltage_rating = 12;
    setRotation(rotation);

    internal_cells["Sister ID"] = make_shared<GPIOCell>("Sister ID");
}


JunctionBox::JunctionBox(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Junction Box");

    voltage_rating = 0.0;

    internal_cells["Buffered Power 1"] = make_shared<PowerCell>("Buffered Power 1",0.5);
    addPort("Rail 1", Vector2f(-10,30), 0,"Power Port");
    ports["Rail 1"]->tapCell("Buffered Power 1");
    internal_cells["Buffered Power 2"] = make_shared<PowerCell>("Buffered Power 2",0.25);
    addPort("Rail 2", Vector2f(-10,-23), 180,"Power Port");
    ports["Rail 2"]->tapCell("Buffered Power 2");
    internal_cells["Buffered Power 3"] = make_shared<PowerCell>("Buffered Power 3",0.25);
    addPort("Rail 3", Vector2f(-10,-40), 180,"Power Port");
    ports["Rail 3"]->tapCell("Buffered Power 3");

    ports["Rail 1"]->voltage = 0.0;
    ports["Rail 2"]->voltage = 0.0;
    ports["Rail 3"]->voltage = 0.0;
}

bool JunctionBox::performFunction(double dt){

    ports["Rail 1"]->voltage = 0.0;
    ports["Rail 2"]->voltage = 0.0;
    ports["Rail 3"]->voltage = 0.0;

    if(ports["Rail 1"]->voltage_in > ports["Rail 2"]->voltage_in && ports["Rail 1"]->voltage_in > ports["Rail 3"]->voltage_in){
        ports["Rail 2"]->voltage = ports["Rail 1"]->voltage_in;
        ports["Rail 3"]->voltage = ports["Rail 1"]->voltage_in;
        //flow from 1 to 2 and 3

        double half_of_quantity_1 = internal_cells["Buffered Power 1"]->vars["milliamp hours"]/2.0;
        double milliamp_hours_to_transfer = half_of_quantity_1;
        if(internal_cells["Buffered Power 2"]->vars["milliamp hours"] + milliamp_hours_to_transfer > internal_cells["Buffered Power 2"]->vars["max milliamp hours"]){
            milliamp_hours_to_transfer = internal_cells["Buffered Power 2"]->vars["max milliamp hours"] - internal_cells["Buffered Power 2"]->vars["milliamp hours"];
        }
        internal_cells["Buffered Power 2"]->vars["milliamp hours"] += milliamp_hours_to_transfer;
        internal_cells["Buffered Power 1"]->vars["milliamp hours"] -= milliamp_hours_to_transfer;
        milliamp_hours_to_transfer = half_of_quantity_1;
        if(internal_cells["Buffered Power 3"]->vars["milliamp hours"] + milliamp_hours_to_transfer > internal_cells["Buffered Power 3"]->vars["max milliamp hours"]){
            milliamp_hours_to_transfer = internal_cells["Buffered Power 3"]->vars["max milliamp hours"] - internal_cells["Buffered Power 3"]->vars["milliamp hours"];
        }
        internal_cells["Buffered Power 3"]->vars["milliamp hours"] += milliamp_hours_to_transfer;
        internal_cells["Buffered Power 1"]->vars["milliamp hours"] -= milliamp_hours_to_transfer;

    }
    else if(ports["Rail 2"]->voltage_in > ports["Rail 1"]->voltage_in && ports["Rail 2"]->voltage_in > ports["Rail 3"]->voltage_in){
        ports["Rail 1"]->voltage = ports["Rail 2"]->voltage_in;
        //flow from 2 to 1
        double milliamp_hours_to_transfer = internal_cells["Buffered Power 2"]->vars["milliamp hours"];
        if(internal_cells["Buffered Power 1"]->vars["milliamp hours"] + milliamp_hours_to_transfer > internal_cells["Buffered Power 1"]->vars["max milliamp hours"]){
            milliamp_hours_to_transfer = internal_cells["Buffered Power 1"]->vars["max milliamp hours"] - internal_cells["Buffered Power 1"]->vars["milliamp hours"];
        }
        internal_cells["Buffered Power 1"]->vars["milliamp hours"] += milliamp_hours_to_transfer;
        internal_cells["Buffered Power 2"]->vars["milliamp hours"] -= milliamp_hours_to_transfer;
    }
    else if(ports["Rail 3"]->voltage_in > ports["Rail 1"]->voltage_in && ports["Rail 3"]->voltage_in > ports["Rail 2"]->voltage_in){
        ports["Rail 1"]->voltage = ports["Rail 3"]->voltage_in;
        //flow from 3 to 1
        double milliamp_hours_to_transfer = internal_cells["Buffered Power 3"]->vars["milliamp hours"];
        if(internal_cells["Buffered Power 1"]->vars["milliamp hours"] + milliamp_hours_to_transfer > internal_cells["Buffered Power 1"]->vars["max milliamp hours"]){
            milliamp_hours_to_transfer = internal_cells["Buffered Power 1"]->vars["max milliamp hours"] - internal_cells["Buffered Power 1"]->vars["milliamp hours"];
        }
        internal_cells["Buffered Power 1"]->vars["milliamp hours"] += milliamp_hours_to_transfer;
        internal_cells["Buffered Power 3"]->vars["milliamp hours"] -= milliamp_hours_to_transfer;
    }
    else if(ports["Rail 2"]->voltage_in > ports["Rail 1"]->voltage_in && ports["Rail 2"]->voltage_in == ports["Rail 3"]->voltage_in){
        ports["Rail 1"]->voltage = ports["Rail 2"]->voltage_in;
        //flow from 3 and 2 to 1
        double milliamp_hours_to_transfer = internal_cells["Buffered Power 2"]->vars["milliamp hours"];
        if(internal_cells["Buffered Power 1"]->vars["milliamp hours"] + milliamp_hours_to_transfer > internal_cells["Buffered Power 1"]->vars["max milliamp hours"]){
            milliamp_hours_to_transfer = internal_cells["Buffered Power 1"]->vars["max milliamp hours"] - internal_cells["Buffered Power 1"]->vars["milliamp hours"];
        }
        internal_cells["Buffered Power 1"]->vars["milliamp hours"] += milliamp_hours_to_transfer;
        internal_cells["Buffered Power 2"]->vars["milliamp hours"] -= milliamp_hours_to_transfer;
        milliamp_hours_to_transfer = internal_cells["Buffered Power 3"]->vars["milliamp hours"];
        if(internal_cells["Buffered Power 1"]->vars["milliamp hours"] + milliamp_hours_to_transfer > internal_cells["Buffered Power 1"]->vars["max milliamp hours"]){
            milliamp_hours_to_transfer = internal_cells["Buffered Power 1"]->vars["max milliamp hours"] - internal_cells["Buffered Power 1"]->vars["milliamp hours"];
        }
        internal_cells["Buffered Power 1"]->vars["milliamp hours"] += milliamp_hours_to_transfer;
        internal_cells["Buffered Power 3"]->vars["milliamp hours"] -= milliamp_hours_to_transfer;
    }
    else if(ports["Rail 1"]->voltage_in == ports["Rail 2"]->voltage_in && ports["Rail 2"]->voltage_in == ports["Rail 3"]->voltage_in){
        //
        double milliamp_hours_to_transfer = internal_cells["Buffered Power 1"]->vars["milliamp hours"] - internal_cells["Buffered Power 2"]->vars["milliamp hours"];
        if(milliamp_hours_to_transfer > 0){
            if(internal_cells["Buffered Power 2"]->vars["milliamp hours"] + milliamp_hours_to_transfer > internal_cells["Buffered Power 2"]->vars["max milliamp hours"]){
                milliamp_hours_to_transfer = internal_cells["Buffered Power 2"]->vars["max milliamp hours"] - internal_cells["Buffered Power 2"]->vars["milliamp hours"];
            }
            internal_cells["Buffered Power 2"]->vars["milliamp hours"] += milliamp_hours_to_transfer;
            internal_cells["Buffered Power 1"]->vars["milliamp hours"] -= milliamp_hours_to_transfer;
        }
        else if(milliamp_hours_to_transfer < 0){
            if(internal_cells["Buffered Power 1"]->vars["milliamp hours"] + milliamp_hours_to_transfer > internal_cells["Buffered Power 1"]->vars["max milliamp hours"]){
                milliamp_hours_to_transfer = internal_cells["Buffered Power 1"]->vars["max milliamp hours"] - internal_cells["Buffered Power 1"]->vars["milliamp hours"];
            }
            internal_cells["Buffered Power 1"]->vars["milliamp hours"] += milliamp_hours_to_transfer;
            internal_cells["Buffered Power 2"]->vars["milliamp hours"] -= milliamp_hours_to_transfer;
        }

        milliamp_hours_to_transfer = internal_cells["Buffered Power 1"]->vars["milliamp hours"] - internal_cells["Buffered Power 3"]->vars["milliamp hours"];
        if(milliamp_hours_to_transfer > 0){
            if(internal_cells["Buffered Power 3"]->vars["milliamp hours"] + milliamp_hours_to_transfer > internal_cells["Buffered Power 3"]->vars["max milliamp hours"]){
                milliamp_hours_to_transfer = internal_cells["Buffered Power 3"]->vars["max milliamp hours"] - internal_cells["Buffered Power 3"]->vars["milliamp hours"];
            }
            internal_cells["Buffered Power 3"]->vars["milliamp hours"] += milliamp_hours_to_transfer;
            internal_cells["Buffered Power 1"]->vars["milliamp hours"] -= milliamp_hours_to_transfer;
        }
        else if(milliamp_hours_to_transfer < 0){
            if(internal_cells["Buffered Power 1"]->vars["milliamp hours"] + milliamp_hours_to_transfer > internal_cells["Buffered Power 1"]->vars["max milliamp hours"]){
                milliamp_hours_to_transfer = internal_cells["Buffered Power 1"]->vars["max milliamp hours"] - internal_cells["Buffered Power 1"]->vars["milliamp hours"];
            }
            internal_cells["Buffered Power 1"]->vars["milliamp hours"] += milliamp_hours_to_transfer;
            internal_cells["Buffered Power 3"]->vars["milliamp hours"] -= milliamp_hours_to_transfer;
        }
    }

    return true;
}


DataDuplicator::DataDuplicator(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Data Duplicator");

    voltage_rating = 0.0;

    internal_cells["State Input"] = make_shared<GPIOCell>("State Input");
    addPort("GPI State", Vector2f(-16,0), -90,"GPI Port");
    ports["GPI State"]->tapCell("State Input");

    internal_cells["State Output 1"] = make_shared<GPIOCell>("State Output 1");
    addPort("GPO State 1", Vector2f(17,9), 90,"GPO Port");
    ports["GPO State 1"]->tapCell("State Output 1");

    internal_cells["State Output 2"] = make_shared<GPIOCell>("State Output 2");
    addPort("GPO State 2", Vector2f(17,-9), 90,"GPO Port");
    ports["GPO State 2"]->tapCell("State Output 2");
}

bool DataDuplicator::performFunction(double dt){

    internal_cells["State Output 1"]->vars["value"] = internal_cells["State Input"]->vars["value"];
    internal_cells["State Output 2"]->vars["value"] = internal_cells["State Input"]->vars["value"];
    return true;
}

Battery12V::Battery12V(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "12V Battery");

    voltage_rating = 12.0;

    internal_cells["Stored Power"] = make_shared<PowerCell>("Stored Power",500);
    addPort("12V Rail 2", Vector2f(21,2), 180,"Power Port");
    ports["12V Rail 2"]->tapCell("Stored Power");
    ports["12V Rail 2"]->voltage = 12.0;

    addPort("12V Rail 1", Vector2f(-21,2), 180,"Power Port");
    ports["12V Rail 1"]->tapCell("Stored Power");
    ports["12V Rail 1"]->voltage = 12.0;

    internal_cells["Stored Power"]->vars["milliamp hours"] = 500;

}

bool Battery12V::performFunction(double dt){

    if(internal_cells["Stored Power"]->vars["milliamp hours"]<0.01){internal_cells["Stored Power"]->vars["milliamp hours"]=0;}
    if(internal_cells["Stored Power"]->vars["running average"] == 0.0){
        ports["12V Rail 1"]->voltage = 0.0;
        ports["12V Rail 2"]->voltage = 0.0;
    }
    else{
        ports["12V Rail 1"]->voltage = 12.0;
        ports["12V Rail 2"]->voltage = 12.0;
    }

    return true;
}

Battery240V::Battery240V(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "240V Battery");

    voltage_rating = 240.0;

    internal_cells["Stored Power"] = make_shared<PowerCell>("Stored Power",70000);
    internal_cells["Stored Power"]->vars["milliamp hours"] = 70000;

    addPort("240V Rail 2", Vector2f(-35,-40), 180,"Power Port");
    ports["240V Rail 2"]->tapCell("Stored Power");
    ports["240V Rail 2"]->voltage = 240.0;

    addPort("240V Rail 1", Vector2f(-20,-40), 180,"Power Port");
    ports["240V Rail 1"]->tapCell("Stored Power");
    ports["240V Rail 1"]->voltage = 240.0;

    internal_cells["mAh Data Output"] = make_shared<GPIOCell>("mAh Data Output");
    addPort("GPO mAh Data", Vector2f(0,-63), 0,"GPO Port");
    ports["GPO mAh Data"]->tapCell("mAh Data Output");

    internal_cells["mAh Data Output"]->vars["value"] = internal_cells["Stored Power"]->vars["milliamp hours"];
}

bool Battery240V::performFunction(double dt){

    internal_cells["mAh Data Output"]->vars["value"] = internal_cells["Stored Power"]->vars["running average"];

    if(internal_cells["Stored Power"]->vars["milliamp hours"]<0.01){internal_cells["Stored Power"]->vars["milliamp hours"]=0;}
    if(internal_cells["Stored Power"]->vars["running average"] == 0.0){
        ports["240V Rail 1"]->voltage = 0.0;
        ports["240V Rail 2"]->voltage = 0.0;
    }
    else{
        ports["240V Rail 1"]->voltage = 240.0;
        ports["240V Rail 2"]->voltage = 240.0;
    }

    return true;
}

Transformer::Transformer(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Transformer");

    voltage_rating = 0.0;

    internal_cells["Buffered Power"] = make_shared<PowerCell>("Buffered Power",0.25);
    addPort("High Voltage Rail", Vector2f(-38,-12), 0,"Power Port");
    ports["High Voltage Rail"]->tapCell("Buffered Power");
    ports["High Voltage Rail"]->voltage = 0.0;
    addPort("Low Voltage Rail", Vector2f(-38,12), 180,"Power Port");
    ports["Low Voltage Rail"]->tapCell("Buffered Power");
    ports["Low Voltage Rail"]->voltage = 0.0;
}

bool Transformer::performFunction(double dt){

    ports["High Voltage Rail"]->voltage = 0.0;
    ports["Low Voltage Rail"]->voltage = 0.0;
    if(ports["High Voltage Rail"]->connection == ""){ ports["High Voltage Rail"]->voltage_in = 0.0; }
    if(ports["Low Voltage Rail"]->connection == ""){ ports["Low Voltage Rail"]->voltage_in = 0.0; }

    if(ports["High Voltage Rail"]->voltage_in > ports["Low Voltage Rail"]->voltage_in){
        animator.play(17,111);

        //convert high to low
        ports["Low Voltage Rail"]->voltage = ports["High Voltage Rail"]->voltage_in/2;

        castLight(sector_id, getPosition(), 2, 14);
    }
    else if(ports["Low Voltage Rail"]->voltage_in > ports["High Voltage Rail"]->voltage_in){
        animator.play(17,111);

        //convert low to high
        ports["High Voltage Rail"]->voltage = ports["Low Voltage Rail"]->voltage_in*2;
        castLight(sector_id, getPosition(), 2, 14);
    }
    else{
        animator.play(0,0);
    }
}

LEDBar::LEDBar(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "LED Bar");

    voltage_rating = 12.0;
    wattage = 30.0;

    internal_cells["Buffered Power"] = make_shared<PowerCell>("Buffered Power",0.1);
    addPort("Neutral Rail 1", Vector2f(39,0), 0,"Power Port");
    ports["Neutral Rail 1"]->tapCell("Buffered Power");
    ports["Neutral Rail 1"]->voltage = 0.0;
    addPort("Neutral Rail 2", Vector2f(-39,0), 0,"Power Port");
    ports["Neutral Rail 2"]->tapCell("Buffered Power");
    ports["Neutral Rail 2"]->voltage = 0.0;

}


bool LEDBar::performFunction(double dt){

    double amps = wattage/voltage_rating;
    double milliamp_hours_needed = ((1000*amps/60)/60)*dt;
    double milliamp_hours_to_take = milliamp_hours_needed;

    double intensity = 175;

    double percent = 1;


    if(internal_cells["Buffered Power"]->vars["milliamp hours"]<0.01){internal_cells["Buffered Power"]->vars["milliamp hours"]=0;}

    ports["Neutral Rail 1"]->voltage = ports["Neutral Rail 2"]->voltage_in - ports["Neutral Rail 1"]->voltage_in;
    if(ports["Neutral Rail 1"]->voltage < 0){ ports["Neutral Rail 1"]->voltage = 0;}
    ports["Neutral Rail 2"]->voltage = ports["Neutral Rail 1"]->voltage_in - ports["Neutral Rail 2"]->voltage_in;
    if(ports["Neutral Rail 2"]->voltage < 0){ ports["Neutral Rail 2"]->voltage = 0;}

    if(internal_cells["Buffered Power"]->vars["milliamp hours"] < milliamp_hours_needed){
        milliamp_hours_to_take = internal_cells["Buffered Power"]->vars["milliamp hours"];
        percent = milliamp_hours_to_take/milliamp_hours_needed;
    }
    internal_cells["Buffered Power"]->vars["milliamp hours"] -= milliamp_hours_to_take;

    castLight(sector_id, getPosition(), 0.35, intensity*percent);

    return true;
}


CombustionThruster::CombustionThruster(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Combustion Thruster");

    voltage_rating = 0.0;

    internal_cells["Ignition Switch Input"] = make_shared<GPIOCell>("Ignition Switch Input");
    addPort("GPI Ignition Switch", Vector2f(-34,15), -90,"GPI Port");
    ports["GPI Ignition Switch"]->tapCell("Ignition Switch Input");

    internal_cells["Ready Status Output"] = make_shared<GPIOCell>("Ready Status Output");
    addPort("GPO Ready Status", Vector2f(-34,-10), -90,"GPO Port");
    ports["GPO Ready Status"]->tapCell("Ready Status Output");

    internal_cells["Buffered Gas"] = make_shared<GasCell>("Buffered Gas", 5);
    addPort("Hose Fitting", Vector2f(34,-4), 90,"Hose Fitting");
    ports["Hose Fitting"]->tapCell("Buffered Gas");

}

bool CombustionThruster::performFunction(double dt){


    internal_cells["Buffered Gas"]->recalculate();

    internal_cells["Ready Status Output"]->vars["value"] = 0;
    if(internal_cells["Buffered Gas"]->vars["total pressure"] >= 14){
        internal_cells["Ready Status Output"]->vars["value"] = 1;

        if(internal_cells["Ignition Switch Input"]->vars["value"] == 1 && !needs_reset){
            //fire!

            double momentum_per_kilogram = 10000;
            Vector2f axis = getMyParallelAxis()*(-1.0);
            Vector2f impulse = axis*momentum_per_kilogram*internal_cells["Buffered Gas"]->vars["total kilograms"];
            applyImpulse(impulse.x,impulse.y, false, true);

            internal_cells["Buffered Gas"]->take(internal_cells["Buffered Gas"]->vars["total kilograms"]);

            double Q = getRotation()*(M_PI/180);
            Vector2f exhaust_position = getPosition() + Vector2f(0*cos(Q)-(-52)*sin(Q),0*sin(Q)+(-52)*cos(Q));
            castLight(sector_id, exhaust_position, 2, 160);

            for(int i = 0; i < 6; i++){

                double neg_x = 1;
                double neg_y = 1;
                if(rand() % 100<50){neg_x=-1;}
                if(rand() % 100<50){neg_y=-1;}

                double x_excitement = neg_x*(rand()%150);
                double y_excitement = neg_y*(rand()%150);

                Vector2f excitement = Vector2f(x_excitement, y_excitement);

                createEffect(sector_id, exhaust_position, (axis*-400)+excitement, "Smoke");
                createEffect(sector_id, exhaust_position, (axis*-450)+excitement, "Embers");
            }


        }
        if(internal_cells["Ignition Switch Input"]->vars["value"] == 1){ needs_reset = true; }
        if(internal_cells["Ignition Switch Input"]->vars["value"] == 0){ needs_reset = false;}
    }

    return true;
}


PlasmaThruster::PlasmaThruster(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Plasma Thruster");

    voltage_rating = 0.0;

    internal_cells["Ignition Switch Input"] = make_shared<GPIOCell>("Ignition Switch Input");
    addPort("GPI Ignition Switch", Vector2f(-34,15), -90,"GPI Port");
    ports["GPI Ignition Switch"]->tapCell("Ignition Switch Input");

   // internal_cells["Buffered Gas"] = make_shared<GasCell>("Buffered Gas", 5);
   // addPort("Hose Fitting", Vector2f(34,-4), 90,"Hose Fitting");
    //ports["Hose Fitting"]->tapCell("Buffered Gas");

}

bool PlasmaThruster::performFunction(double dt){


 //   internal_cells["Buffered Gas"]->recalculate();

 //  internal_cells["Ready Status Output"]->vars["value"] = 0;
 //   if(internal_cells["Buffered Gas"]->vars["total pressure"] >= 14){
     //   internal_cells["Ready Status Output"]->vars["value"] = 1;

        if(internal_cells["Ignition Switch Input"]->vars["value"] == 1){
            //fire!

            double momentum_per_kilogram = 10000;
            Vector2f axis = getMyParallelAxis()*(-1.0);
            Vector2f impulse = axis*momentum_per_kilogram*10;
            applyImpulse(impulse.x,impulse.y, false, true);

          //  internal_cells["Buffered Gas"]->take(internal_cells["Buffered Gas"]->vars["total kilograms"]);

            double Q = getRotation()*(M_PI/180);
            Vector2f exhaust_position = getPosition() + Vector2f(11*cos(Q)-(-64)*sin(Q),11*sin(Q)+(-64)*cos(Q));
            castLight(sector_id, exhaust_position, 2, 160);

            for(int i = 0; i < 6; i++){

                double neg_x = 1;
                double neg_y = 1;
                if(rand() % 100<50){neg_x=-1;}
                if(rand() % 100<50){neg_y=-1;}

                double x_excitement = neg_x*(rand()%150);
                double y_excitement = neg_y*(rand()%150);

                Vector2f excitement = Vector2f(x_excitement, y_excitement);

             //   createEffect(sector_id, exhaust_position, (axis*-400)+excitement, "Smoke");
              //  createEffect(sector_id, exhaust_position, (axis*-450)+excitement, "Embers");
            }


        }
   //     if(internal_cells["Ignition Switch Input"]->vars["value"] == 1){ needs_reset = true; }
   //     if(internal_cells["Ignition Switch Input"]->vars["value"] == 0){ needs_reset = false;}
   // }

    return true;
}

GasPump::GasPump(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Gas Pump");

    voltage_rating = 12.0;
    wattage = 350;

    internal_cells["Buffered Power"] = make_shared<PowerCell>("Buffered Power",10);
    addPort("Neutral Rail", Vector2f(5,27), 0,"Power Port");
    ports["Neutral Rail"]->tapCell("Buffered Power");
    ports["Neutral Rail"]->voltage = 0.0;

    internal_cells["Buffered Gas A"] = make_shared<GasCell>("Buffered Gas A", 1);
    addPort("Hose Fitting", Vector2f(-46,-5), -90,"Hose Fitting");
    ports["Hose Fitting"]->tapCell("Buffered Gas A");

    internal_cells["Buffered Gas B"] = make_shared<GasCell>("Buffered Gas B", 0.1);
    addPort("Hose Fitting 2", Vector2f(49,-7), 90,"Hose Fitting");
    ports["Hose Fitting 2"]->tapCell("Buffered Gas B");
}

bool GasPump::performFunction(double dt){

    double amps = wattage/voltage_rating;
    double milliamp_hours_needed = ((1000*amps/60)/60)*dt;

    if(internal_cells["Buffered Power"]->vars["milliamp hours"]<0.01){internal_cells["Buffered Power"]->vars["milliamp hours"]=0;}

    if(internal_cells["Buffered Power"]->vars["milliamp hours"] >= milliamp_hours_needed){

        internal_cells["Buffered Power"]->vars["milliamp hours"] -= milliamp_hours_needed;
        animator.play(17,79);

        internal_cells["Buffered Gas A"]->recalculate();
        internal_cells["Buffered Gas B"]->recalculate();

        if(internal_cells["Buffered Gas A"]->vars["total kilograms"] > 0 && internal_cells["Buffered Gas B"]->vars["total pressure"] <= 60){
            map<string, double> moving_quantities = internal_cells["Buffered Gas A"]->take(internal_cells["Buffered Gas A"]->vars["total kilograms"]);
            for(map<string, double>::iterator parser = moving_quantities.begin(); parser!=moving_quantities.end(); parser++){

                if(internal_cells["Buffered Gas B"]->vars.count(parser->first)==0){
                    internal_cells["Buffered Gas B"]->vars[parser->first] = parser->second;
                }
                else{
                    internal_cells["Buffered Gas B"]->vars[parser->first] += parser->second;
                }
            }
        }
    }
    else{
        animator.play(0,0);
    }

    return true;
}

Lever::Lever(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Lever");

    voltage_rating = 0.0;

    internal_cells["Lever State Output"] = make_shared<GPIOCell>("Lever State Output");
    addPort("GPO Lever State", Vector2f(0,-19), 0,"GPO Port");
    ports["GPO Lever State"]->tapCell("Lever State Output");
}

bool Lever::performFunction(double dt){

    if(internal_cells["Lever State Output"]->vars["value"]==1){

        animator.play(1,1);
    }
    else if(internal_cells["Lever State Output"]->vars["value"]==0){

        animator.play(0,0);
    }

    return true;
}

PressureGauge::PressureGauge(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Pressure Gauge");

    voltage_rating = 0.0;

    internal_cells["PSI Data Output"] = make_shared<GPIOCell>("PSI Data Output");
    addPort("GPO PSI Data", Vector2f(0,-21), 0,"GPO Port");
    ports["GPO PSI Data"]->tapCell("PSI Data Output");

    internal_cells["Buffered Gas"] = make_shared<GasCell>("Buffered Gas", 1);
    addPort("Hose Fitting", Vector2f(-23.5,7), -90, "Hose Fitting");
    ports["Hose Fitting"]->tapCell("Buffered Gas");

    max_bound = 60; //psi
    turn_speed = 180; //degrees per second
    max_rotation = 90; //degrees
    current_rotation = 0;

    arm = RectangleShape(Vector2f(-7,-1));
    arm.setOrigin(0,0.5);
    arm.setFillColor(Color(1,1,1,255));
    arm_coords = Vector2f(-2,1);
    arm.setPosition(sprite.getPosition().x+arm_coords.x,sprite.getPosition().y+arm_coords.y);
}

bool PressureGauge::performFunction(double dt){

    internal_cells["Buffered Gas"]->recalculate();
    if(!ports["Hose Fitting"]->isConnected()){
        internal_cells["Buffered Gas"]->take(internal_cells["Buffered Gas"]->vars["total kilograms"]);
    }
    internal_cells["Buffered Gas"]->recalculate();

    double percentage = internal_cells["Buffered Gas"]->vars["total pressure"]/max_bound;
    internal_cells["PSI Data Output"]->vars["value"] = internal_cells["Buffered Gas"]->vars["total pressure"];

    //turn the arm to display the var_num["value"]
    double target_rotation = percentage*max_rotation;
    if(current_rotation < target_rotation){
        current_rotation += turn_speed*dt;
    }
    else if(current_rotation > target_rotation){
        if(current_rotation-(turn_speed*dt)>=0)
        {
            current_rotation -= turn_speed*dt;
        }
    }

    if(current_rotation > max_rotation){
        current_rotation -= 25*turn_speed*dt;
    }
    //

    //calculate world location and rotations of arm
    double Q = getRotation() * (M_PI/180);

    Vector2f new_position = getPosition() + Vector2f(arm_coords.x*cos(Q)-arm_coords.y*sin(Q),arm_coords.x*sin(Q)+arm_coords.y*cos(Q));
    arm.setPosition(new_position.x,new_position.y);
    arm.setRotation((Q*(180/M_PI))+current_rotation);
    //
    arm.setFillColor(Color(1+(light_level/10),1+(light_level/10),1+(light_level/10)));

    return true;
}

void PressureGauge::draw(){

    window.draw(sprite);

    for(map<string, shared_ptr<FlowPort> >::iterator parser = ports.begin(); parser!=ports.end(); parser++)
    {
        parser->second->updatePosition(getPosition(),getRotation());
        window.draw(parser->second->sprite);
    }

    window.draw(arm);
}

Ironworker::Ironworker(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Ironworker");

    takes_input = true;
    can_be_occupied = true;
    internal_cells["User Info"] = make_shared<FlowCell>();
    internal_cells["User Info"]->id = "User Info";
    internal_cells["User Info"]->data["id"] = "";
    internal_cells["User Info"]->data["toolmode"] = "none";

    voltage_rating = 12.0;
    wattage = 350;

    internal_cells["Buffered Power"] = make_shared<PowerCell>("Buffered Power",10);
    addPort("Neutral Rail", Vector2f(0,11), 0,"Power Port");
    ports["Neutral Rail"]->tapCell("Buffered Power");
    ports["Neutral Rail"]->voltage = 0.0;

    top_animator = Animator();
    top_sprite = Sprite();
    top_sprite.setTexture(getProperties("IRONWORKER TOP")->sheet,false);
    top_sprite.setTextureRect(IntRect(0,0,136,100));
    top_sprite.setOrigin(68,50);
    top_sprite.setPosition(sprite.getPosition().x,sprite.getPosition().y-12);

    double_draw = true;
}

bool Ironworker::performFunction(double dt){


    top_animator.update(dt, top_sprite);
      //  double Q = getRotation()*(M_PI/180);
       // Vector2f screen_position = getPosition() + Vector2f(21*cos(Q)-2*sin(Q),21*sin(Q)+2*cos(Q));
       // castLight(sector_id, screen_position, 2, 90);

    double amps = wattage/voltage_rating;
    double milliamp_hours_needed = ((1000*amps/60)/60)*dt;

    if(internal_cells["Buffered Power"]->vars["milliamp hours"]<0.01){internal_cells["Buffered Power"]->vars["milliamp hours"]=0;}

    if(internal_cells["Buffered Power"]->vars["milliamp hours"] >= milliamp_hours_needed){

        internal_cells["Buffered Power"]->vars["milliamp hours"] -= milliamp_hours_needed;

        if(internal_cells["User Info"]->data["id"] != ""){

            if(input.keys_held.count("space") != 0 && internal_cells["User Info"]->data["toolmode"] == "saw"){
               top_animator.fps = 36;
               top_animator.play(39,42);
            }

            if(input.keys_released.count("t") != 0){
                if(internal_cells["User Info"]->data["toolmode"] == "saw"){
                    internal_cells["User Info"]->data["toolmode"] = "none";
                    top_animator.fps = 24;
                    top_animator.play(38,16);
                }
                else{
                    internal_cells["User Info"]->data["toolmode"] = "saw";
                    top_animator.fps = 24;
                    top_animator.play(16,38);
                }

            }
            else if(top_animator.loop_completed){
                if(internal_cells["User Info"]->data["toolmode"] == "saw"){
                    top_animator.fps = 24;
                    top_animator.play(38,38);
                }
                else{
                    top_animator.fps = 24;
                    top_animator.play(0,0);
                }
                top_animator.update(0.000000000000000000001, top_sprite);
            }



          //

            bool moving = false;

            double walk_speed = 1000;
            if(input.keys_held.count("w") != 0){
                Vector2f axis= getMyParallelAxis();

                Vector2f v_difference = getVelocity()-(axis*walk_speed);
                if(v_difference.x/(axis.x) > 0){v_difference.x = 0;}
                if(v_difference.y/(axis.y) > 0){v_difference.y = 0;}

                Vector2f p_to_add = v_difference*getMyMass()*10*dt;

                if((getSector(sector_id)->planetside || floors.size()!=0) and (not bonded or floor_bonds.count(bond)==0))
                {
                    applyImpulse(-p_to_add.x,-p_to_add.y,getPosition(),false,true);
                    for(set<string>::iterator parser = floors.begin(); parser != floors.end(); parser++){
                        if(shared_ptr<Entity> floor_ent = getSector(sector_id)->getEnt(*parser)){
                            floor_ent->applyImpulse(p_to_add.x/floors.size(),p_to_add.y/floors.size(),getPosition(),false,false);
                        }
                    }
                }

                moving = true;
            }
            if(input.keys_held.count("s") != 0){
                Vector2f axis= getMyParallelAxis();

                Vector2f v_difference = getVelocity()-(axis*-1*walk_speed);
                if(v_difference.x/(-axis.x) > 0){v_difference.x = 0;}
                if(v_difference.y/(-axis.y) > 0){v_difference.y = 0;}

                Vector2f p_to_add = v_difference*getMyMass()*10*dt;

                if((getSector(sector_id)->planetside || floors.size()!=0) and (not bonded or floor_bonds.count(bond)==0))
                {
                    applyImpulse(-p_to_add.x,-p_to_add.y,getPosition(),false,true);
                    for(set<string>::iterator parser = floors.begin(); parser != floors.end(); parser++){
                        if(shared_ptr<Entity> floor_ent = getSector(sector_id)->getEnt(*parser)){
                            floor_ent->applyImpulse(p_to_add.x/floors.size(),p_to_add.y/floors.size(),getPosition(),false,false);
                        }
                    }
                }

                moving = true;
            }
            if(input.keys_held.count("a") != 0){
                Vector2f axis= getMyPerpendicularAxis();

                Vector2f v_difference = getVelocity()-(axis*-1*walk_speed);
                if(v_difference.x/(-axis.x) > 0){v_difference.x = 0;}
                if(v_difference.y/(-axis.y) > 0){v_difference.y = 0;}

                Vector2f p_to_add = v_difference*getMyMass()*10*dt;

                if((getSector(sector_id)->planetside || floors.size()!=0) and (not bonded or floor_bonds.count(bond)==0))
                {
                    applyImpulse(-p_to_add.x,-p_to_add.y,getPosition(),false,true);
                    for(set<string>::iterator parser = floors.begin(); parser != floors.end(); parser++){
                        if(shared_ptr<Entity> floor_ent = getSector(sector_id)->getEnt(*parser)){
                            floor_ent->applyImpulse(p_to_add.x/floors.size(),p_to_add.y/floors.size(),getPosition(),false,false);
                        }
                    }
                }

                moving = true;
            }
            if(input.keys_held.count("d") != 0){
                Vector2f axis= getMyPerpendicularAxis();

                Vector2f v_difference = getVelocity()-(axis*walk_speed);
                if(v_difference.x/(axis.x) > 0){v_difference.x = 0;}
                if(v_difference.y/(axis.y) > 0){v_difference.y = 0;}

                Vector2f p_to_add = v_difference*getMyMass()*10*dt;

                if((getSector(sector_id)->planetside || floors.size()!=0) and (not bonded or floor_bonds.count(bond)==0))
                {
                    applyImpulse(-p_to_add.x,-p_to_add.y,getPosition(),false,true);
                    for(set<string>::iterator parser = floors.begin(); parser != floors.end(); parser++){
                        if(shared_ptr<Entity> floor_ent = getSector(sector_id)->getEnt(*parser)){
                            floor_ent->applyImpulse(p_to_add.x/floors.size(),p_to_add.y/floors.size(),getPosition(),false,false);
                        }
                    }
                }

                moving = true;
            }

            double turn_speed = 180;
            if(input.keys_held.count("e") != 0){
                double difference = getRotationalVelocity() - (turn_speed);
                double p_to_add = difference*getMyInertia()*10*dt;
                if((getSector(sector_id)->planetside || floors.size()!=0) and (not bonded or floor_bonds.count(bond)==0))
                {
                    applyRotationalImpulse(-p_to_add);
                }

                moving = true;
            }
            if(input.keys_held.count("q") != 0){
                double difference = getRotationalVelocity() - (-1*turn_speed);
                double p_to_add = difference*getMyInertia()*10*dt;
                if((getSector(sector_id)->planetside || floors.size()!=0) and (not bonded or floor_bonds.count(bond)==0))
                {
                    applyRotationalImpulse(-p_to_add);
                }

                moving = true;
            }

            if(moving){
                animator.fps = 44;
                animator.play(16,23);
            }
            else{
                animator.play(0,0);
            }

        }
        else{
            top_animator.fps = 24;
            top_animator.play(0,0);
            animator.fps = 24;
            animator.play(0,0);
        }
    }
    else{
       top_animator.fps = 24;
        top_animator.play(0,0);
        animator.fps = 24;
        animator.play(0,0);
    }

        input.reset();

    return true;
}

void Ironworker::draw(){

    double Q = getRotation()*(M_PI/180);
    Vector2f user_position = getPosition() + Vector2f(0*cos(Q)+12*sin(Q),0*sin(Q)-12*cos(Q));
    top_sprite.setPosition(user_position.x,user_position.y);
    top_sprite.setRotation(getRotation());
    top_sprite.setColor(sprite.getColor());

    if(internal_cells["User Info"]->data["id"] != ""){

   //     cout << getSector(sector_id)->getEnt(internal_cells["User Info"]->data["id"])->getPosition().x << " " << getSector(sector_id)->getEnt(internal_cells["User Info"]->data["id"])->getPosition().y << endl;
        getSector(sector_id)->getEnt(internal_cells["User Info"]->data["id"])->setPosition(user_position.x,user_position.y);

     //   cout << getSector(sector_id)->getEnt(internal_cells["User Info"]->data["id"])->getPosition().x << " " << getSector(sector_id)->getEnt(internal_cells["User Info"]->data["id"])->getPosition().y << endl << endl;
        getSector(sector_id)->getEnt(internal_cells["User Info"]->data["id"])->setRotation(getRotation());

    }

     window.draw(sprite);

    if(internal_cells["User Info"]->data["id"] != ""){
        window.draw(getSector(sector_id)->getEnt(internal_cells["User Info"]->data["id"])->sprite);
    }

    window.draw(top_sprite);

    for(map<string, shared_ptr<FlowPort> >::iterator parser = ports.begin(); parser!=ports.end(); parser++)
    {
        parser->second->updatePosition(getPosition(),getRotation());
        window.draw(parser->second->sprite);
    }


}

ControlBoard::ControlBoard(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Control Board");

    voltage_rating = 12.0;
    wattage = 350;

    internal_cells["Buffered Power"] = make_shared<PowerCell>("Buffered Power",10);
    addPort("Neutral Rail", Vector2f(-48,-15), 0,"Power Port");
    ports["Neutral Rail"]->tapCell("Buffered Power");
    ports["Neutral Rail"]->voltage = 0.0;

    takes_input = true;
    can_be_occupied = true;
    internal_cells["User Info"] = make_shared<FlowCell>();
    internal_cells["User Info"]->id = "User Info";
    internal_cells["User Info"]->data["id"] = "";

    internal_cells["Control Output 1"] = make_shared<GPIOCell>("Control Output 1");
    addPort("GPO 1", Vector2f(-73,0), -90,"GPO Port");
    ports["GPO 1"]->tapCell("Control Output 1");

    internal_cells["Control Output 2"] = make_shared<GPIOCell>("Control Output 2");
    addPort("GPO 2", Vector2f(-73,-15), -90,"GPO Port");
    ports["GPO 2"]->tapCell("Control Output 2");

    internal_cells["Control Output 3"] = make_shared<GPIOCell>("Control Output 3");
    addPort("GPO 3", Vector2f(-73,-30), -90,"GPO Port");
    ports["GPO 3"]->tapCell("Control Output 3");

    internal_cells["Control Output 4"] = make_shared<GPIOCell>("Control Output 4");
    addPort("GPO 4", Vector2f(-56,-46), 0,"GPO Port");
    ports["GPO 4"]->tapCell("Control Output 4");

    internal_cells["Control Output 5"] = make_shared<GPIOCell>("Control Output 5");
    addPort("GPO 5", Vector2f(-41,-46), 0,"GPO Port");
    ports["GPO 5"]->tapCell("Control Output 5");

    internal_cells["Control Output 6"] = make_shared<GPIOCell>("Control Output 6");
    addPort("GPO 6", Vector2f(61,-46), 0,"GPO Port");
    ports["GPO 6"]->tapCell("Control Output 6");

    internal_cells["Control Output 7"] = make_shared<GPIOCell>("Control Output 7");
    addPort("GPO 7", Vector2f(73,-34), 90,"GPO Port");
    ports["GPO 7"]->tapCell("Control Output 7");

    internal_cells["Control Output 8"] = make_shared<GPIOCell>("Control Output 8");
    addPort("GPO 8", Vector2f(73,-19), 90,"GPO Port");
    ports["GPO 8"]->tapCell("Control Output 8");

    internal_cells["Control Output 9"] = make_shared<GPIOCell>("Control Output 9");
    addPort("GPO 9", Vector2f(73,-4), 90,"GPO Port");
    ports["GPO 9"]->tapCell("Control Output 9");
}

bool ControlBoard::performFunction(double dt){



    double amps = wattage/voltage_rating;
    double milliamp_hours_needed = ((1000*amps/60)/60)*dt;

    if(internal_cells["Buffered Power"]->vars["milliamp hours"]<0.01){internal_cells["Buffered Power"]->vars["milliamp hours"]=0;}

    if(internal_cells["Buffered Power"]->vars["milliamp hours"] >= milliamp_hours_needed){

        internal_cells["Buffered Power"]->vars["milliamp hours"] -= milliamp_hours_needed;

        double Q = getRotation()*(M_PI/180);
        Vector2f screen_position = getPosition() + Vector2f(21*cos(Q)-2*sin(Q),21*sin(Q)+2*cos(Q));
        castLight(sector_id, screen_position, 2, 90);

        if(internal_cells["User Info"]->data["id"] != ""){

            for(int i = 1; i <= 9; i++){
                if(input.keys_held.count(asString(i)) != 0){
                    internal_cells["Control Output " + asString(i)]->vars["value"] = 1;
                }
                else{
                    internal_cells["Control Output " + asString(i)]->vars["value"] = 0;
                }
            }
        }

        animator.play(17,79);
        input.reset();
    }
    else{
        animator.play(0,0);
    }

    return true;
}

void ControlBoard::draw(){

    sprite.setPosition(coords.x,coords.y);
    window.draw(sprite);

    for(map<string, shared_ptr<FlowPort> >::iterator parser = ports.begin(); parser!=ports.end(); parser++)
    {
        parser->second->updatePosition(getPosition(),getRotation());
        window.draw(parser->second->sprite);
    }

    if(internal_cells["User Info"]->data["id"] != ""){
        double Q = getRotation()*(M_PI/180);
        Vector2f user_position = getPosition() + Vector2f(21*cos(Q)-20*sin(Q),21*sin(Q)+20*cos(Q));
   //     cout << getSector(sector_id)->getEnt(internal_cells["User Info"]->data["id"])->getPosition().x << " " << getSector(sector_id)->getEnt(internal_cells["User Info"]->data["id"])->getPosition().y << endl;
        getSector(sector_id)->getEnt(internal_cells["User Info"]->data["id"])->setPosition(user_position.x,user_position.y);
     //   cout << getSector(sector_id)->getEnt(internal_cells["User Info"]->data["id"])->getPosition().x << " " << getSector(sector_id)->getEnt(internal_cells["User Info"]->data["id"])->getPosition().y << endl << endl;
        getSector(sector_id)->getEnt(internal_cells["User Info"]->data["id"])->setRotation(getRotation());
        window.draw(getSector(sector_id)->getEnt(internal_cells["User Info"]->data["id"])->sprite);
    }
}


Beacon::Beacon(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Beacon");

    voltage_rating = 12.0;
    wattage = 350;

    internal_cells["Buffered Power"] = make_shared<PowerCell>("Buffered Power",10);
    addPort("Neutral Rail", Vector2f(-48,-15), 0,"Power Port");
    ports["Neutral Rail"]->tapCell("Buffered Power");
    ports["Neutral Rail"]->voltage = 0.0;

    takes_input = false;
    can_be_occupied = false;
}


bool Beacon::performFunction(double dt){

    double amps = wattage/voltage_rating;
    double milliamp_hours_needed = ((1000*amps/60)/60)*dt;

    if(internal_cells["Buffered Power"]->vars["milliamp hours"]<0.01){internal_cells["Buffered Power"]->vars["milliamp hours"]=0;}

    if(internal_cells["Buffered Power"]->vars["milliamp hours"] >= milliamp_hours_needed){

        internal_cells["Buffered Power"]->vars["milliamp hours"] -= milliamp_hours_needed;

        double Q = getRotation()*(M_PI/180);
        Vector2f screen_position = getPosition() + Vector2f(21*cos(Q)-2*sin(Q),21*sin(Q)+2*cos(Q));
        castLight(sector_id, screen_position, 2, 90);

        //animator.play(17,79);
    }
    else{
        animator.play(0,0);
    }

    return true;
}

void Beacon::draw(){

    sprite.setPosition(coords.x,coords.y);
    window.draw(sprite);

    for(map<string, shared_ptr<FlowPort> >::iterator parser = ports.begin(); parser!=ports.end(); parser++)
    {
        parser->second->updatePosition(getPosition(),getRotation());
        window.draw(parser->second->sprite);
    }
}
/*
Propulsor::Propulsor(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Propulsor");

    internal_cells["Buffered Power"] = make_shared<PowerCell>("Buffered Power",10);
    addPort("Leads", Vector2f(0,43), 180,"Power Port");
    ports["Leads"]->tapCell("Buffered Power");

}

bool Propulsor::performFunction(double dt){

    if(internal_cells["Buffered Power"]->vars["joules"]>0.1){
        double sum_kg = 1.5;
        double kg_per_s = 550;

        double Q = sprite.getRotation() * (M_PI/180);
        Vector2f u_vector = Vector2f(1*sin(Q),-1*cos(Q));
        Vector2f exit_point = getPosition() + (u_vector*30);

        double factor = sum_kg/(kg_per_s*dt);
        double neg_x = 1;
        double neg_y = 1;
        if(rand() % 100<50){neg_x=-1;}
        if(rand() % 100<50){neg_y=-1;}

        double x_excitement = neg_x*(rand()%25)/factor;
        double y_excitement = neg_y*(rand()%25)/factor;

        createEffect(sector_id, exit_point, (u_vector*100*factor)+Vector2f(x_excitement,y_excitement), "Smoke");



    applyImpulse(u_vector.x*(sum_kg)*-1500,u_vector.y*(sum_kg)*-1500,getPosition(),false);
    }

    return true;
}
*/

string registerNewMachine(string sector_id, string new_id, double x, double y, double rotation, string type, bool loading){

    if(type=="Piston"){

        shared_ptr<Piston> new_machine = make_shared<Piston>(sector_id, new_id, x, y, rotation, loading);
        getSector(sector_id)->machines[new_id] =  weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);

        if(getSector(sector_id)->ents.count(new_machine->id) != 0 && getSector(sector_id)->ents.count(new_machine->internal_cells["Sister ID"]->data["id"]) != 0){
            makeNewConnector(sector_id, createUniqueId(), "PISTON", new_machine->id, new_machine->internal_cells["Sister ID"]->data["id"]);
        }
    }
    if(type=="Piston Head"){

        shared_ptr<PistonHead> new_machine = make_shared<PistonHead>(sector_id, new_id, x, y, rotation);
        getSector(sector_id)->machines[new_id] =  weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);

        if(getSector(sector_id)->ents.count(new_machine->id) != 0 && getSector(sector_id)->ents.count(new_machine->internal_cells["Sister ID"]->data["id"]) != 0){
            makeNewConnector(sector_id, createUniqueId(), "PISTON", new_machine->id, new_machine->internal_cells["Sister ID"]->data["id"]);
        }
    }
    if(type=="Gas Tank"){

        shared_ptr<GasTank> new_machine = make_shared<GasTank>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] =  weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);

        new_machine->internal_cells["Stored Gas"]->vars["Nitrous Oxide"] = 6.4;
    }
    if(type=="Gas Vent"){

        shared_ptr<GasVent> new_machine = make_shared<GasVent>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] =  weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="Warp Drive"){

        shared_ptr<WarpDrive> new_machine = make_shared<WarpDrive>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] =  weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="Gas Valve"){

        shared_ptr<GasValve> new_machine = make_shared<GasValve>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] =  weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="Gas Pump"){

        shared_ptr<GasPump> new_machine = make_shared<GasPump>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] =  weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="Combustion Thruster"){

        shared_ptr<CombustionThruster> new_machine = make_shared<CombustionThruster>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] =  weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="Plasma Thruster"){

        shared_ptr<PlasmaThruster> new_machine = make_shared<PlasmaThruster>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] =  weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="Pipe Joint"){

        shared_ptr<PipeJoint> new_machine = make_shared<PipeJoint>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] =  weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="LED Bar"){

        shared_ptr<LEDBar> new_machine = make_shared<LEDBar>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] = weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="Transformer"){

        shared_ptr<Transformer> new_machine = make_shared<Transformer>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] = weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="240V Battery"){

        shared_ptr<Battery240V> new_machine = make_shared<Battery240V>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] = weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);


    }
    if(type=="12V Battery"){

        shared_ptr<Battery12V> new_machine = make_shared<Battery12V>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] = weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="Data Duplicator"){

        shared_ptr<DataDuplicator> new_machine = make_shared<DataDuplicator>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] = weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="Junction Box"){

        shared_ptr<JunctionBox> new_machine = make_shared<JunctionBox>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] = weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="Switch"){

        shared_ptr<Switch> new_machine = make_shared<Switch>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] = weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
  /*  if(type=="Propulsor"){

        shared_ptr<Propulsor> new_machine = make_shared<Propulsor>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] = weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }*/
    if(type=="Control Board"){

        shared_ptr<ControlBoard> new_machine = make_shared<ControlBoard>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] = weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="Beacon"){

        shared_ptr<Beacon> new_machine = make_shared<Beacon>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] = weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="Ironworker"){

        shared_ptr<Ironworker> new_machine = make_shared<Ironworker>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] = weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="Lever"){

        shared_ptr<Lever> new_machine = make_shared<Lever>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] = weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="Pressure Gauge"){

        shared_ptr<PressureGauge> new_machine = make_shared<PressureGauge>(sector_id, new_id, x, y);
        getSector(sector_id)->machines[new_id] = weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }
    if(type=="Terminal"){

        shared_ptr<Terminal> new_machine = make_shared<Terminal>(sector_id, new_id, x, y);
        getSector(sector_id)->terminals[new_id] = weak_ptr<Terminal>(new_machine);
        getSector(sector_id)->machines[new_id] = weak_ptr<Machine>(new_machine);
        getSector(sector_id)->ents[new_id] = shared_ptr<Entity>(new_machine);
    }

    return new_id;
}
