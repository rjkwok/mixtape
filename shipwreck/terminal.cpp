#include "main.h"

using namespace std;
using namespace sf;

extern RenderWindow window;
extern World world;

Chip::Chip(){}

Chip::Chip(string new_sector_id, string item_id){

    properties = getProperties("chip");
    sector_id = new_sector_id;
    id = item_id;
}

void Chip::process(double dt){

}

SwitchChip::SwitchChip(){}

SwitchChip::SwitchChip(string new_sector_id, string item_id){

    properties = getProperties("Switch v1.0 (Chip)");
    sector_id = new_sector_id;
    id = item_id;

    var_num[1] = 0;
    var_num[2] = 0;
    var_num[3] = 0;
    var_num[4] = 0;
}

void SwitchChip::process(double dt){

    shared_ptr<Terminal> terminal = getSector(sector_id)->terminals[storage_id].lock();

    RectangleShape button_A = createRectangle(Vector2f(600, 335), Vector2f(100,50), 3, Color(0,0,0,0), Color(255,255,255,255));
    if(button_A.getGlobalBounds().contains(terminal->input.fixed_mouse)){
        button_A.setFillColor(Color(155,155,155,55));
        if(terminal->input.lmb_released){
            if(var_num[1] == 1){ var_num[1] = 0; }
            else{ var_num[1] = 1; }
        }
    }
    Caption caption_A = Caption("SWITCH A", "font1", button_A.getPosition(), 28);
    Caption status_A;
    if(var_num[1] == 1){ status_A = Caption(":ON", "font1", button_A.getPosition() + Vector2f(60, 0), 28, Color(255,255,255,200), "left"); }
    else{ status_A = Caption(":OFF", "font1", button_A.getPosition() + Vector2f(60, 0), 28, Color(255,255,255,200), "left"); }

    RectangleShape button_B = createRectangle(Vector2f(600, 415), Vector2f(100,50), 3, Color(0,0,0,0), Color(255,255,255,255));
    if(button_B.getGlobalBounds().contains(terminal->input.fixed_mouse)){
        button_B.setFillColor(Color(155,155,155,55));
        if(terminal->input.lmb_released){
            if(var_num[2] == 1){ var_num[2] = 0; }
            else{ var_num[2] = 1; }
        }
    }
    Caption caption_B = Caption("SWITCH B", "font1", button_B.getPosition(), 28);
    Caption status_B;
    if(var_num[2] == 1){ status_B = Caption(":ON", "font1", button_B.getPosition() + Vector2f(60, 0), 28, Color(255,255,255,200), "left"); }
    else{ status_B = Caption(":OFF", "font1", button_B.getPosition() + Vector2f(60, 0), 28, Color(255,255,255,200), "left"); }

    RectangleShape button_C = createRectangle(Vector2f(600, 495), Vector2f(100,50), 3, Color(0,0,0,0), Color(255,255,255,255));
    if(button_C.getGlobalBounds().contains(terminal->input.fixed_mouse)){
        button_C.setFillColor(Color(155,155,155,55));
        if(terminal->input.lmb_released){
            if(var_num[3] == 1){ var_num[3] = 0; }
            else{ var_num[3] = 1; }
        }
    }
    Caption caption_C = Caption("SWITCH C", "font1", button_C.getPosition(), 28);
    Caption status_C;
    if(var_num[3] == 1){ status_C = Caption(":ON", "font1", button_C.getPosition() + Vector2f(60, 0), 28, Color(255,255,255,200), "left"); }
    else{ status_C = Caption(":OFF", "font1", button_C.getPosition() + Vector2f(60, 0), 28, Color(255,255,255,200), "left"); }

    RectangleShape button_D = createRectangle(Vector2f(600, 575), Vector2f(100,50), 3, Color(0,0,0,0), Color(255,255,255,255));
    if(button_D.getGlobalBounds().contains(terminal->input.fixed_mouse)){
        button_D.setFillColor(Color(155,155,155,55));
        if(terminal->input.lmb_released){
            if(var_num[4] == 1){ var_num[4] = 0; }
            else{ var_num[4] = 1; }
        }
    }
    Caption caption_D = Caption("SWITCH D", "font1", button_D.getPosition(), 28);
    Caption status_D;
    if(var_num[4] == 1){ status_D = Caption(":ON", "font1", button_D.getPosition() + Vector2f(60, 0), 28, Color(255,255,255,200), "left"); }
    else{ status_D = Caption(":OFF", "font1", button_D.getPosition() + Vector2f(60, 0), 28, Color(255,255,255,200), "left"); }

    terminal->fixed_rectangles.push_back(button_A);
    terminal->captions.push_back(caption_A);
    terminal->captions.push_back(status_A);
    terminal->fixed_rectangles.push_back(button_B);
    terminal->captions.push_back(caption_B);
    terminal->captions.push_back(status_B);
    terminal->fixed_rectangles.push_back(button_C);
    terminal->captions.push_back(caption_C);
    terminal->captions.push_back(status_C);
    terminal->fixed_rectangles.push_back(button_D);
    terminal->captions.push_back(caption_D);
    terminal->captions.push_back(status_D);
/*
    terminal->ports["GPIO A"]->write("1:" + asString(var_num[1]) + ";");
    terminal->ports["GPIO B"]->write("1:" + asString(var_num[2]) + ";");
    terminal->ports["GPIO C"]->write("1:" + asString(var_num[3]) + ";");
    terminal->ports["GPIO D"]->write("1:" + asString(var_num[4]) + ";");*/
}

BatteryChip::BatteryChip(){}

BatteryChip::BatteryChip(string new_sector_id, string item_id){

    properties = getProperties("Battery v1.0 (Chip)");
    sector_id = new_sector_id;
    id = item_id;
}

void BatteryChip::process(double dt){

    shared_ptr<Terminal> terminal = getSector(sector_id)->terminals[storage_id].lock();

    vector<string> ports_to_display;
    for(map<string, shared_ptr<FlowPort> >::iterator parser = terminal->ports.begin(); parser != terminal->ports.end(); parser++){
        if(parser->second->properties->get("port_type") == 2 && parser->second->connection != ""){
            ports_to_display.push_back(parser->first);
        }
    }

    if(ports_to_display.size()==1){

        terminal->fixed_rectangles.push_back(createRectangle(Vector2f(terminal->terminal_window.left+5+200, terminal->terminal_window.top+5+200),Vector2f(380,380), 3, Color(0,0,0,0), Color(255,255,255,255)));
        terminal->captions.push_back(Caption("__" + ports_to_display[0] + "__", "font1", Vector2f(terminal->terminal_window.left+140, terminal->terminal_window.top+110), 40, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Current Charge: " + asString(terminal->ports[ports_to_display[0]]->getCell(3)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+75, terminal->terminal_window.top+165), 32, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Maximum Charge: " + asString(terminal->ports[ports_to_display[0]]->getCell(4)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+75, terminal->terminal_window.top+180), 32, Color(255,255,255,255), "left"));
    }
    else if(ports_to_display.size()==2){

        terminal->fixed_rectangles.push_back(createRectangle(Vector2f(terminal->terminal_window.left+5+200, terminal->terminal_window.top+105),Vector2f(380,185), 3, Color(0,0,0,0), Color(255,255,255,255)));
        terminal->captions.push_back(Caption("__" + ports_to_display[0] + "__", "font1", Vector2f(terminal->terminal_window.left+140, terminal->terminal_window.top+65), 32, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Current Charge: " + asString(terminal->ports[ports_to_display[0]]->getCell(3)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+75, terminal->terminal_window.top+105), 25, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Maximum Charge: " + asString(terminal->ports[ports_to_display[0]]->getCell(4)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+75, terminal->terminal_window.top+120), 25, Color(255,255,255,255), "left"));

        terminal->fixed_rectangles.push_back(createRectangle(Vector2f(terminal->terminal_window.left+5+200, terminal->terminal_window.top+305),Vector2f(380,185), 3, Color(0,0,0,0), Color(255,255,255,255)));
        terminal->captions.push_back(Caption("__" + ports_to_display[1] + "__", "font1", Vector2f(terminal->terminal_window.left+140, terminal->terminal_window.top+265), 32, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Current Charge: " + asString(terminal->ports[ports_to_display[1]]->getCell(3)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+75, terminal->terminal_window.top+305), 25, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Maximum Charge: " + asString(terminal->ports[ports_to_display[1]]->getCell(4)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+75, terminal->terminal_window.top+320), 25, Color(255,255,255,255), "left"));
    }
    else if(ports_to_display.size()==3){

        terminal->fixed_rectangles.push_back(createRectangle(Vector2f(terminal->terminal_window.left+105, terminal->terminal_window.top+105),Vector2f(185,185), 3, Color(0,0,0,0), Color(255,255,255,255)));
        terminal->captions.push_back(Caption("__" + ports_to_display[0] + "__", "font1", Vector2f(terminal->terminal_window.left+65, terminal->terminal_window.top+65), 25, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Current Charge: " + asString(terminal->ports[ports_to_display[0]]->getCell(3)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+20, terminal->terminal_window.top+105), 20, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Maximum Charge: " + asString(terminal->ports[ports_to_display[0]]->getCell(4)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+20, terminal->terminal_window.top+120), 20, Color(255,255,255,255), "left"));

        terminal->fixed_rectangles.push_back(createRectangle(Vector2f(terminal->terminal_window.left+305, terminal->terminal_window.top+105),Vector2f(185,185), 3, Color(0,0,0,0), Color(255,255,255,255)));
        terminal->captions.push_back(Caption("__" + ports_to_display[1] + "__", "font1", Vector2f(terminal->terminal_window.left+265, terminal->terminal_window.top+65), 25, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Current Charge: " + asString(terminal->ports[ports_to_display[1]]->getCell(3)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+220, terminal->terminal_window.top+105), 20, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Maximum Charge: " + asString(terminal->ports[ports_to_display[1]]->getCell(4)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+220, terminal->terminal_window.top+120), 20, Color(255,255,255,255), "left"));

        terminal->fixed_rectangles.push_back(createRectangle(Vector2f(terminal->terminal_window.left+5+200, terminal->terminal_window.top+305),Vector2f(380,185), 3, Color(0,0,0,0), Color(255,255,255,255)));
        terminal->captions.push_back(Caption("__" + ports_to_display[2] + "__", "font1", Vector2f(terminal->terminal_window.left+140, terminal->terminal_window.top+265), 32, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Current Charge: " + asString(terminal->ports[ports_to_display[2]]->getCell(3)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+75, terminal->terminal_window.top+305), 25, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Maximum Charge: " + asString(terminal->ports[ports_to_display[2]]->getCell(4)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+75, terminal->terminal_window.top+320), 25, Color(255,255,255,255), "left"));
    }
    else if(ports_to_display.size()==4){
        terminal->fixed_rectangles.push_back(createRectangle(Vector2f(terminal->terminal_window.left+105, terminal->terminal_window.top+105),Vector2f(185,185), 3, Color(0,0,0,0), Color(255,255,255,255)));
        terminal->captions.push_back(Caption("__" + ports_to_display[0] + "__", "font1", Vector2f(terminal->terminal_window.left+65, terminal->terminal_window.top+65), 25, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Current Charge: " + asString(terminal->ports[ports_to_display[0]]->getCell(3)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+20, terminal->terminal_window.top+105), 20, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Maximum Charge: " + asString(terminal->ports[ports_to_display[0]]->getCell(4)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+20, terminal->terminal_window.top+120), 20, Color(255,255,255,255), "left"));

        terminal->fixed_rectangles.push_back(createRectangle(Vector2f(terminal->terminal_window.left+305, terminal->terminal_window.top+105),Vector2f(185,185), 3, Color(0,0,0,0), Color(255,255,255,255)));
        terminal->captions.push_back(Caption("__" + ports_to_display[1] + "__", "font1", Vector2f(terminal->terminal_window.left+265, terminal->terminal_window.top+65), 25, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Current Charge: " + asString(terminal->ports[ports_to_display[1]]->getCell(3)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+220, terminal->terminal_window.top+105), 20, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Maximum Charge: " + asString(terminal->ports[ports_to_display[1]]->getCell(4)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+220, terminal->terminal_window.top+120), 20, Color(255,255,255,255), "left"));

        terminal->fixed_rectangles.push_back(createRectangle(Vector2f(terminal->terminal_window.left+105, terminal->terminal_window.top+305),Vector2f(185,185), 3, Color(0,0,0,0), Color(255,255,255,255)));
        terminal->captions.push_back(Caption("__" + ports_to_display[2] + "__", "font1", Vector2f(terminal->terminal_window.left+65, terminal->terminal_window.top+265), 25, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Current Charge: " + asString(terminal->ports[ports_to_display[2]]->getCell(3)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+20, terminal->terminal_window.top+305), 20, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Maximum Charge: " + asString(terminal->ports[ports_to_display[2]]->getCell(4)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+20, terminal->terminal_window.top+320), 20, Color(255,255,255,255), "left"));

        terminal->fixed_rectangles.push_back(createRectangle(Vector2f(terminal->terminal_window.left+305, terminal->terminal_window.top+305),Vector2f(185,185), 3, Color(0,0,0,0), Color(255,255,255,255)));
        terminal->captions.push_back(Caption("__" + ports_to_display[3] + "__", "font1", Vector2f(terminal->terminal_window.left+265, terminal->terminal_window.top+265), 25, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Current Charge: " + asString(terminal->ports[ports_to_display[3]]->getCell(3)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+220, terminal->terminal_window.top+305), 20, Color(255,255,255,255), "left"));
        terminal->captions.push_back(Caption("Maximum Charge: " + asString(terminal->ports[ports_to_display[3]]->getCell(4)->vars["value"]) + " mAh", "font1", Vector2f(terminal->terminal_window.left+220, terminal->terminal_window.top+320), 20, Color(255,255,255,255), "left"));
    }

}

Terminal::Terminal(string new_sector_id, string e_id, double x, double y){

    constructEntity(new_sector_id, e_id, x, y, "Terminal");

    voltage_rating = 12.0;
    wattage = 350;

    internal_cells["Buffered Power"] = make_shared<PowerCell>("Buffered Power",10);
    addPort("Neutral Rail", Vector2f(-10,-16), 0,"Power Port");
    ports["Neutral Rail"]->tapCell("Buffered Power");
    ports["Neutral Rail"]->voltage = 0.0;

    internal_cells["Number Input 1"] = make_shared<GPIOCell>("Number Input 1");
    internal_cells["Number Input 2"] = make_shared<GPIOCell>("Number Input 2");
    internal_cells["Number Input 3"] = make_shared<GPIOCell>("Number Input 3");
    internal_cells["Number Input 4"] = make_shared<GPIOCell>("Number Input 4");

    addPort("GPI 1", Vector2f(-30,-7), -90, "GPI Port");
    ports["GPI 1"]->tapCell("Number Input 1");
    addPort("GPI 2", Vector2f(-30,-22), -90, "GPI Port");
    ports["GPI 2"]->tapCell("Number Input 2");
    addPort("GPI 3", Vector2f(-18,-34), 0, "GPI Port");
    ports["GPI 3"]->tapCell("Number Input 3");
    addPort("GPI 4", Vector2f(1,-34), 0, "GPI Port");
    ports["GPI 4"]->tapCell("Number Input 4");

    setSlots(1); //slot that will hold chip

    takes_input = true;

}

//Terminal::Terminal(string new_sector_id, string e_id, double x, double y){
//
//    constructEntity(new_sector_id, e_id, x, y, "Terminal");
//
//    internal_cells["Buffered Power"] = make_shared<PowerCell>("Buffered Power", 5);
//    addPort("Leads", Vector2f(39,0), 0,"Power Port");
//    ports["Leads"]->tapCell("Buffered Power");
//
//    setSlots(1); //slot that will hold chip
//
//    internal_cells["Received Data"] = make_shared<DataCell>("Received Data");
//    addPort("Input", Vector2f(0,0), 0, "Data Input Port");
//    ports["Input"]->tapCell("Received Data");
//
//    internal_cells["Sent Data"] = make_shared<DataCell>("Sent Data");
//    addPort("Output", Vector2f(0,0), 0, "Data Output Port");
//    ports["Output"]->tapCell("Sent Data");
//}
//
bool Terminal::performFunction(double dt){

    double amps = wattage/voltage_rating;
    double milliamp_hours_needed = ((1000*amps/60)/60)*dt;

    if(internal_cells["Buffered Power"]->vars["milliamp hours"]<0.01){internal_cells["Buffered Power"]->vars["milliamp hours"]=0;}

    if(internal_cells["Buffered Power"]->vars["milliamp hours"] >= milliamp_hours_needed){

        internal_cells["Buffered Power"]->vars["milliamp hours"] -= milliamp_hours_needed;

        animator.play(17,79);

        double Q = getRotation()*(M_PI/180);
        Vector2f screen_position = getPosition() + Vector2f(16*cos(Q)-50*sin(Q),16*sin(Q)+50*cos(Q));
        castLight(sector_id, screen_position, 2, 120);
        fixed_sprites.clear();
        fixed_circles.clear();
        fixed_rectangles.clear();
        captions.clear();

        RectangleShape bounding_box = createRectangle(Vector2f(720,450), Vector2f(400,400), 5, Color(0,155,155,105), Color(0,155,155,205));
        fixed_rectangles.push_back(bounding_box);
        terminal_window = bounding_box.getGlobalBounds();

        //progression += dt*20;
        //if(progression >= 32){ progression = 0;}

        if(getSector(sector_id)->chips.count(contents[1]) == 0){

           /* if(sin(world.date.second*6) > 0){
                captions.push_back(Caption("- BOOT ERROR: No control chip present. -", "font1", Vector2f(535, 275), 24, Color(255,255,255,255), "left"));
            }

            //display 16 at a time
            int progress_index = floor(progression);
            vector<string> lines;
            //alert that no chip is inserted
            if(ports["GPIO A"]->connection == ""){
                for(int i = 1; i <= 8; i++){
                    lines.push_back("-A:" + asString(i) + " = NULL CONNECTION RETURN");
                }
            }
            else{
                for(int i = 1; i <= 8; i++){
                    lines.push_back("-A:" + asString(i) + " = " + asString(ports["GPIO A"]->getCell(i)->vars["value"]));
                }
            }

            if(ports["GPIO B"]->connection == ""){
                for(int i = 1; i <= 8; i++){
                    lines.push_back("-B:" + asString(i) + " = NULL CONNECTION RETURN");
                }
            }
            else{
                for(int i = 1; i <= 8; i++){
                    lines.push_back("-B:" + asString(i) + " = " + asString(ports["GPIO B"]->getCell(i)->vars["value"]));
                }
            }

            if(ports["GPIO C"]->connection == ""){
                for(int i = 1; i <= 8; i++){
                    lines.push_back("-C:" + asString(i) + " = NULL CONNECTION RETURN");
                }
            }
            else{
                for(int i = 1; i <= 8; i++){
                    lines.push_back("-C:" + asString(i) + " = " + asString(ports["GPIO C"]->getCell(i)->vars["value"]));
                }
            }

            if(ports["GPIO D"]->connection == ""){
                for(int i = 1; i <= 8; i++){
                    lines.push_back("-D:" + asString(i) + " = NULL CONNECTION RETURN");
                }
            }
            else{
                for(int i = 1; i <= 8; i++){
                    lines.push_back("-D:" + asString(i) + " = " + asString(ports["GPIO D"]->getCell(i)->vars["value"]));
                }
            }

            for(int i = 0; i < 16; i++){
                int index = progress_index + i;
                if(index > 31){ index -= 32; }

                captions.push_back(Caption(lines[index], "font1", Vector2f(535, 310 + (i*20)), 24, Color(255,255,255,255),"left"));
            }*/

            captions.push_back(Caption(asString(internal_cells["Number Input 1"]->vars["value"]), "font1", Vector2f(535, 310 + (0*20)), 24, Color(255,255,255,255),"left"));
            captions.push_back(Caption(asString(internal_cells["Number Input 2"]->vars["value"]), "font1", Vector2f(535, 310 + (1*20)), 24, Color(255,255,255,255),"left"));
            captions.push_back(Caption(asString(internal_cells["Number Input 3"]->vars["value"]), "font1", Vector2f(535, 310 + (2*20)), 24, Color(255,255,255,255),"left"));
            captions.push_back(Caption(asString(internal_cells["Number Input 4"]->vars["value"]), "font1", Vector2f(535, 310 + (3*20)), 24, Color(255,255,255,255),"left"));

            return true;
        }

        shared_ptr<Chip> chip = getSector(sector_id)->chips[contents[1]].lock();
        chip->storage_id = id;
        //evaluate user and machine input
        chip->process(dt);
        input.reset();
    }
    else{
        animator.play(0,0);

        fixed_sprites.clear();
        fixed_circles.clear();
        fixed_rectangles.clear();
        captions.clear();

        RectangleShape bounding_box;
        bounding_box.setSize(Vector2f(400,400));
        bounding_box.setFillColor(Color(100,100,100,105));
        bounding_box.setOutlineColor(Color(100,100,100,205));
        bounding_box.setOutlineThickness(5);
        bounding_box.setOrigin(200,200);
        bounding_box.setPosition(720,450);
        fixed_rectangles.push_back(bounding_box);
        terminal_window = bounding_box.getGlobalBounds();
    }

    return true;
}



string registerNewItem(string sector_id, string item_id, string type){

    string return_id;

    if(type == "Switch v1.0 (Chip)"){
        shared_ptr<SwitchChip> new_item = make_shared<SwitchChip>(sector_id, item_id);
        getSector(sector_id)->chips[new_item->id] = weak_ptr<Chip>(new_item);
        getSector(sector_id)->items[new_item->id] = shared_ptr<Item>(new_item);
        return_id = new_item->id;
    }
    else if(type == "Battery v1.0 (Chip)"){
        shared_ptr<BatteryChip> new_item = make_shared<BatteryChip>(sector_id, item_id);
        getSector(sector_id)->chips[new_item->id] = weak_ptr<Chip>(new_item);
        getSector(sector_id)->items[new_item->id] = shared_ptr<Item>(new_item);
        return_id = new_item->id;
    }
    else{
        shared_ptr<Item> new_item = make_shared<Item>(sector_id,item_id,type);
        getSector(sector_id)->items[new_item->id] = new_item;
        return_id = new_item->id;
    }

    return return_id;
}
