#include "main.h"

using namespace std;
using namespace sf;


extern World world;
extern RenderWindow window;

Interface::Interface(){}

void Interface::runPhysicsAndGraphics(double dt){


    Vector2f center = Vector2f(getPort()->sprite.getPosition().x,getPort()->sprite.getPosition().y);

    for(int i = 0; i < sprite.size(); i++){

        double Q = getPort()->sprite.getRotation() * (M_PI/180);

        sprite[i].setColor(Color(getSector(sector_id)->ambient_light+20+light_level[i],getSector(sector_id)->ambient_light+20+light_level[i],getSector(sector_id)->ambient_light+20+light_level[i],255));
        light_level[i] = 0;

        Vector2f new_position = center + Vector2f(relative_position[i].x*cos(Q)-relative_position[i].y*sin(Q),relative_position[i].x*sin(Q)+relative_position[i].y*cos(Q));
        sprite[i].setPosition(new_position.x,new_position.y);

        sprite[i].setRotation(getPort()->sprite.getRotation() + relative_rotation[i]);
    }


}

shared_ptr<FlowPort> Interface::getPort(){

    return getMachine()->ports[port_id];
}

shared_ptr<Machine> Interface::getMachine(){
    return getSector(sector_id)->machines[machine_id].lock();
}

void Interface::disconnect(){

    if(getSector(sector_id)->machines.count(machine_id) != 0){
        getSector(sector_id)->machines[machine_id].lock()->ports[port_id]->connection = "";
    }
    if(getSector(sector_id)->machines.count(machine_id_2) != 0){
        getSector(sector_id)->machines[machine_id_2].lock()->ports[port_id_2]->connection = "";
    }
}

void Interface::run(double dt){

    runPhysicsAndGraphics(dt);
}

void Interface::activate(){}

void Interface::draw(){

    for(int i = 0; i < sprite.size(); i++){
        window.draw(sprite[i]);
    }
}

Transfer::Transfer(){}

shared_ptr<FlowPort> Transfer::getPort2(){

    return getMachine2()->ports[port_id_2];
}

shared_ptr<Machine> Transfer::getMachine2(){

    return getSector(sector_id)->machines[machine_id_2].lock();
}

void Transfer::setConnection(shared_ptr<FlowPort> A, shared_ptr<FlowPort> B){

    if(port_id!="" and port_id_2!=""){
        getPort()->connection = "";
        getPort2()->connection = "";
    }

    machine_id = A->machine_id;
    port_id = A->id;
    machine_id_2 = B->machine_id;
    port_id_2 = B->id;

    generateSections();

    getPort()->connection = id;
    getPort2()->connection = id;
}

void Transfer::run(double dt){

    transfer(dt);
    updateSections();
}

void Transfer::generateSections(){

    physical_id = makeNewConnector(sector_id, id, "PIPE", machine_id, machine_id_2);

    sprite.clear();
    relative_position.clear();
    relative_rotation.clear();
    light_level.clear();

    Vector2f start_point = Vector2f(getPort()->sprite.getPosition().x,getPort()->sprite.getPosition().y);
    Vector2f end_point = Vector2f(getPort2()->sprite.getPosition().x,getPort2()->sprite.getPosition().y);

    Vector2f relative_end_point = end_point - start_point;

    double Q = getPort()->sprite.getRotation() * (M_PI/180);
    Vector2f unrotated_end_point = Vector2f(relative_end_point.x*cos(-Q)-relative_end_point.y*sin(-Q),relative_end_point.x*sin(-Q)+relative_end_point.y*cos(-Q));

    double end_point_rotation = formatRotation((getPort2()->sprite.getRotation() - getPort()->sprite.getRotation())*(M_PI/180));

    Vector2f segment_vector_1(0,1);
    Vector2f segment_vector_2(sin(end_point_rotation),-cos(end_point_rotation));

    double h = hypot(unrotated_end_point.x,unrotated_end_point.y);

    Vector2f midpoint_1 = segment_vector_1*(0.3*h);
    Vector2f midpoint_2 = unrotated_end_point - segment_vector_2*(0.3*h);

    if(hypot(unrotated_end_point.x - midpoint_1.x, unrotated_end_point.y - midpoint_1.y) > h){
        midpoint_1 = (segment_vector_1*-1)*(0.3*h);
        segment_vector_1 = segment_vector_1 * -1;
    }
    if(hypot(midpoint_2.x, midpoint_2.y) > h){
        midpoint_2 = unrotated_end_point - (segment_vector_2*-1)*(0.3*h);
        segment_vector_2 = segment_vector_2 * -1;
    }



    IntRect window_rect = IntRect(0,0,properties->get("width"),properties->get("height"));
    Sprite first_sprite;
    first_sprite.setTexture(properties->sheet,false);
    first_sprite.setTextureRect(window_rect);
    first_sprite.setOrigin(properties->get("width")/2,properties->get("height")/2);

    Sprite chute_sprite;
    chute_sprite.setTexture(properties->sheet,false);
    window_rect.left = properties->get("width");
    chute_sprite.setTextureRect(window_rect);
    chute_sprite.setOrigin(properties->get("width")/2,properties->get("height")/2);

    Sprite last_sprite;
    last_sprite.setTexture(properties->sheet,false);
    window_rect.left = 2*properties->get("width");
    last_sprite.setTextureRect(window_rect);
    last_sprite.setOrigin(properties->get("width")/2,properties->get("height")/2);

    for(int i = 0; i < ceil((0.3*h)/properties->get("height")); i++){
        Vector2f new_position = (segment_vector_1*properties->get("height")*i);
        sprite.push_back(chute_sprite);
        relative_position.push_back(new_position);
        relative_rotation.push_back(getRotationFromAxis(segment_vector_1));
        light_level.push_back(0);
    }
    for(int i = 0; i < ceil((0.3*h)/properties->get("height")); i++){
        Vector2f new_position = midpoint_2 + (segment_vector_2*properties->get("height")*i);
        sprite.push_back(chute_sprite);
        relative_position.push_back(new_position);
        relative_rotation.push_back(getRotationFromAxis(segment_vector_2));
        light_level.push_back(0);
    }

    sprite.push_back(first_sprite);
    relative_position.push_back(Vector2f(0,0));
    relative_rotation.push_back(getRotationFromAxis(segment_vector_1));
    light_level.push_back(0);

    sprite.push_back(last_sprite);
    relative_position.push_back(unrotated_end_point);
    relative_rotation.push_back(getRotationFromAxis(segment_vector_2));
    light_level.push_back(0);

    Vector2f mid_vector = midpoint_2 - midpoint_1;
    double mid_length = hypot(mid_vector.x, mid_vector.y);
    mid_vector = mid_vector/mid_length;

    for(int i = 0; i < mid_length/properties->get("height"); i++){
        Vector2f new_position = midpoint_1 + (mid_vector*properties->get("height")*i);
        sprite.push_back(chute_sprite);
        relative_position.push_back(new_position);
        relative_rotation.push_back(getRotationFromAxis(mid_vector));
        light_level.push_back(0);
    }

    Sprite joint_sprite;
    joint_sprite.setTexture(properties->sheet,false);
    window_rect.left = 0;
    window_rect.height = properties->get("height")*2;
    window_rect.top = properties->get("height");
    joint_sprite.setTextureRect(window_rect);
    joint_sprite.setOrigin(properties->get("width")/2,properties->get("height"));

    relative_position.push_back(midpoint_1);
    relative_rotation.push_back(getRotationFromAxis(segment_vector_1));
    sprite.push_back(joint_sprite);
    light_level.push_back(0);

    relative_position.push_back(midpoint_1);
    relative_rotation.push_back(getRotationFromAxis(mid_vector*-1));
    sprite.push_back(joint_sprite);
    light_level.push_back(0);

    relative_position.push_back(midpoint_2);
    relative_rotation.push_back(getRotationFromAxis(mid_vector));
    sprite.push_back(joint_sprite);
    light_level.push_back(0);

    relative_position.push_back(midpoint_2);
    relative_rotation.push_back(getRotationFromAxis(segment_vector_2*-1));
    sprite.push_back(joint_sprite);
    light_level.push_back(0);

}
void Transfer::updateSections(){

    if(getSector(sector_id)->connectors.count(physical_id)==0){
        getSector(sector_id)->trash(id);
        return;
    }

    Vector2f start_point = Vector2f(getPort()->sprite.getPosition().x,getPort()->sprite.getPosition().y);

    double Q = (getPort()->sprite.getRotation()) * (M_PI/180);

    for(int i = 0; i < sprite.size(); i++){

        sprite[i].setColor(Color(getSector(sector_id)->ambient_light+20+light_level[i],getSector(sector_id)->ambient_light+20+light_level[i],getSector(sector_id)->ambient_light+20+light_level[i],255));
        light_level[i] = 0;

        Vector2f new_position = start_point + Vector2f(relative_position[i].x*cos(Q)-relative_position[i].y*sin(Q),relative_position[i].x*sin(Q)+relative_position[i].y*cos(Q));
        sprite[i].setPosition(new_position.x,new_position.y);
        sprite[i].setRotation(relative_rotation[i] + getPort()->sprite.getRotation());
    }
}
void Transfer::transfer(double dt){}

TransferGas::TransferGas(string new_sector_id,string new_id, shared_ptr<FlowPort> A, shared_ptr<FlowPort> B, string type){

    sector_id = new_sector_id;
    id = new_id;
    properties = getProperties(type);
    setConnection(A,B);
}

void TransferGas::transfer(double dt){

    shared_ptr<FlowCell> A = getPort()->getCell();
    shared_ptr<FlowCell> B = getPort2()->getCell();

    if(getPort()->closed || getPort2()->closed){
        return;
    }

    A->recalculate();
    B->recalculate();

    double kg_A = A->vars["total kilograms"];
    double kg_B = B->vars["total kilograms"];
    double volume_A = A->vars["volume"];
    double volume_B = B->vars["volume"];

    double new_pressure = (kg_A+kg_B)/(volume_A+volume_B);

    double kg_to_move = 100*(kg_B-(new_pressure*volume_B)) * dt; //the change that is required to make the pressure in B == the new pressure

    if(kg_to_move > 0){

        kg_to_move = abs(kg_to_move);

        if(kg_to_move > kg_B){

            kg_to_move = kg_B;
        }

        map<string, double> moving_quantities = B->take(kg_to_move);
        for(map<string, double>::iterator parser = moving_quantities.begin(); parser!=moving_quantities.end(); parser++){

            if(A->vars.count(parser->first)==0){
                A->vars[parser->first] = parser->second;
            }
            else{
                A->vars[parser->first] += parser->second;
            }
        }
    }
    else{

        kg_to_move = abs(kg_to_move);

        if(kg_to_move > kg_A){

            kg_to_move = kg_A;
        }

        map<string, double> moving_quantities = A->take(kg_to_move);
        for(map<string, double>::iterator parser = moving_quantities.begin(); parser!=moving_quantities.end(); parser++){

            if(B->vars.count(parser->first)==0){
                B->vars[parser->first] = parser->second;
            }
            else{
                B->vars[parser->first] += parser->second;
            }
        }
    }

}

TransferPower::TransferPower(string new_sector_id,string new_id, shared_ptr<FlowPort> A, shared_ptr<FlowPort> B){

    sector_id = new_sector_id;
    id = new_id;

    properties = getProperties("Power Cable");
    setConnection(A,B);
}

void TransferPower::generateSections(){

    physical_id = id;

    if(getSector(sector_id)->ropes.count(id) == 0){
        getSector(sector_id)->ropes[id] = make_shared<Rope>(sector_id, id, Vector2f(getPort()->sprite.getPosition().x,getPort()->sprite.getPosition().y), Vector2f(getPort2()->sprite.getPosition().x,getPort2()->sprite.getPosition().y), "Insulated Cable");
        getSector(sector_id)->ropes[id]->attach(getSector(sector_id)->ropes[id]->nodes.size()-1, machine_id, getPort()->relative_coords);
        getSector(sector_id)->ropes[id]->attach(0,machine_id_2, getPort2()->relative_coords);
    }
}

void TransferPower::updateSections(){

    if(getSector(sector_id)->ropes.count(physical_id)==0){
        getSector(sector_id)->trash(id);
        return;
    }

}

void TransferPower::transfer(double dt){

    shared_ptr<FlowCell> A = getPort()->getCell();
    shared_ptr<FlowCell> B = getPort2()->getCell();

    if(getMachine2()->voltage_rating != 0 && getPort()->voltage > getMachine2()->voltage_rating + 5){
        //blowup machine 2
        createEffect(sector_id, getMachine2()->getPosition(), Vector2f(0,0), "Smoke");
        createEffect(sector_id, getMachine2()->getPosition(), Vector2f(0,0), "Sparks");
        getSector(sector_id)->trash(id);
        getSector(sector_id)->trash(machine_id_2);
        return;
    }
    if(getMachine()->voltage_rating != 0 && getPort2()->voltage > getMachine()->voltage_rating + 5){
        //blowup machine 1
        createEffect(sector_id, getMachine()->getPosition(), Vector2f(0,0), "Smoke");
        createEffect(sector_id, getMachine2()->getPosition(), Vector2f(0,0), "Sparks");
        getSector(sector_id)->trash(id);
        getSector(sector_id)->trash(machine_id);
        return;
    }

    getPort()->voltage_in = getPort2()->voltage;
    getPort2()->voltage_in = getPort()->voltage;

    if(getPort()->voltage == getPort2()->voltage){
        if(getMachine()->voltage_rating != 0 && getPort2()->voltage < getMachine()->voltage_rating - 5){
            //not enough voltage to push through
            return;
        }
        if(getMachine2()->voltage_rating != 0 && getPort()->voltage < getMachine2()->voltage_rating - 5){
            //not enough voltage to push through
            return;
        }
    }
    else if(getPort()->voltage > getPort2()->voltage){
        if(getMachine2()->voltage_rating != 0 && getPort()->voltage < getMachine2()->voltage_rating - 5){
            //not enough voltage to push through
            return;
        }
    }
    else if(getPort2()->voltage > getPort()->voltage){
        if(getMachine()->voltage_rating != 0 && getPort2()->voltage < getMachine()->voltage_rating - 5){
            //not enough voltage to push through
            return;
        }
    }

    //otherwise



    if(getPort()->voltage == getPort2()->voltage){ //equalize

        if(A->vars["milliamp hours"] > B->vars["milliamp hours"]){

            double difference = A->vars["milliamp hours"] - B->vars["milliamp hours"];
            if(B->vars["milliamp hours"] + difference > B->vars["max milliamp hours"]){
                difference = B->vars["max amp hours"] - B->vars["milliamp hours"];
            }
            A->vars["milliamp hours"] -= difference;
            B->vars["milliamp hours"] += difference;
        }
        else if(B->vars["milliamp hours"] > A->vars["milliamp hours"]){
            double difference = B->vars["milliamp hours"] - A->vars["milliamp hours"];
            if(A->vars["milliamp hours"] + difference > A->vars["max milliamp hours"]){
                difference = A->vars["max milliamp hours"] - A->vars["milliamp hours"];
            }
            B->vars["milliamp hours"] -= difference;
            A->vars["milliamp hours"] += difference;
        }
    }
    else if(getPort()->voltage > getPort2()->voltage){ //send A to B

        double amount_to_transfer = A->vars["milliamp hours"];
        if(B->vars["milliamp hours"] + amount_to_transfer > B->vars["max milliamp hours"]){
            amount_to_transfer = B->vars["max milliamp hours"] - B->vars["milliamp hours"];
        }
        A->vars["milliamp hours"] -= amount_to_transfer;
        B->vars["milliamp hours"] += amount_to_transfer;
    }
    else if(getPort2()->voltage > getPort()->voltage){ //send B to A

        double amount_to_transfer = B->vars["milliamp hours"];
        if(A->vars["milliamp hours"] + amount_to_transfer > A->vars["max milliamp hours"]){
            amount_to_transfer = A->vars["max milliamp hours"] - A->vars["milliamp hours"];
        }
        B->vars["milliamp hours"] -= amount_to_transfer;
        A->vars["milliamp hours"] += amount_to_transfer;
    }
}

TransferGPIO::TransferGPIO(string new_sector_id,string new_id, shared_ptr<FlowPort> A, shared_ptr<FlowPort> B){

    sector_id = new_sector_id;
    id = new_id;

    properties = getProperties("GPIO Cable");
    setConnection(A,B);
}

void TransferGPIO::generateSections(){

    physical_id = id;

    if(getSector(sector_id)->ropes.count(id) == 0){
        getSector(sector_id)->ropes[id] = make_shared<Rope>(sector_id, id, Vector2f(getPort()->sprite.getPosition().x,getPort()->sprite.getPosition().y), Vector2f(getPort2()->sprite.getPosition().x,getPort2()->sprite.getPosition().y), "Data Cable");
        getSector(sector_id)->ropes[id]->attach(getSector(sector_id)->ropes[id]->nodes.size()-1, machine_id, getPort()->relative_coords);
        getSector(sector_id)->ropes[id]->attach(0,machine_id_2, getPort2()->relative_coords);
    }
}

void TransferGPIO::updateSections(){

    if(getSector(sector_id)->ropes.count(physical_id)==0){
        getSector(sector_id)->trash(id);
        return;
    }

}

void TransferGPIO::transfer(double dt){

    shared_ptr<FlowPort> A = getPort();
    shared_ptr<FlowPort> B = getPort2();

    if(A->is_input){
        A->getCell()->vars["value"] = B->getCell()->vars["value"];
    }
    else if(B->is_input){
        B->getCell()->vars["value"] = A->getCell()->vars["value"];
    }
}

string registerNewInterface(string sector_id, string new_id, string port_id, string machine_id, Vector2f new_coords, string type){


    return new_id;
}

string registerNewTransfer(string sector_id, string new_id, string port_id_a, string port_id_b, string machine_id_a, string machine_id_b, string type){

    if(type=="Steel Pipe" || type=="Hazard Pipe"){
        getSector(sector_id)->interfaces[new_id] = make_shared<TransferGas>(sector_id, new_id, getSector(sector_id)->getMachine(machine_id_a)->ports[port_id_a],  getSector(sector_id)->getMachine(machine_id_b)->ports[port_id_b], type);
    }
    if(type=="Power Cable"){
        getSector(sector_id)->interfaces[new_id] = make_shared<TransferPower>(sector_id, new_id, getSector(sector_id)->getMachine(machine_id_a)->ports[port_id_a],  getSector(sector_id)->getMachine(machine_id_b)->ports[port_id_b]);
    }
    if(type=="GPIO Cable"){
        getSector(sector_id)->interfaces[new_id] = make_shared<TransferGPIO>(sector_id, new_id, getSector(sector_id)->getMachine(machine_id_a)->ports[port_id_a],  getSector(sector_id)->getMachine(machine_id_b)->ports[port_id_b]);
    }
    return new_id;
}

string makeNewInterface(string sector_id, string new_id, string port_id, string machine_id, Vector2f new_coords, string type){
    return registerNewInterface(sector_id,new_id,port_id,machine_id,new_coords,type);
}

string makeNewTransfer(string sector_id, string new_id, string port_id_a, string port_id_b, string machine_id_a, string machine_id_b, string type){

    return registerNewTransfer(sector_id,new_id, port_id_a,port_id_b,machine_id_a,machine_id_b,type);
}
