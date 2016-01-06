#include "main.h"

using namespace std;
using namespace sf;


extern World world;
extern RenderWindow window;


FlowPort::FlowPort(string new_sector_id, string new_id, Vector2f new_relative_coords, double new_relative_rotation, string new_machine_id, string type){

    properties = getProperties(type);

    if(type=="GPI Port"){
        is_input = true;
    }
    else if(type=="GPO Port"){
        is_input = false;
    }

    id = new_id;
    machine_id = new_machine_id;

    sector_id = new_sector_id;
    relative_coords = new_relative_coords;
    relative_rotation = new_relative_rotation;

    sprite.setTexture(properties->sheet,true);
    IntRect bounds_rect = sprite.getTextureRect();
    sprite.setOrigin(bounds_rect.width/2,bounds_rect.height/2);
}

void FlowPort::updatePosition(Vector2f parent_coords, double parent_rotation){

    Vector2f world_coords = parent_coords;
    double Q = parent_rotation*(M_PI/180);

    Vector2f new_position = world_coords + Vector2f(relative_coords.x*cos(Q)-relative_coords.y*sin(Q),relative_coords.x*sin(Q)+relative_coords.y*cos(Q));
    sprite.setPosition(new_position.x,new_position.y);
    sprite.setRotation(relative_rotation + (Q*(180/M_PI)));
}

void FlowPort::run(double dt){
/*
    Vector2f world_coords = getSector(sector_id)->getEnt(machine_id)->getPosition();
    double Q = getSector(sector_id)->getEnt(machine_id)->sprite.getRotation()*(M_PI/180);

    Vector2f new_position = world_coords + Vector2f(relative_coords.x*cos(Q)-relative_coords.y*sin(Q),relative_coords.x*sin(Q)+relative_coords.y*cos(Q));
    sprite.setPosition(new_position.x,new_position.y);
    sprite.setRotation(relative_rotation + (Q*(180/M_PI)));*/
    sprite.setColor(Color(getSector(sector_id)->ambient_light+20+light_level,getSector(sector_id)->ambient_light+20+light_level,getSector(sector_id)->ambient_light+20+light_level,255));
    light_level = 0;

    if(getSector(sector_id)->interfaces.count(connection)==0){
        voltage_in = 0.0;
        connection = "";
    }
}

void FlowPort::light(double amount)
{
    light_level += amount;
    if(light_level + getSector(sector_id)->ambient_light > 200){light_level = 200-getSector(sector_id)->ambient_light;}
}

bool FlowPort::isConnected(){

    if(connection!=""){return true;}
    else{return false;}
}

shared_ptr<FlowCell> FlowPort::getCell(int i){

    return getSector(sector_id)->getMachine(machine_id)->internal_cells[cell_ids[i]];
}

void FlowPort::tapCell(string new_cell_id){

    if(cell_ids.size()==0){
        cell_ids[1] = new_cell_id;
    }
    else{
        cell_ids[cell_ids.size()+1] = new_cell_id;
    }
}
void FlowPort::tapCell(string new_cell_id, int i){

    cell_ids[i] = new_cell_id;
}


FlowCell::FlowCell(){}
void FlowCell::recalculate(){}
map<string, double> FlowCell::take(double){

    map<string, double> empty_list;
    return empty_list;
}

GasCell::GasCell(string new_id, double new_volume){

    id = new_id;

    vars["volume"] = new_volume;
    vars["total kilomoles"] = 0;
    vars["total kilograms"] = 0;
    vars["total pressure"] = 0;

}

GPIOCell::GPIOCell(string new_id){

    id = new_id;

    vars["value"] = 0;
}

PowerCell::PowerCell(string new_id, double max_charge){

    id = new_id;

    vars["milliamp hours"] = 0;
    vars["max milliamp hours"] = max_charge;
    vars["running average"] = 0.0;
}

void PowerCell::recalculate(){

    charge_record[0] = charge_record[1];
    charge_record[1] = charge_record[2];
    charge_record[2] = charge_record[3];
    charge_record[3] = vars["milliamp hours"];

    vars["running average"] = (floor(((charge_record[0] + charge_record[1] + charge_record[2] + charge_record[3])/4)*100))/100;
}

ItemCell::ItemCell(string new_id, int slot_id){

    id = new_id;

    vars["storage index"] = static_cast<double>(slot_id); //can be cast to int index that corresponds to machine storage slot
}

map<string, double> GasCell::take(double amount){

    recalculate();

    map<string, double> return_quantities;

    set<string> delete_list;
    for(map<string, double>::iterator parser = vars.begin(); parser!=vars.end(); parser++){
        if(parser->first=="volume" or parser->first=="total kilomoles" or parser->first=="total kilograms" or parser->first=="total pressure"){
            continue;
        }
        double molar_mass = getProperties(parser->first)->get("molar_mass");
        double kg_to_take = amount * ((parser->second*molar_mass)/vars["total kilograms"]);
        double kilomoles_to_take = kg_to_take/molar_mass;
        if(kilomoles_to_take > parser->second){
            kilomoles_to_take = parser->second;
        }

        parser->second -= kilomoles_to_take;
        return_quantities[parser->first] = kilomoles_to_take;
        if(parser->second <= 0.001){
            delete_list.insert(parser->first);
        }
    }

    for(set<string>::iterator parser = delete_list.begin(); parser!=delete_list.end(); parser++){
        vars.erase(*parser);
    }

    return return_quantities;
}

void GasCell::recalculate(){

    //calculate kilomoles
    vars["total kilomoles"] = 0;
    for(map<string, double>::iterator parser = vars.begin(); parser!=vars.end(); parser++){
        if(parser->first=="volume" or parser->first=="total kilomoles" or parser->first=="total kilograms" or parser->first=="total pressure"){
            continue;
        }
        vars["total kilomoles"] += parser->second;
    }

    //calculate kilograms
    vars["total kilograms"] = 0;
    for(map<string, double>::iterator parser = vars.begin(); parser!=vars.end(); parser++){
        if(parser->first=="volume" or parser->first=="total kilomoles" or parser->first=="total kilograms" or parser->first=="total pressure"){
            continue;
        }
        vars["total kilograms"] += (parser->second*1000*getProperties(parser->first)->get("molar_mass"));
    }

    //calculate pressure

    vars["total pressure"] = convertToPSI(vars["total kilomoles"]*1000/vars["volume"]);


}


Machine::Machine(){}
Machine::Machine(string new_sector_id, string e_id, double x, double y, string type)
{
    constructEntity(new_sector_id, e_id, x, y,type);

    setSlots(36); //36 arbitrarily chosen to give "breathing room"
}

void Machine::disconnect(){

    for(map<string, shared_ptr<FlowPort> >::iterator parser = ports.begin(); parser != ports.end(); parser++){

        string connection_id = parser->second->connection;
        if(getSector(sector_id)->interfaces.count(connection_id) != 0){
            getSector(sector_id)->interfaces[connection_id]->disconnect();
            getSector(sector_id)->trash(connection_id);
        }
    }
}

int Machine::getFirstUnusedSlot(){

    for(map<int,string>::iterator i = contents.begin(); i!=contents.end(); i++){
        int index = i->first;
        bool already_in_use = false;
        for(map<string,shared_ptr<FlowCell> >::iterator parser = internal_cells.begin(); parser!= internal_cells.end(); parser++){

            if(parser->second->vars.count("storage index")!=0 and parser->second->vars["storage index"]==index){
                already_in_use = true;
                break;
            }
        }
        if(already_in_use){
            continue;
        }
        return index;
    }
}

void Machine::addPort(string new_id, Vector2f new_coords, double new_rotation, string type){

    ports[new_id] = make_shared<FlowPort>(sector_id, new_id, new_coords, new_rotation, id, type);

    Vector2f world_coords = getPosition();
    double Q = sprite.getRotation()*(M_PI/180);

    Vector2f new_position = world_coords + Vector2f(ports[new_id]->relative_coords.x*cos(Q)-ports[new_id]->relative_coords.y*sin(Q),ports[new_id]->relative_coords.x*sin(Q)+ports[new_id]->relative_coords.y*cos(Q));
    ports[new_id]->sprite.setPosition(new_position.x,new_position.y);
    ports[new_id]->sprite.setRotation(new_rotation + (Q*(180/M_PI)));
}

void Machine::run(double dt){

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

  //  performFunction(dt);
}

void Machine::readInput(InputData new_input){

    input = new_input;
}

void Machine::draw(){

    sprite.setPosition(coords.x,coords.y);
    window.draw(sprite);

    for(map<string, shared_ptr<FlowPort> >::iterator parser = ports.begin(); parser!=ports.end(); parser++)
    {
        parser->second->updatePosition(getPosition(),getRotation());
        window.draw(parser->second->sprite);
    }

}

//void Machine::eject(string port_id, double kg){
//
//    shared_ptr<FlowCell> cell = ports[port_id]->getCell();
//
//
//    map<string, double> moving_quantities = cell->take(kg);
//    for(map<string, double>::iterator parser = moving_quantities.begin(); parser!=moving_quantities.end(); parser++){
//
//        shared_ptr<Cloud> found_cloud;
//        for(map<string, shared_ptr<Cloud> >::iterator parser2 = getSector(sector_id)->clouds.begin(); parser2!=getSector(sector_id)->clouds.end(); parser2++){
//
//            if(parser2->second->inRange(getPosition(),125) and parser2->second->properties->name==parser->first){
//
//                found_cloud = parser2->second;
//                break;
//            }
//        }
//
//        Vector2f starting_pos = Vector2f(ports[port_id]->sprite.getPosition().x,ports[port_id]->sprite.getPosition().y);
//        Vector2f dist = starting_pos - getPosition();
//        double h = hypot(dist.x,dist.y);
//        Vector2f u_vector = dist/h;
//
//        Vector2f starting_vel = u_vector * 600;
//
//
//        if(found_cloud){
//
//            found_cloud->addKilomoles(parser->second);
//
//        }
//        else{
//
//            string flow_id = parser->first;
//            string new_id = createUniqueId();
//
//            getSector(sector_id)->clouds[new_id] = make_shared<Cloud>(sector_id, new_id,parser->second,starting_pos,starting_vel,flow_id);
//        }
//    }
//
//    applyImpulse(0,-200*kg/1000,getPosition(),false);
//}

//void Machine::inject(string port_id, double kg){
//
//    shared_ptr<FlowCell> cell = ports[port_id]->getCell();
//
//    map<string, double> moving_quantities = getSector(sector_id)->liftKilograms(getPosition()+Vector2f(0,1),125,kg);
//
//    for(map<string, double>::iterator parser = moving_quantities.begin(); parser!=moving_quantities.end(); parser++){
//
//        if(cell->vars.count(parser->first)==0){
//            cell->vars[parser->first] = parser->second;
//        }
//        else{
//            cell->vars[parser->first] += parser->second;
//        }
//    }
//
//}

bool Machine::performFunction(double dt)
{
    return true;
}


string makeNewMachine(string sector_id, string ent_id, double x, double y, double rotation, string info_key, bool loading){

    registerNewMachine(sector_id, ent_id, x, y, rotation, info_key, loading);
    for(map<string, shared_ptr<FlowPort> >::iterator parser = getSector(sector_id)->getMachine(ent_id)->ports.begin(); parser != getSector(sector_id)->getMachine(ent_id)->ports.end(); parser++){
        parser->second->updatePosition(Vector2f(x,y), rotation);
    }
    getSector(sector_id)->ents[ent_id]->setRotation(rotation);
    return ent_id;
}

