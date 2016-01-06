#include "main.h"

using namespace std;
using namespace sf;


extern World world;
extern RenderWindow window;

Sector::Sector(){}


Sector::Sector(string cid, double r, string tex, double ambient, bool plnt)
{
    id = cid;

    ambient_light = ambient;

    planetside = plnt;
    radius = r;
    backdrop_tx_path = tex;

    backdrop.setTextureRect(IntRect(0,0,2*radius,2*radius));
    backdrop.setOrigin(radius,radius);
    backdrop_tx.loadFromFile(backdrop_tx_path.c_str());
    backdrop_tx.setRepeated(true);
    backdrop.setTexture(backdrop_tx,false);

    collision_grid = CollisionGrid(id,160,Vector2f(-radius,-radius),Vector2f(radius,radius));
    floor_grid = CollisionGrid(id,500,Vector2f(-radius,-radius),Vector2f(radius,radius));
}

void Sector::process(double dt){


    for(map<string, shared_ptr<Entity> >::iterator i = ents.begin(); i != ents.end(); i++) //run per-loop operations on floor
    {
        i->second->floors.clear();
        i->second->floor_bonds.clear();
        i->second->bonded_to_floor = false;
    }
    for(map<string, shared_ptr<Floor> >::iterator i = floors.begin(); i != floors.end(); i++) //run per-loop operations on floor
    {
        i->second->flagBonded();
    }
    for(map<string, shared_ptr<Floor> >::iterator i = floors.begin(); i != floors.end(); i++) //run per-loop operations on floor
    {
        i->second->determinePassengers();
    }

    //WIPE COLLISION GRID SO THAT OBJECTS CAN RE-ADD THEMSELVES BASED ON NEW LOCATIONS
    collision_grid.reset();
    floor_grid.reset();
    //

    //REINSERT INTO GRID
    for(map<string, shared_ptr<Floor> >::iterator i = floors.begin(); i != floors.end(); i++) //run per-loop operations on floor
    {
        floor_grid.insertObject(i->second->id);
    }
    for(map<string, shared_ptr<Entity> >::iterator i = ents.begin(); i != ents.end(); i++) //run per-loop operations on object
    {
        collision_grid.insertObject(i->second->id);
    }
    //

    //RUN OBJECT PROCESSES FOR THIS TICK + DETERMINE NEW LOCATIONS

    for(map<string, shared_ptr<Floor> >::iterator i = floors.begin(); i != floors.end(); i++) //run per-loop operations on floor
    {
        shared_ptr<Floor> floor = i->second;
        i->second->run(dt);


        if(planetside){

            Vector2f relative_velocity = floor->getVelocity()*-1;
            double surface_area = 1;
            floor->applyImpulse(relative_velocity.x*floor->getMyMass()*surface_area*20*dt,relative_velocity.y*floor->getMyMass()*surface_area*20*dt,floor->getPosition(),false,false);

            double relative_spin = floor->getRotationalVelocity()*-1;
            floor->applyRotationalImpulse(relative_spin*floor->getMyInertia()*surface_area*20*dt);
        }
    }
   for(map<string, weak_ptr<Machine> >::iterator i = machines.begin(); i != machines.end(); i++) //run per-loop operations on object
    {
        i->second.lock()->performFunction(dt);
    }
    for(map<string, shared_ptr<Entity> >::iterator i = ents.begin(); i != ents.end(); i++) //run per-loop operations on object
    {


        i->second->run(dt);
        shared_ptr<Entity> entity = i->second;
        if(planetside && entity->floors.size()==0){

            if(people.count(i->first)!=0 && people[i->first].lock()->jetpack_on){continue;}

            Vector2f relative_velocity = entity->getVelocity()*-1;
            double surface_area = M_PI*(convertToMeters(entity->collision_radius)*convertToMeters(entity->collision_radius));
            entity->applyImpulse(relative_velocity.x*entity->getMyMass()*surface_area*20*dt,relative_velocity.y*entity->getMyMass()*surface_area*20*dt,entity->getPosition(),false,false);

            double relative_spin = entity->getRotationalVelocity()*-1;
            entity->applyRotationalImpulse(relative_spin*entity->getMyInertia()*surface_area*20*dt);
        }
    }
    for(map<string, shared_ptr<Floor> >::iterator i = floors.begin(); i != floors.end(); i++) //run per-loop operations on floor
    {
        i->second->applyGrip(dt);
    }
    for(map<string, shared_ptr<Poster> >::iterator i = posters.begin(); i != posters.end(); i++) //run per-loop operations on object
    {
        i->second->run(dt);
    }
    for(map<string, shared_ptr<Interface> >::iterator i = interfaces.begin(); i != interfaces.end(); i++)
    {
        i->second->run(dt);
    }
    for(map<string, shared_ptr<Rope> >::iterator i = ropes.begin(); i != ropes.end(); i++)
    {
        i->second->run(dt);
    }
//    for(map<string, shared_ptr<Cloud> >::iterator parser = clouds.begin(); parser!=clouds.end(); parser++){
//
//        bool too_far_out = true;
//        for(map<string, shared_ptr<Entity> >::iterator i = ents.begin(); i != ents.end(); i++)
//        {
//            if(i->second->properties->classification=="ITEM"){continue;}
//            if(parser->second->inRange(i->second->getPosition(),500)){too_far_out = false;}
//            if(parser->second->inRange(i->second->getPosition(),i->second->collision_radius-5))
//            {
//                Vector2f dist = Vector2f((parser->second->coords.x - i->second->getPosition().x),(parser->second->coords.y - i->second->getPosition().y));
//                double h = hypot(dist.x,dist.y);
//                double seperation_dist = h - (i->second->collision_radius+parser->second->radius);
//
//                if(h != 0){
//                    Vector2f unit_vector = Vector2f(dist.x/h,dist.y/h);
//
//                    if(seperation_dist<0){parser->second->coords = parser->second->coords - unit_vector*seperation_dist;}
//
//                    double vector_p = hypot(parser->second->p.x,parser->second->p.y);
//                    parser->second->p = Vector2f(unit_vector.x*vector_p,unit_vector.y*vector_p);
//                    i->second->applyImpulse(-unit_vector.x*vector_p/1000,-unit_vector.y*vector_p/1000,i->second->getPosition(),false);
//                }
//            }
//        }
//
//        double my_pressure = getPressure(parser->second->coords,20);
//        Vector2f east_dif = Vector2f((getPressure(parser->second->coords+Vector2f(100,0),100) - my_pressure),0);
//        Vector2f west_dif = Vector2f(-(getPressure(parser->second->coords+Vector2f(-100,0),100) - my_pressure),0);
//        Vector2f south_dif = Vector2f(0,(getPressure(parser->second->coords+Vector2f(0,100),100) - my_pressure));
//        Vector2f north_dif = Vector2f(0,-(getPressure(parser->second->coords+Vector2f(0,-100),100) - my_pressure));
//
//        Vector2f net_vector = east_dif + west_dif + south_dif + north_dif;
//        Vector2f impulse = net_vector*(-50*(parser->second->properties->get("molar_mass")*parser->second->kilomoles)*dt);
//        parser->second->applyImpulse(impulse.x,impulse.y);
//
//        if(too_far_out){
//           trash(parser->first);
//        }
//
//    }
//    for(map<string, shared_ptr<Cloud> >::iterator i = clouds.begin(); i != clouds.end(); i++) //run per-loop operations on object
//    {
//        i->second->run(dt);
//    }

    for(map<string, shared_ptr<Bond> >::iterator i = bonds.begin(); i != bonds.end(); i++)
    {
        i->second->applyBond(dt);
    }
    for(map<string, shared_ptr<Connector> >::iterator i = connectors.begin(); i != connectors.end(); i++) //run per-loop operations on object
    {
        i->second->updateSprite();
    }
    for(map<string,shared_ptr<Effect> >::iterator i = effects.begin(); i != effects.end(); i++)
    {
        i->second->run(dt);
    }


    //COLLISION DETECTION AND RESPONSE

    collision_grid.processCollisions(dt);
    floor_grid.processCollisions(dt);

    for(map<string,shared_ptr<Field> >::iterator i = fields.begin(); i != fields.end(); i++){
        if(i->second->grid_cells.size() == 0){
            collision_grid.insertField(i->first);
        }
    }
    collision_grid.processFields();
    //

    //HANDLE REMOVAL OF OBJECTS SCHEDULED FOR DELETION
    emptyGarbageBin();

    //

    
    if(connectors.size() != last_connectors.size()){
        resortAllObjectsIntoBonds();
        last_connectors = connectors;
    }

   //DRAW here


}

void Sector::drawBackdrop(){

    window.draw(backdrop);
}

void Sector::draw(){

    for(map<string, shared_ptr<Floor> >::iterator i = floors.begin(); i != floors.end(); i++) //run per-loop operations on floor
    {
        i->second->draw();
    }
    for(map<string, shared_ptr<Field> >::iterator i = fields.begin(); i != fields.end(); i++) //run per-loop operations on floor
    {
        i->second->draw();
    }
    for(vector<Sprite>::iterator i = lights.begin(); i != lights.end(); i++){

        window.draw(*i);
    }
    lights.clear();
    for(map<string, shared_ptr<Connector> >::iterator i = connectors.begin(); i != connectors.end(); i++) //run per-loop operations on object
    {
        if(i->second->classification == "MOUNTER"){
            i->second->draw();
        }
    }

    set<string> draw_again;
    for(map<string, shared_ptr<Entity> >::iterator i = ents.begin(); i != ents.end(); i++) //run per-loop operations on object
    {
        i->second->draw();
        if(i->second->double_draw){draw_again.insert(i->first);}
    }
    for(set<string>::iterator i = draw_again.begin(); i != draw_again.end(); i++){
        ents[*i]->draw();
    }
    for(map<string, shared_ptr<Connector> >::iterator i = connectors.begin(); i != connectors.end(); i++) //run per-loop operations on object
    {
        if(i->second->classification == "CONNECTOR"){
            i->second->draw();
        }
    }
    for(map<string, shared_ptr<Poster> >::iterator i = posters.begin(); i != posters.end(); i++) //run per-loop operations on object
    {
        i->second->draw();
    }
    for(map<string, shared_ptr<Interface> >::iterator i = interfaces.begin(); i != interfaces.end(); i++)
    {
        i->second->draw();
    }
    for(map<string, shared_ptr<Rope> >::iterator i = ropes.begin(); i != ropes.end(); i++)
    {
        i->second->draw();
    }

   // for(map<string, shared_ptr<Cloud> >::iterator i = clouds.begin();i!=clouds.end(); i++){
    //    i->second->draw();
   // }
    for(map<string,shared_ptr<Effect> >::iterator i = effects.begin(); i != effects.end(); i++)
    {
        i->second->draw();
    }
}

//double Sector::getPressure(Vector2f coords, double radius){
//
//    double kilograms = 0;
//    double area = (2*M_PI*radius)/80;
//
//    for(map<string,shared_ptr<Cloud> >::iterator parser = clouds.begin(); parser!= clouds.end(); parser++){
//        if(parser->second->inRange(coords, radius)){
//
//            kilograms += (parser->second->kilomoles*parser->second->properties->get("molar_mass"));
//        }
//    }
//
//    return kilograms/area; //kilograms per 80 pixels^2 (m^2)
//}

//
//map<string, double> Sector::liftKilograms(Vector2f coords, double radius, double kg){
//
//
//    map<string, double> return_quantities;
//
//    set<string> delete_list;
//
//    map<string,double> composition; //second part is mass at first, then later is converted to kg to take
//    double total_mass = 0;
//
//    for(map<string, shared_ptr<Cloud> >::iterator parser = clouds.begin(); parser!=clouds.end(); parser++){
//        if(parser->second->inRange(coords, radius)){
//
//            if(composition.count(parser->second->properties->name)==0){
//                composition[parser->second->properties->name] = (parser->second->kilomoles*parser->second->properties->get("molar_mass"));
//            }
//            else{
//                composition[parser->second->properties->name] += (parser->second->kilomoles*parser->second->properties->get("molar_mass"));
//            }
//            total_mass += (parser->second->kilomoles*parser->second->properties->get("molar_mass"));
//
//        }
//    }
//
//    for(map<string,double>::iterator parser = composition.begin(); parser != composition.end(); parser++){
//
//        parser->second = kg*(parser->second/total_mass);
//    }
//
//    for(map<string, shared_ptr<Cloud> >::iterator parser = clouds.begin(); parser!=clouds.end(); parser++){
//
//        if(parser->second->inRange(coords,radius)){
//
//            if(return_quantities.count(parser->second->properties->name)==0){
//                return_quantities[parser->second->properties->name] = 0;
//            }
//
//            double kg_needed = composition[parser->second->properties->name] - return_quantities[parser->second->properties->name]*parser->second->properties->get("molar_mass");
//            if(kg_needed > 0){
//
//                if(kg_needed > parser->second->kilomoles*parser->second->properties->get("molar_mass")){
//                    kg_needed = parser->second->kilomoles*parser->second->properties->get("molar_mass");
//                }
//
//                parser->second->kilomoles -= kg_needed/parser->second->properties->get("molar_mass");
//                return_quantities[parser->second->properties->name] += kg_needed/parser->second->properties->get("molar_mass");
//
//                if(parser->second->kilomoles<1){
//                    trash(parser->first);
//                }
//            }
//        }
//    }
//
//    return return_quantities;
//
//}



string Sector::getClassification(string lookup_id){

    if(ents.count(lookup_id) != 0 || floors.count(lookup_id) != 0){
        if(getEnt(lookup_id)->properties->name=="Warp Drive"){
            return "WARP DRIVE";
        }
        return getEnt(lookup_id)->properties->classification;
    }
    if(connectors.count(lookup_id) != 0){
        return "CONNECTOR";
    }
    if(posters.count(lookup_id) != 0){
        return "POSTER";
    }
    if(interfaces.count(lookup_id) != 0){
        return "INTERFACE";
    }
    if(items.count(lookup_id) != 0){
        return items[lookup_id]->properties->classification;
    }
}

shared_ptr<Entity> Sector::getEnt(string ent_id){

    shared_ptr<Entity> blank;

    if(ents.count(ent_id)!=0){
        return ents.find(ent_id)->second;
    }
    else if(floors.count(ent_id)!=0){
        return shared_ptr<Entity>(floors.find(ent_id)->second);
    }

    return blank;
}


shared_ptr<Machine> Sector::getMachine(string ent_id){

    shared_ptr<Machine> blank;

    if(machines.count(ent_id)!=0){
        return machines[ent_id].lock();
    }
    return blank;
}


void Sector::trash(string g_id){

    garbage_bin.insert(g_id);
}

void Sector::emptyGarbageBin(){

    for(set<string>::iterator parser = garbage_bin.begin(); parser != garbage_bin.end(); parser++)
    {
        collision_grid.removeObject(*parser);
        floor_grid.removeObject(*parser);

        if(interfaces.count(*parser)!=0){
            interfaces[*parser]->disconnect();
            interfaces.erase(*parser);
        }
        if(machines.count(*parser)!=0){
            machines[*parser].lock()->disconnect();
            machines.erase(*parser);
        }
        if(terminals.count(*parser)!=0){
            terminals.erase(*parser);
        }
        if(ents.count(*parser)!=0){
            ents[*parser]->cleanup();
            //ents[*parser]->breakdown();
            ents.erase(*parser);
        }
        if(people.count(*parser)!=0){
            people.erase(*parser);
        }
        if(floors.count(*parser)!=0){
            floors[*parser]->cleanup();
            floors.erase(*parser);
        }
        if(connectors.count(*parser)!=0){
            connectors[*parser]->cleanup();
            connectors.erase(*parser);
        }
        if(bonds.count(*parser)!=0){
            bonds[*parser]->cleanup();
            bonds.erase(*parser);
        }
        if(effects.count(*parser)!=0){
            effects.erase(*parser);
        }
        if(items.count(*parser)!=0){
            items.erase(*parser);
        }
        if(fields.count(*parser) != 0){
            fields.erase(*parser);
        }
      //  if(scrap.count(*parser)!=0){
      //      scrap.erase(*parser);
     //   }
        if(ropes.count(*parser)!=0){
            ropes.erase(*parser);
        }
      //  if(clouds.count(garbage_bin[i])!=0){
         //   clouds.erase(garbage_bin[i]);
        //}

    }
    garbage_bin.clear();
}

void Sector::trashEmptyBonds(){

    for(map<string,shared_ptr<Bond> >::iterator parser = bonds.begin(); parser!= bonds.end(); parser++){
        if(parser->second->members.size()<2){
           trash(parser->first);
        }
    }
}

void Sector::resortIntoBonds(set<string> ent_ids)
{

    set<string> checked_ents;
    vector<set<string> > bond_groups;

    while(true)
    {

        bool none_left = true;
        string ent_id_to_check;
        for(set<string>::iterator parser = ent_ids.begin(); parser!=ent_ids.end(); parser++){

            if(checked_ents.count(*parser)==0){

                ent_id_to_check = *parser;
                none_left=false;
                break;
            }
        }

        if(none_left){break;}

        checked_ents.insert(ent_id_to_check);
        if(shared_ptr<Entity> ent = getEnt(ent_id_to_check)){

            set<string> new_grouping;
            new_grouping.insert(ent_id_to_check);
            ent->getConnected(&new_grouping);
            bond_groups.push_back(new_grouping);

            for(set<string>::iterator parser = new_grouping.begin(); parser!=new_grouping.end(); parser++){

                checked_ents.insert(*parser);
            }
        }
    }

    for(set<string>::iterator parser = checked_ents.begin(); parser!=checked_ents.end(); parser++){

        if(shared_ptr<Entity> e = getEnt(*parser)){

            if(e->bonded){
                string bond_id = e->bond;
                bonds[bond_id]->removeMember(*parser);
            }
        }
    }

    for(vector<set<string> >::iterator parser = bond_groups.begin(); parser!=bond_groups.end(); parser++)
    {
        set<string> current_group = *parser;
        if(current_group.size()<2){continue;}

        makeNewBond(id,current_group);
    }

    trashEmptyBonds();
}


void Sector::resortAllObjectsIntoBonds()
{
    set<string> resort_list;

    for(map<string,shared_ptr<Entity> >::iterator parser = ents.begin(); parser!=ents.end(); parser++){

        resort_list.insert(parser->first);
    }
    for(map<string,shared_ptr<Floor> >::iterator parser = floors.begin(); parser!=floors.end(); parser++){

        resort_list.insert(parser->first);
    }
    resortIntoBonds(resort_list);
}

void Sector::boom(Vector2f coords, double energy, double scaling_per_meter){

    createEffect(id, coords,Vector2f(0,0),"charged_smoke");

    double max_dist = (1/scaling_per_meter)*80;

    for(map<string, shared_ptr<Entity> >::iterator parser = ents.begin(); parser != ents.end(); parser++){

        if(parser->first=="Avalanche"){continue;}

        Vector2f dist = parser->second->getPosition() - coords;
        double h = hypot(dist.x,dist.y);

        if(h<max_dist and collision_grid.lineOfSight(parser->first, coords)){

            double energy_at_range = energy - (energy*(scaling_per_meter*(h/80)));
            if(h!=0 and energy_at_range<1){continue;}
            Vector2f unit_vector = dist/h;

            if(parser->second->properties->classification=="ITEM"){
                energy_at_range *= 0.005;//empirically determined. theoretically, accounts for the much smaller surface area of a piece of scrap, which makes it much less likely to be struck by the debris of an explosion.
                double neg = 1;
                if(rand() % 100<50){neg=-1;}
               // parser->second->applyRotationalImpulse(neg);//makes scrap spin when explosions happen, to give a "chaotic" feeling
            }
            parser->second->applyImpulse(unit_vector.x * energy_at_range, unit_vector.y * energy_at_range,parser->second->getPosition(), true ,false);
        }

    }
    for(map<string, shared_ptr<Floor> >::iterator parser = floors.begin(); parser != floors.end(); parser++){

        Vector2f dist = parser->second->getPosition() - coords;
        double h = hypot(dist.x,dist.y);

        if(h<max_dist){

            double energy_at_range = energy - (energy*(scaling_per_meter*(h/80)));
            if(h!=0 and energy_at_range<1){continue;}
            Vector2f unit_vector = dist/h;

            parser->second->applyImpulse(unit_vector.x * energy_at_range, unit_vector.y * energy_at_range, parser->second->getPosition(), true,false);
        }

    }

}

shared_ptr<Sector> getSector(string which_sector){

    return world.sectors[which_sector];
}

Change::Change(){}

Change::Change(string c_id, Date c_min, Date c_relative){

    id = c_id;
    min_date = c_min;
    relative_date = c_relative;
}

bool Change::trigger(){

    bool can_trigger = true;
    for(map<string, bool>::iterator parser = prereq_events.begin(); parser != prereq_events.end(); parser++){
        can_trigger = parser->second;
        if(!can_trigger){ break; }
    }

    if(world.date < min_date && can_trigger == true){ can_trigger = false; }
    if(world.date < calculated_date && can_trigger == true){ can_trigger = false; }

    if(can_trigger){
        //trigger the change

        TiXmlDocument doc("changes/"+id+".xml");
        bool doc_valid = doc.LoadFile();

        if(not doc_valid){return false;}

        for(TiXmlElement* change_event = doc.FirstChildElement(); change_event != NULL; change_event = change_event->NextSiblingElement()){
            string event_type = change_event->ValueStr();

            if(event_type == "AddEntity")
            {
                makeNewEntity(change_event->Attribute("sector_id"), change_event->Attribute("id"), strtod(change_event->FirstChildElement("x")->GetText(),NULL), strtod(change_event->FirstChildElement("y")->GetText(),NULL),strtod(change_event->FirstChildElement("rotation")->GetText(),NULL),change_event->FirstChildElement("type")->GetText());
                getSector(change_event->Attribute("sector_id"))->ents[
                change_event->Attribute("id")]->setRotationalVelocity(strtod(change_event->FirstChildElement("rotational_velocity")->GetText(),NULL));
                getSector(change_event->Attribute("sector_id"))->ents[
                change_event->Attribute("id")]->setVelocity(strtod(
                                                                   change_event->FirstChildElement("x_velocity")->GetText(),NULL),strtod(change_event->FirstChildElement("y_velocity")->GetText(),NULL));
                if(string(
                          change_event->FirstChildElement("slaved")->GetText()) == "true"){
                    getSector(change_event->Attribute("sector_id"))->ents[change_event->Attribute("id")]->slaved = true;
                }
                int i = 1;
                for(TiXmlElement* sub_element =
                    change_event->FirstChildElement("slot"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("slot")){
                    getSector(change_event->Attribute("sector_id"))->ents[change_event->Attribute("id")]->contents[i] = sub_element->GetText();
                    i++;
                }
            }
            if(event_type == "AddMachine")
            {
                makeNewMachine(change_event->Attribute("sector_id"),
                               change_event->Attribute("id"), strtod(change_event->FirstChildElement("x")->GetText(),NULL), strtod(
                                                                                                                                   change_event->FirstChildElement("y")->GetText(),NULL),strtod(change_event->FirstChildElement("rotation")->GetText(),NULL),change_event->FirstChildElement("type")->GetText(), true);
                getSector(change_event->Attribute("sector_id"))->ents[
                change_event->Attribute("id")]->setRotationalVelocity(strtod(change_event->FirstChildElement("rotational_velocity")->GetText(),NULL));
                getSector(change_event->Attribute("sector_id"))->ents[
                change_event->Attribute("id")]->setVelocity(strtod(change_event->FirstChildElement("x_velocity")->GetText(),NULL),strtod(
                                                                                                                                         change_event->FirstChildElement("y_velocity")->GetText(),NULL));
                if(string(
                          change_event->FirstChildElement("slaved")->GetText()) == "true"){
                    getSector(change_event->Attribute("sector_id"))->ents[change_event->Attribute("id")]->slaved = true;
                }
                int i = 1;
                for(TiXmlElement* sub_element = change_event->FirstChildElement("slot"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("slot")){
                    getSector(change_event->Attribute("sector_id"))->ents[change_event->Attribute("id")]->contents[i] = sub_element->GetText();
                    i++;
                }
                for(TiXmlElement* sub_element = change_event->FirstChildElement("cell_var"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("cell_var"))
                {
                    getSector(change_event->Attribute("sector_id"))->getMachine(change_event->Attribute("id"))->internal_cells[sub_element->Attribute("cell_id")]->vars[sub_element->Attribute("var_id")] = strtod(sub_element->GetText(),NULL);
                }
                for(TiXmlElement* sub_element = change_event->FirstChildElement("cell_data"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("cell_data"))
                {
                    if(sub_element->GetText() == NULL){
                        getSector(change_event->Attribute("sector_id"))->getMachine(change_event->Attribute("id"))->internal_cells[sub_element->Attribute("cell_id")]->data[sub_element->Attribute("data_id")] = "";
                    }
                    else{
                        getSector(change_event->Attribute("sector_id"))->getMachine(change_event->Attribute("id"))->internal_cells[sub_element->Attribute("cell_id")]->data[sub_element->Attribute("data_id")] = sub_element->GetText();
                    }
                }
                for(TiXmlElement* sub_element = change_event->FirstChildElement("port_connection"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("port_connection"))
                {
                    getSector(change_event->Attribute("sector_id"))->getMachine(change_event->Attribute("id"))->ports[sub_element->Attribute("port_id")]->connection = sub_element->GetText();
                }

            }
            if(event_type == "AddPoster")
            {
                makeNewPoster(change_event->Attribute("sector_id"), change_event->Attribute("id"), change_event->FirstChildElement("type")->GetText(), change_event->FirstChildElement("parent_id")->GetText(), Vector2f(strtod(
                                                                                                                                                                                                                                change_event->FirstChildElement("relative_x")->GetText(),NULL),strtod(change_event->FirstChildElement("relative_y")->GetText(),NULL)));
            }
            if(event_type == "AddInterface")
            {
                makeNewInterface(change_event->Attribute("sector_id"), change_event->Attribute("id"),
                                 change_event->FirstChildElement("port_id")->GetText(), change_event->FirstChildElement("machine_id")->GetText(), Vector2f(strtod(change_event->FirstChildElement("relative_x")->GetText(),NULL), strtod(change_event->FirstChildElement("relative_y")->GetText(),NULL)),change_event->FirstChildElement("type")->GetText());
                for(TiXmlElement* sub_element = change_event->FirstChildElement("var_num"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("var_num"))
                {
                    getSector(change_event->Attribute("sector_id"))->interfaces[change_event->Attribute("id")]->var_num[sub_element->Attribute("id")] = strtod(sub_element->GetText(),NULL);
                }
            }
            if(event_type == "AddTransfer")
            {
                makeNewTransfer(change_event->Attribute("sector_id"), change_event->Attribute("id"), change_event->FirstChildElement("port_id_a")->GetText(), change_event->FirstChildElement("port_id_b")->GetText(), change_event->FirstChildElement("machine_id_a")->GetText(), change_event->FirstChildElement("machine_id_b")->GetText(),change_event->FirstChildElement("type")->GetText());
                for(TiXmlElement* sub_element = change_event->FirstChildElement("var_num"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("var_num"))
                {
                    getSector(change_event->Attribute("sector_id"))->interfaces[change_event->Attribute("id")]->var_num[sub_element->Attribute("id")] = strtod(sub_element->GetText(),NULL);
                }
            }
            if(event_type == "AddFloor")
            {
                makeNewFloor(change_event->Attribute("sector_id"), change_event->Attribute("id"), strtod(change_event->FirstChildElement("x")->GetText(),NULL), strtod(change_event->FirstChildElement("y")->GetText(),NULL), strtod(change_event->FirstChildElement("x_tiles")->GetText(),NULL), strtod(change_event->FirstChildElement("y_tiles")->GetText(),NULL),strtod(change_event->FirstChildElement("rotation")->GetText(),NULL),
                             change_event->FirstChildElement("type")->GetText());
                getSector(change_event->Attribute("sector_id"))->floors[change_event->Attribute("id")]->setRotationalVelocity(strtod(change_event->FirstChildElement("rotational_velocity")->GetText(),NULL));
                getSector(change_event->Attribute("sector_id"))->floors[change_event->Attribute("id")]->setVelocity(strtod(change_event->FirstChildElement("x_velocity")->GetText(),NULL),strtod(change_event->FirstChildElement("y_velocity")->GetText(),NULL));
            }
            if(event_type == "AddConnector")
            {
                makeNewConnector(change_event->Attribute("sector_id"), change_event->Attribute("id"),
                                 change_event->FirstChildElement("type")->GetText(), change_event->FirstChildElement("A")->GetText(),
                                 change_event->FirstChildElement("B")->GetText());
                getSector(change_event->Attribute("sector_id"))->getEnt(change_event->FirstChildElement("A")->GetText())->connectors.insert(change_event->Attribute("id"));
                getSector(change_event->Attribute("sector_id"))->getEnt(change_event->FirstChildElement("B")->GetText())->connectors.insert(change_event->Attribute("id"));
            }
            if(event_type == "AddItem")
            {
                registerNewItem(change_event->Attribute("sector_id"), change_event->Attribute("id"), change_event->FirstChildElement("type")->GetText());
                //getSector(change_event->Attribute("sector_id"))->items[change_event->Attribute("id")]->storage_id = change_event->FirstChildElement("storage_id")->GetText();
            }
            if(event_type == "AddRope")
            {
                getSector(change_event->Attribute("sector_id"))->ropes[change_event->Attribute("id")] = make_shared<Rope>(change_event->Attribute("sector_id"), change_event->Attribute("id"), change_event->FirstChildElement("type")->GetText());
                for(TiXmlElement* sub_element = change_event->FirstChildElement("node"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("node")){
                    string attached_id;
                    if(sub_element->FirstChildElement("attached")->GetText()==NULL){
                        attached_id = "";
                    }
                    else{
                        attached_id = sub_element->FirstChildElement("attached")->GetText();
                    }
                    getSector(change_event->Attribute("sector_id"))->ropes[change_event->Attribute("id")]->addExplicitNode(Vector2f(strtod(sub_element->FirstChildElement("x")->GetText(),NULL),strtod(sub_element->FirstChildElement("y")->GetText(),NULL)),Vector2f(strtod(sub_element->FirstChildElement("x_velocity")->GetText(),NULL),strtod(sub_element->FirstChildElement("y_velocity")->GetText(),NULL)),attached_id, Vector2f(strtod(sub_element->FirstChildElement("attachment_x")->GetText(),NULL),strtod(sub_element->FirstChildElement("attachment_y")->GetText(),NULL)));

                }
            }
        }

        triggered = true;
    }

    return can_trigger;
}

void Change::updateFlags(){

    for(map<string, bool>::iterator parser = prereq_events.begin(); parser != prereq_events.end(); parser++){
        if(!parser->second){
            if(world.enacted_changes.count(parser->first) != 0){
                parser->second = true;
                calculated_date = world.enacted_changes[parser->first] + relative_date;
            }
        }
    }
}

World::World(){}

void World::transfer(string origin_sector_id, string transfer_ent_id, string target_sector_id, Vector2f target_sector_spawn){

    transfers[transfer_ent_id] = make_pair(origin_sector_id, make_pair(target_sector_id, target_sector_spawn));

}

void World::getRestingBonded(string ent_id, map<string,Vector2f> &ents_to_move, string origin_sector_id, string transfer_ent_id, string target_sector_id, Vector2f target_sector_spawn){

    ents_to_move[ent_id] = sectors[origin_sector_id]->getEnt(ent_id)->getPosition() - sectors[origin_sector_id]->getEnt(transfer_ent_id)->getPosition();
    if(sectors[origin_sector_id]->getEnt(ent_id)->bonded){
        for(set<string>::iterator parser = sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->getEnt(ent_id)->bond]->members.begin(); parser != sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->getEnt(ent_id)->bond]->members.end(); parser++){
            ents_to_move[*parser] = sectors[origin_sector_id]->getEnt(*parser)->getPosition() - sectors[origin_sector_id]->getEnt(transfer_ent_id)->getPosition();
            if(sectors[origin_sector_id]->floors.count(*parser) != 0){
                for(set<string>::iterator parser_2 = sectors[origin_sector_id]->floors[*parser]->passengers.begin(); parser_2 != sectors[origin_sector_id]->floors[*parser]->passengers.end(); parser_2++){
                    getRestingBonded(*parser_2, ents_to_move, origin_sector_id, transfer_ent_id, target_sector_id, target_sector_spawn);
                }
            }
        }
    }
}

void World::processTransfers(){

    for(map<string,pair<string,pair<string, Vector2f> > >::iterator a = transfers.begin(); a != transfers.end(); a++){
        string origin_sector_id = a->second.first;
        string transfer_ent_id = a->first;
        string target_sector_id = a->second.second.first;
        Vector2f target_sector_spawn = a->second.second.second;
        Vector2f origin_position = sectors[origin_sector_id]->getEnt(transfer_ent_id)->getPosition();

        if(sectors[origin_sector_id]->ents.count(transfer_ent_id) != 0){

            if(sectors[origin_sector_id]->ents[transfer_ent_id]->bonded){

                if(!sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->ents[transfer_ent_id]->bond]->moveable){return;}//refuses transfer if bond contains things bound to sector

                map<string,Vector2f> ents_to_move;

                Vector2f new_velocity =  sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->ents[transfer_ent_id]->bond]->p/sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->ents[transfer_ent_id]->bond]->mass;
                double new_rot_velocity = sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->ents[transfer_ent_id]->bond]->rot_p/sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->ents[transfer_ent_id]->bond]->getInertia();
                for(set<string>::iterator parser = sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->ents[transfer_ent_id]->bond]->members.begin(); parser != sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->ents[transfer_ent_id]->bond]->members.end(); parser++){
                    //for every member in the bond
                    ents_to_move[*parser] = sectors[origin_sector_id]->getEnt(*parser)->getPosition() - sectors[origin_sector_id]->getEnt(transfer_ent_id)->getPosition();
                    if(sectors[origin_sector_id]->floors.count(*parser)!=0){
                        //for everything resting loose on a floor in the bond

                        for(set<string>::iterator parser_2 = sectors[origin_sector_id]->floors[*parser]->passengers.begin(); parser_2 != sectors[origin_sector_id]->floors[*parser]->passengers.end(); parser_2++){

                            getRestingBonded(*parser_2, ents_to_move, origin_sector_id, transfer_ent_id, target_sector_id, target_sector_spawn);
                        }
                    }
                }
                for(map<string,Vector2f>::iterator parser = ents_to_move.begin(); parser != ents_to_move.end(); parser++){
                    for(set<string>::iterator parser_2 = sectors[origin_sector_id]->getEnt(parser->first)->connectors.begin(); parser_2 != sectors[origin_sector_id]->getEnt(parser->first)->connectors.end(); parser_2++){
                        if(sectors[target_sector_id]->connectors.count(*parser_2)==0){
                            sectors[target_sector_id]->connectors[*parser_2] = sectors[origin_sector_id]->connectors[*parser_2];
                            sectors[origin_sector_id]->connectors[*parser_2]->sector_id = target_sector_id;
                            sectors[origin_sector_id]->connectors.erase(*parser_2);
                        }
                    }
                    for(map<int,string>::iterator parser_2 = sectors[origin_sector_id]->getEnt(parser->first)->contents.begin(); parser_2 != sectors[origin_sector_id]->getEnt(parser->first)->contents.end(); parser_2++){
                        if(parser_2->second == ""){continue;}
                        sectors[target_sector_id]->items[parser_2->second] = sectors[origin_sector_id]->items[parser_2->second];
                        sectors[origin_sector_id]->items[parser_2->second]->sector_id = target_sector_id;
                        sectors[origin_sector_id]->items.erase(parser_2->second);
                    }
                    sectors[origin_sector_id]->getEnt(parser->first)->bonded = false;
                    sectors[origin_sector_id]->getEnt(parser->first)->bond = "";
                    sectors[origin_sector_id]->getEnt(parser->first)->setVelocity(new_velocity.x,new_velocity.y);
                    sectors[origin_sector_id]->getEnt(parser->first)->setRotationalVelocity(new_rot_velocity);
                    if(sectors[origin_sector_id]->ents.count(parser->first) != 0){
                        sectors[target_sector_id]->ents[parser->first] = sectors[origin_sector_id]->ents[parser->first];
                        sectors[origin_sector_id]->ents.erase(parser->first);
                        sectors[target_sector_id]->ents[parser->first]->sector_id = target_sector_id;
                        sectors[target_sector_id]->ents[parser->first]->setPosition(target_sector_spawn.x + parser->second.x,target_sector_spawn.y + parser->second.y);

                        if(sectors[origin_sector_id]->machines.count(parser->first) != 0){
                            sectors[target_sector_id]->machines[parser->first] = sectors[origin_sector_id]->machines[parser->first];
                            for(map<string, shared_ptr<FlowPort> >::iterator parser_3 = sectors[origin_sector_id]->machines[parser->first].lock()->ports.begin(); parser_3 != sectors[origin_sector_id]->machines[parser->first].lock()->ports.end(); parser_3++){
                                parser_3->second->sector_id = target_sector_id;
                                if(parser_3->second->connection!=""){
                                    if(sectors[origin_sector_id]->ropes.count(parser_3->second->connection)!=0){
                                        if(ents_to_move.count(sectors[origin_sector_id]->interfaces[parser_3->second->connection]->machine_id_2)!=0 && ents_to_move.count(sectors[origin_sector_id]->interfaces[parser_3->second->connection]->machine_id)!=0){
                                            //copy rope and interface
                                            if(sectors[target_sector_id]->interfaces.count(parser_3->second->connection)==0){
                                                sectors[target_sector_id]->interfaces[parser_3->second->connection] = sectors[origin_sector_id]->interfaces[parser_3->second->connection];
                                                sectors[origin_sector_id]->interfaces[parser_3->second->connection]->sector_id = target_sector_id;
                                                sectors[origin_sector_id]->interfaces.erase(parser_3->second->connection);
                                            }
                                            sectors[target_sector_id]->ropes[parser_3->second->connection] = sectors[origin_sector_id]->ropes[parser_3->second->connection];
                                            sectors[origin_sector_id]->ropes[parser_3->second->connection]->sector_id = target_sector_id;
                                            for(vector<shared_ptr<RopeNode> >::iterator nodes = sectors[origin_sector_id]->ropes[parser_3->second->connection]->nodes.begin(); nodes != sectors[origin_sector_id]->ropes[parser_3->second->connection]->nodes.end(); nodes++){
                                                shared_ptr<RopeNode> temp = *nodes;
                                                temp->sector_id = target_sector_id;
                                                temp->coords = (temp->coords - origin_position) + target_sector_spawn;
                                                temp->sprite.setPosition(temp->coords.x,temp->coords.y);
                                            }
                                            sectors[origin_sector_id]->ropes.erase(parser_3->second->connection);
                                        }
                                        else{
                                            //disconnect interface and delete rope
                                            sectors[origin_sector_id]->trash(parser_3->second->connection);
                                        }
                                    }
                                    else if(sectors[target_sector_id]->interfaces.count(parser_3->second->connection)==0){
                                        sectors[target_sector_id]->interfaces[parser_3->second->connection] = sectors[origin_sector_id]->interfaces[parser_3->second->connection];
                                        sectors[origin_sector_id]->interfaces[parser_3->second->connection]->sector_id = target_sector_id;
                                        sectors[origin_sector_id]->interfaces.erase(parser_3->second->connection);
                                    }
                                }
                            }
                            sectors[origin_sector_id]->machines.erase(parser->first);
                        }
                        if(sectors[origin_sector_id]->terminals.count(parser->first) != 0){
                            sectors[target_sector_id]->terminals[parser->first] = sectors[origin_sector_id]->terminals[parser->first];
                            sectors[origin_sector_id]->terminals.erase(parser->first);
                        }
                        if(sectors[origin_sector_id]->people.count(parser->first) != 0){
                            sectors[target_sector_id]->people[parser->first] = sectors[origin_sector_id]->people[parser->first];
                            sectors[origin_sector_id]->people.erase(parser->first);
                        }
                    }
                    if(sectors[origin_sector_id]->floors.count(parser->first) != 0){
                        sectors[target_sector_id]->floors[parser->first] = sectors[origin_sector_id]->floors[parser->first];
                        sectors[origin_sector_id]->floors.erase(parser->first);
                        sectors[target_sector_id]->floors[parser->first]->sector_id = target_sector_id;
                        sectors[target_sector_id]->floors[parser->first]->setPosition(target_sector_spawn.x + parser->second.x,target_sector_spawn.y + parser->second.y);
                    }

                }
                continue;
            }
            if(sectors[origin_sector_id]->ents[transfer_ent_id]->bound_to_sector){
                continue;
            }
            if(sectors[origin_sector_id]->machines.count(transfer_ent_id) != 0){
                bool is_connected = false;
                for(map<string, shared_ptr<FlowPort> >::iterator parser = sectors[origin_sector_id]->machines[transfer_ent_id].lock()->ports.begin(); parser != sectors[origin_sector_id]->machines[transfer_ent_id].lock()->ports.end(); parser++){
                    if(parser->second->isConnected()){
                        is_connected = true;
                        break;
                    }
                }
                if(is_connected){
                    continue;
                }
            }

            for(map<int,string>::iterator parser_2 = sectors[origin_sector_id]->getEnt(transfer_ent_id)->contents.begin(); parser_2 != sectors[origin_sector_id]->getEnt(transfer_ent_id)->contents.end(); parser_2++){
                if(parser_2->second == ""){continue;}
                sectors[target_sector_id]->items[parser_2->second] = sectors[origin_sector_id]->items[parser_2->second];
                sectors[origin_sector_id]->items[parser_2->second]->sector_id = target_sector_id;
                sectors[origin_sector_id]->items.erase(parser_2->second);
            }

            sectors[target_sector_id]->ents[transfer_ent_id] = sectors[origin_sector_id]->ents[transfer_ent_id];
            sectors[origin_sector_id]->ents.erase(transfer_ent_id);
            sectors[target_sector_id]->ents[transfer_ent_id]->sector_id = target_sector_id;
            sectors[target_sector_id]->ents[transfer_ent_id]->setPosition(target_sector_spawn.x,target_sector_spawn.y);


            if(sectors[origin_sector_id]->machines.count(transfer_ent_id) != 0){
                sectors[target_sector_id]->machines[transfer_ent_id] = sectors[origin_sector_id]->machines[transfer_ent_id];
                for(map<string, shared_ptr<FlowPort> >::iterator parser_3 = sectors[origin_sector_id]->machines[transfer_ent_id].lock()->ports.begin(); parser_3 != sectors[origin_sector_id]->machines[transfer_ent_id].lock()->ports.end(); parser_3++){
                    parser_3->second->sector_id = target_sector_id;
                    if(parser_3->second->connection!=""){
                        if(sectors[origin_sector_id]->ropes.count(parser_3->second->connection)!=0){
                            sectors[origin_sector_id]->trash(parser_3->second->connection);
                        }
                        else if(sectors[target_sector_id]->interfaces.count(parser_3->second->connection)==0){
                            sectors[target_sector_id]->interfaces[parser_3->second->connection] = sectors[origin_sector_id]->interfaces[parser_3->second->connection];
                            sectors[origin_sector_id]->interfaces[parser_3->second->connection]->sector_id = target_sector_id;
                            sectors[origin_sector_id]->interfaces.erase(parser_3->second->connection);
                        }
                    }
                }
                sectors[origin_sector_id]->machines.erase(transfer_ent_id);
            }
            if(sectors[origin_sector_id]->terminals.count(transfer_ent_id) != 0){
                sectors[target_sector_id]->terminals[transfer_ent_id] = sectors[origin_sector_id]->terminals[transfer_ent_id];
                sectors[origin_sector_id]->terminals.erase(transfer_ent_id);
            }
            if(sectors[origin_sector_id]->people.count(transfer_ent_id) != 0){
                sectors[target_sector_id]->people[transfer_ent_id] = sectors[origin_sector_id]->people[transfer_ent_id];
                sectors[origin_sector_id]->people.erase(transfer_ent_id);
            }
        }
        if(sectors[origin_sector_id]->floors.count(transfer_ent_id) != 0){

            if(sectors[origin_sector_id]->floors[transfer_ent_id]->bonded){

                if(!sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->floors[transfer_ent_id]->bond]->moveable){return;}//refuses transfer if bond contains things bound to sector

                map<string,Vector2f> ents_to_move;
                Vector2f new_velocity =  sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->ents[transfer_ent_id]->bond]->p/sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->ents[transfer_ent_id]->bond]->mass;
                double new_rot_velocity = sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->ents[transfer_ent_id]->bond]->rot_p/sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->ents[transfer_ent_id]->bond]->getInertia();
                for(set<string>::iterator parser = sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->floors[transfer_ent_id]->bond]->members.begin(); parser != sectors[origin_sector_id]->bonds[sectors[origin_sector_id]->floors[transfer_ent_id]->bond]->members.end(); parser++){
                    //for every member in the bond
                    ents_to_move[*parser] = sectors[origin_sector_id]->getEnt(*parser)->getPosition() - sectors[origin_sector_id]->getEnt(transfer_ent_id)->getPosition();
                    if(sectors[origin_sector_id]->floors.count(*parser)!=0){
                        //for everything resting loose on a floor in the bond
                        for(set<string>::iterator parser_2 = sectors[origin_sector_id]->floors[*parser]->passengers.begin(); parser_2 != sectors[origin_sector_id]->floors[*parser]->passengers.end(); parser_2++){
                            getRestingBonded(*parser_2, ents_to_move, origin_sector_id, transfer_ent_id, target_sector_id, target_sector_spawn);
                        }
                    }
                }
                for(map<string,Vector2f>::iterator parser = ents_to_move.begin(); parser != ents_to_move.end(); parser++){
                    for(set<string>::iterator parser_2 = sectors[origin_sector_id]->getEnt(parser->first)->connectors.begin(); parser_2 != sectors[origin_sector_id]->getEnt(parser->first)->connectors.end(); parser_2++){
                        if(sectors[target_sector_id]->connectors.count(*parser_2)==0){
                            sectors[target_sector_id]->connectors[*parser_2] = sectors[origin_sector_id]->connectors[*parser_2];
                            sectors[origin_sector_id]->connectors[*parser_2]->sector_id = target_sector_id;
                            sectors[origin_sector_id]->trash(*parser_2);
                        }
                    }
                    for(map<int,string>::iterator parser_2 = sectors[origin_sector_id]->getEnt(parser->first)->contents.begin(); parser_2 != sectors[origin_sector_id]->getEnt(parser->first)->contents.end(); parser_2++){
                        if(parser_2->second == ""){continue;}
                        sectors[target_sector_id]->items[parser_2->second] = sectors[origin_sector_id]->items[parser_2->second];
                        sectors[origin_sector_id]->items[parser_2->second]->sector_id = target_sector_id;
                        sectors[origin_sector_id]->items.erase(parser_2->second);
                    }

                    sectors[origin_sector_id]->getEnt(parser->first)->bonded = false;
                    sectors[origin_sector_id]->getEnt(parser->first)->bond = "";
                    sectors[origin_sector_id]->getEnt(parser->first)->setVelocity(new_velocity.x,new_velocity.y);
                    sectors[origin_sector_id]->getEnt(parser->first)->setRotationalVelocity(new_rot_velocity);
                    if(sectors[origin_sector_id]->ents.count(parser->first) != 0){
                        sectors[target_sector_id]->ents[parser->first] = sectors[origin_sector_id]->ents[parser->first];
                        sectors[origin_sector_id]->ents.erase(parser->first);
                        sectors[target_sector_id]->ents[parser->first]->sector_id = target_sector_id;
                        sectors[target_sector_id]->ents[parser->first]->setPosition(target_sector_spawn.x + parser->second.x,target_sector_spawn.y + parser->second.y);

                        if(sectors[origin_sector_id]->machines.count(parser->first) != 0){
                            sectors[target_sector_id]->machines[parser->first] = sectors[origin_sector_id]->machines[parser->first];
                            for(map<string, shared_ptr<FlowPort> >::iterator parser_3 = sectors[origin_sector_id]->machines[parser->first].lock()->ports.begin(); parser_3 != sectors[origin_sector_id]->machines[parser->first].lock()->ports.end(); parser_3++){
                                parser_3->second->sector_id = target_sector_id;

                                if(parser_3->second->connection!=""){
                                    if(sectors[origin_sector_id]->ropes.count(parser_3->second->connection)!=0){
                                        if(ents_to_move.count(sectors[origin_sector_id]->interfaces[parser_3->second->connection]->machine_id_2)!=0 && ents_to_move.count(sectors[origin_sector_id]->interfaces[parser_3->second->connection]->machine_id)!=0){
                                            //copy rope and interface
                                            if(sectors[target_sector_id]->interfaces.count(parser_3->second->connection)==0){
                                                sectors[target_sector_id]->interfaces[parser_3->second->connection] = sectors[origin_sector_id]->interfaces[parser_3->second->connection];
                                                sectors[origin_sector_id]->interfaces[parser_3->second->connection]->sector_id = target_sector_id;
                                                sectors[origin_sector_id]->interfaces.erase(parser_3->second->connection);
                                            }
                                            sectors[target_sector_id]->ropes[parser_3->second->connection] = sectors[origin_sector_id]->ropes[parser_3->second->connection];
                                            sectors[origin_sector_id]->ropes[parser_3->second->connection]->sector_id = target_sector_id;
                                            for(vector<shared_ptr<RopeNode> >::iterator nodes = sectors[origin_sector_id]->ropes[parser_3->second->connection]->nodes.begin(); nodes != sectors[origin_sector_id]->ropes[parser_3->second->connection]->nodes.end(); nodes++){
                                                shared_ptr<RopeNode> temp = *nodes;
                                                temp->sector_id = target_sector_id;
                                                temp->coords = (temp->coords - origin_position) + target_sector_spawn;
                                                temp->sprite.setPosition(temp->coords.x,temp->coords.y);
                                            }
                                            sectors[origin_sector_id]->ropes.erase(parser_3->second->connection);
                                        }
                                        else{
                                            //disconnect interface and delete rope
                                            sectors[origin_sector_id]->trash(parser_3->second->connection);
                                        }
                                    }
                                    else if(sectors[target_sector_id]->interfaces.count(parser_3->second->connection)==0){
                                        sectors[target_sector_id]->interfaces[parser_3->second->connection] = sectors[origin_sector_id]->interfaces[parser_3->second->connection];
                                        sectors[origin_sector_id]->interfaces[parser_3->second->connection]->sector_id = target_sector_id;
                                        sectors[origin_sector_id]->interfaces.erase(parser_3->second->connection);
                                    }
                                }
                            }
                            sectors[origin_sector_id]->machines.erase(parser->first);
                        }
                        if(sectors[origin_sector_id]->terminals.count(parser->first) != 0){
                            sectors[target_sector_id]->terminals[parser->first] = sectors[origin_sector_id]->terminals[parser->first];
                            sectors[origin_sector_id]->terminals.erase(parser->first);
                        }
                        if(sectors[origin_sector_id]->people.count(parser->first) != 0){
                            sectors[target_sector_id]->people[parser->first] = sectors[origin_sector_id]->people[parser->first];
                            sectors[origin_sector_id]->people.erase(parser->first);
                        }
                    }
                    if(sectors[origin_sector_id]->floors.count(parser->first) != 0){
                        sectors[target_sector_id]->floors[parser->first] = sectors[origin_sector_id]->floors[parser->first];
                        sectors[origin_sector_id]->floors.erase(parser->first);
                        sectors[target_sector_id]->floors[parser->first]->sector_id = target_sector_id;
                        sectors[target_sector_id]->floors[parser->first]->setPosition(target_sector_spawn.x + parser->second.x,target_sector_spawn.y + parser->second.y);
                    }

                }
                continue;
            }
            if(sectors[origin_sector_id]->floors[transfer_ent_id]->bound_to_sector){
                continue;
            }

            for(map<int,string>::iterator parser_2 = sectors[origin_sector_id]->getEnt(transfer_ent_id)->contents.begin(); parser_2 != sectors[origin_sector_id]->getEnt(transfer_ent_id)->contents.end(); parser_2++){
                if(parser_2->second == ""){continue;}
                sectors[target_sector_id]->items[parser_2->second] = sectors[origin_sector_id]->items[parser_2->second];
                sectors[origin_sector_id]->items[parser_2->second]->sector_id = target_sector_id;
                sectors[origin_sector_id]->items.erase(parser_2->second);
            }

            sectors[target_sector_id]->floors[transfer_ent_id] = sectors[origin_sector_id]->floors[transfer_ent_id];
            sectors[origin_sector_id]->floors.erase(transfer_ent_id);
            sectors[target_sector_id]->floors[transfer_ent_id]->sector_id = target_sector_id;
            sectors[target_sector_id]->floors[transfer_ent_id]->setPosition(target_sector_spawn.x,target_sector_spawn.y);
        }

        sectors[target_sector_id]->resortAllObjectsIntoBonds();
    }
    transfers.clear();
}

void World::enactChanges(){

    for(map<string, shared_ptr<Change> >::iterator parser = changes.begin(); parser != changes.end(); parser++){
        if(parser->second->triggered){continue;}
        parser->second->updateFlags();

        if(parser->second->trigger()){

            enacted_changes[parser->first] = date;
        }
    }
}

void World::generateStation(double seed){

    string sector_id = "main";
    sectors[sector_id] = make_shared<Sector>(sector_id, 10000,"backdrops\\sand_default.png",200,true);

    makeNewFloor(sector_id, createUniqueId(), 0, 0, 5, 5, 0,"Station Floor");
    makeNewFloor(sector_id, createUniqueId(), 1200, 100, 2, 1, 0,"Steel Floor");
}

bool World::saveWorldToFile(string worldsavename)
{
    TiXmlDocument main_index("saves\\index.xml");
    main_index.LoadFile();
    bool index_exists = false;
    for(TiXmlElement* save_parser = main_index.FirstChildElement("Save"); save_parser != NULL; save_parser = save_parser->NextSiblingElement("Save")){
        if(save_parser->GetText() == worldsavename){
            index_exists = true;
            continue;
        }
    }
    if(!index_exists){
        TiXmlElement* index_listing = new TiXmlElement("Save");
        index_listing->LinkEndChild(new TiXmlText(worldsavename));
        main_index.LinkEndChild(index_listing);
    }
    main_index.SaveFile("saves\\index.xml");

    TiXmlDocument doc; //index
    TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
    doc.LinkEndChild(decl);

    TiXmlElement* world_entry = new TiXmlElement("World");
    world_entry->SetAttribute("id",worldsavename);
    doc.LinkEndChild(world_entry);

    for(map<string,shared_ptr<Sector> >::iterator i = sectors.begin(); i!=sectors.end(); i++)
    {
        TiXmlElement* index_entry = new TiXmlElement("Sector");
        string path_string = "saves/" + worldsavename + "/" + i->first + ".xml";
        index_entry->SetAttribute("path",path_string);
        world_entry->LinkEndChild(index_entry);
    }

    string p = "saves/" + worldsavename + "/";
    CreateDirectory(p.c_str(),NULL);
    doc.SaveFile("saves/" + worldsavename + "/index.xml");

    //index is done

    for(map<string,shared_ptr<Sector> >::iterator i = sectors.begin(); i!=sectors.end(); i++)
    {
        shared_ptr<Sector> sector = i->second;

        TiXmlDocument sector_doc;
        TiXmlDeclaration* sector_decl = new TiXmlDeclaration("1.0", "", "");
        sector_doc.LinkEndChild(sector_decl);

        TiXmlElement* sector_entry = new TiXmlElement("Sector");
        sector_entry->SetAttribute("id",sector->id);
        sector_entry->SetAttribute("radius",asString(sector->radius));
        sector_entry->SetAttribute("ambient",asString(sector->ambient_light));
        sector_entry->SetAttribute("tile",sector->backdrop_tx_path);
        if(sector->planetside){
            sector_entry->SetAttribute("planetside","true");
        }
        else{
            sector_entry->SetAttribute("planetside","false");
        }
        sector_doc.LinkEndChild(sector_entry);

        for(map<string,shared_ptr<Entity> >::iterator j = sector->ents.begin(); j!= sector->ents.end(); j++)
        {
            shared_ptr<Entity> entity = j->second;


            if(entity->properties->classification == "ENTITY" || entity->prop)
            {
                TiXmlElement* object_entry = new TiXmlElement("Entity");
                object_entry->SetAttribute("id",entity->id);
                sector_entry->LinkEndChild(object_entry);

                TiXmlElement* subentry_1 = new TiXmlElement("type");
                subentry_1->LinkEndChild(new TiXmlText(entity->properties->name));
                object_entry->LinkEndChild(subentry_1);

                TiXmlElement* subentry_2 = new TiXmlElement("x");
                subentry_2->LinkEndChild(new TiXmlText(asString(entity->coords.x)));
                object_entry->LinkEndChild(subentry_2);

                TiXmlElement* subentry_3 = new TiXmlElement("y");
                subentry_3->LinkEndChild(new TiXmlText(asString(entity->coords.y)));
                object_entry->LinkEndChild(subentry_3);

                TiXmlElement* subentry_4 = new TiXmlElement("x_velocity");
                subentry_4->LinkEndChild(new TiXmlText(asString(entity->getVelocity().x)));
                object_entry->LinkEndChild(subentry_4);

                TiXmlElement* subentry_5 = new TiXmlElement("y_velocity");
                subentry_5->LinkEndChild(new TiXmlText(asString(entity->getVelocity().y)));
                object_entry->LinkEndChild(subentry_5);

                TiXmlElement* subentry_6 = new TiXmlElement("rotation");
                subentry_6->LinkEndChild(new TiXmlText(asString(entity->sprite.getRotation())));
                object_entry->LinkEndChild(subentry_6);

                TiXmlElement* subentry_7 = new TiXmlElement("rotational_velocity");
                subentry_7->LinkEndChild(new TiXmlText(asString(entity->getRotationalVelocity())));
                object_entry->LinkEndChild(subentry_7);

                TiXmlElement* subentry_8 = new TiXmlElement("slaved");
                if(entity->slaved){ subentry_8->LinkEndChild(new TiXmlText("true"));}
                else{ subentry_8->LinkEndChild(new TiXmlText("false"));}
                object_entry->LinkEndChild(subentry_8);

                TiXmlElement* subentry_9 = new TiXmlElement("prop");
                if(entity->prop){ subentry_9->LinkEndChild(new TiXmlText("true"));}
                else{ subentry_9->LinkEndChild(new TiXmlText("false"));}
                object_entry->LinkEndChild(subentry_9);

                for(map<int,string>::iterator slot_parser = entity->contents.begin(); slot_parser != entity->contents.end(); slot_parser++){
                    if(slot_parser->second!=""){
                        TiXmlElement* slot_subentry = new TiXmlElement("slot");
                        slot_subentry->LinkEndChild(new TiXmlText(slot_parser->second));
                        object_entry->LinkEndChild(slot_subentry);
                    }
                }
            }
            else if(entity->properties->classification == "PLAYER")
            {
                TiXmlElement* object_entry = new TiXmlElement("Player");
                object_entry->SetAttribute("id",entity->id);
                sector_entry->LinkEndChild(object_entry);

                TiXmlElement* subentry_1 = new TiXmlElement("type");
                subentry_1->LinkEndChild(new TiXmlText(entity->properties->name));
                object_entry->LinkEndChild(subentry_1);

                TiXmlElement* subentry_2 = new TiXmlElement("x");
                subentry_2->LinkEndChild(new TiXmlText(asString(entity->coords.x)));
                object_entry->LinkEndChild(subentry_2);

                TiXmlElement* subentry_3 = new TiXmlElement("y");
                subentry_3->LinkEndChild(new TiXmlText(asString(entity->coords.y)));
                object_entry->LinkEndChild(subentry_3);

                TiXmlElement* subentry_4 = new TiXmlElement("x_velocity");
                subentry_4->LinkEndChild(new TiXmlText(asString(entity->getVelocity().x)));
                object_entry->LinkEndChild(subentry_4);

                TiXmlElement* subentry_5 = new TiXmlElement("y_velocity");
                subentry_5->LinkEndChild(new TiXmlText(asString(entity->getVelocity().y)));
                object_entry->LinkEndChild(subentry_5);

                TiXmlElement* subentry_6 = new TiXmlElement("rotation");
                subentry_6->LinkEndChild(new TiXmlText(asString(entity->sprite.getRotation())));
                object_entry->LinkEndChild(subentry_6);

                TiXmlElement* subentry_7 = new TiXmlElement("rotational_velocity");
                subentry_7->LinkEndChild(new TiXmlText(asString(entity->getRotationalVelocity())));
                object_entry->LinkEndChild(subentry_7);

                TiXmlElement* subentry_8 = new TiXmlElement("slaved");
                if(entity->slaved){ subentry_8->LinkEndChild(new TiXmlText("true"));}
                else{ subentry_8->LinkEndChild(new TiXmlText("false"));}
                object_entry->LinkEndChild(subentry_8);

                TiXmlElement* subentry_9 = new TiXmlElement("prop");
                if(entity->prop){ subentry_9->LinkEndChild(new TiXmlText("true"));}
                else{ subentry_9->LinkEndChild(new TiXmlText("false"));}
                object_entry->LinkEndChild(subentry_9);


                for(map<int,string>::iterator slot_parser = entity->contents.begin(); slot_parser != entity->contents.end(); slot_parser++){
                    if(slot_parser->second!=""){
                        TiXmlElement* slot_subentry = new TiXmlElement("slot");
                        slot_subentry->LinkEndChild(new TiXmlText(slot_parser->second));
                        object_entry->LinkEndChild(slot_subentry);
                    }
                }
            }

            else if(j->second->properties->classification == "MACHINE" || j->second->properties->classification == "TERMINAL" || j->second->properties->classification == "CONTROLBOARD")
            {

                TiXmlElement* object_entry = new TiXmlElement("Machine");
                object_entry->SetAttribute("id",entity->id);
                sector_entry->LinkEndChild(object_entry);

                TiXmlElement* subentry_1 = new TiXmlElement("type");
                subentry_1->LinkEndChild(new TiXmlText(entity->properties->name));
                object_entry->LinkEndChild(subentry_1);

                TiXmlElement* subentry_2 = new TiXmlElement("x");
                subentry_2->LinkEndChild(new TiXmlText(asString(entity->coords.x)));
                object_entry->LinkEndChild(subentry_2);

                TiXmlElement* subentry_3 = new TiXmlElement("y");
                subentry_3->LinkEndChild(new TiXmlText(asString(entity->coords.y)));
                object_entry->LinkEndChild(subentry_3);

                TiXmlElement* subentry_4 = new TiXmlElement("x_velocity");
                subentry_4->LinkEndChild(new TiXmlText(asString(entity->getVelocity().x)));
                object_entry->LinkEndChild(subentry_4);

                TiXmlElement* subentry_5 = new TiXmlElement("y_velocity");
                subentry_5->LinkEndChild(new TiXmlText(asString(entity->getVelocity().y)));
                object_entry->LinkEndChild(subentry_5);

                TiXmlElement* subentry_6 = new TiXmlElement("rotation");
                subentry_6->LinkEndChild(new TiXmlText(asString(entity->sprite.getRotation())));
                object_entry->LinkEndChild(subentry_6);

                TiXmlElement* subentry_7 = new TiXmlElement("rotational_velocity");
                subentry_7->LinkEndChild(new TiXmlText(asString(entity->getRotationalVelocity())));
                object_entry->LinkEndChild(subentry_7);

                TiXmlElement* subentry_8 = new TiXmlElement("slaved");
                if(entity->slaved){ subentry_8->LinkEndChild(new TiXmlText("true"));}
                else{ subentry_8->LinkEndChild(new TiXmlText("false"));}
                object_entry->LinkEndChild(subentry_8);

                TiXmlElement* subentry_9 = new TiXmlElement("prop");
                if(entity->prop){ subentry_9->LinkEndChild(new TiXmlText("true"));}
                else{ subentry_9->LinkEndChild(new TiXmlText("false"));}
                object_entry->LinkEndChild(subentry_9);


                if(getSector(sector->id)->machines.count(entity->id)){
                    shared_ptr<Machine> machine = getSector(sector->id)->getMachine(entity->id);
                    for(map<string, shared_ptr<FlowCell> >::iterator cell_parser = machine->internal_cells.begin(); cell_parser!=machine->internal_cells.end(); cell_parser++){
                        for(map<string, double>::iterator var_parser = cell_parser->second->vars.begin(); var_parser!=cell_parser->second->vars.end(); var_parser++){
                            TiXmlElement* cell_entry = new TiXmlElement("cell_var");
                            cell_entry->SetAttribute("cell_id",cell_parser->first);
                            cell_entry->SetAttribute("var_id",var_parser->first);
                            cell_entry->LinkEndChild(new TiXmlText(asString(var_parser->second)));
                            object_entry->LinkEndChild(cell_entry);
                        }
                        for(map<string, string>::iterator data_parser = cell_parser->second->data.begin(); data_parser!=cell_parser->second->data.end(); data_parser++){
                            TiXmlElement* cell_entry = new TiXmlElement("cell_data");
                            cell_entry->SetAttribute("cell_id",cell_parser->first);
                            cell_entry->SetAttribute("data_id",data_parser->first);
                            cell_entry->LinkEndChild(new TiXmlText(data_parser->second));
                            object_entry->LinkEndChild(cell_entry);
                        }
                    }

                    for(map<string, shared_ptr<FlowPort> >::iterator port_parser = machine->ports.begin(); port_parser!=machine->ports.end(); port_parser++){
                        if(port_parser->second->connection!=""){
                            TiXmlElement* port_entry = new TiXmlElement("port_connection");
                            port_entry->SetAttribute("port_id",port_parser->first);
                            port_entry->LinkEndChild(new TiXmlText(port_parser->second->connection));
                            object_entry->LinkEndChild(port_entry);
                        }
                    }
                    for(map<int,string>::iterator slot_parser = entity->contents.begin(); slot_parser != entity->contents.end(); slot_parser++){
                        if(slot_parser->second!=""){
                            TiXmlElement* slot_subentry = new TiXmlElement("slot");
                            slot_subentry->LinkEndChild(new TiXmlText(slot_parser->second));
                            object_entry->LinkEndChild(slot_subentry);
                        }
                    }
                }

            }
            else if(j->second->properties->classification == "CONTAINER")
            {
                TiXmlElement* object_entry = new TiXmlElement("Entity");
                object_entry->SetAttribute("id",entity->id);
                sector_entry->LinkEndChild(object_entry);

                TiXmlElement* subentry_1 = new TiXmlElement("type");
                subentry_1->LinkEndChild(new TiXmlText(entity->properties->name));
                object_entry->LinkEndChild(subentry_1);

                TiXmlElement* subentry_2 = new TiXmlElement("x");
                subentry_2->LinkEndChild(new TiXmlText(asString(entity->coords.x)));
                object_entry->LinkEndChild(subentry_2);

                TiXmlElement* subentry_3 = new TiXmlElement("y");
                subentry_3->LinkEndChild(new TiXmlText(asString(entity->coords.y)));
                object_entry->LinkEndChild(subentry_3);

                TiXmlElement* subentry_4 = new TiXmlElement("x_velocity");
                subentry_4->LinkEndChild(new TiXmlText(asString(entity->getVelocity().x)));
                object_entry->LinkEndChild(subentry_4);

                TiXmlElement* subentry_5 = new TiXmlElement("y_velocity");
                subentry_5->LinkEndChild(new TiXmlText(asString(entity->getVelocity().y)));
                object_entry->LinkEndChild(subentry_5);

                TiXmlElement* subentry_6 = new TiXmlElement("rotation");
                subentry_6->LinkEndChild(new TiXmlText(asString(entity->sprite.getRotation())));
                object_entry->LinkEndChild(subentry_6);

                TiXmlElement* subentry_7 = new TiXmlElement("rotational_velocity");
                subentry_7->LinkEndChild(new TiXmlText(asString(entity->getRotationalVelocity())));
                object_entry->LinkEndChild(subentry_7);

                TiXmlElement* subentry_8 = new TiXmlElement("slaved");
                if(entity->slaved){ subentry_8->LinkEndChild(new TiXmlText("true"));}
                else{ subentry_8->LinkEndChild(new TiXmlText("false"));}
                object_entry->LinkEndChild(subentry_8);

                TiXmlElement* subentry_9 = new TiXmlElement("prop");
                if(entity->prop){ subentry_9->LinkEndChild(new TiXmlText("true"));}
                else{ subentry_9->LinkEndChild(new TiXmlText("false"));}
                object_entry->LinkEndChild(subentry_9);


                for(map<int,string>::iterator slot_parser = entity->contents.begin(); slot_parser != entity->contents.end(); slot_parser++){
                    if(slot_parser->second!=""){
                        TiXmlElement* slot_subentry = new TiXmlElement("slot");
                        slot_subentry->LinkEndChild(new TiXmlText(slot_parser->second));
                        object_entry->LinkEndChild(slot_subentry);
                    }
                }
            }
        }
        for(map<string,shared_ptr<Floor> >::iterator j = sector->floors.begin(); j!= sector->floors.end(); j++)
        {
            shared_ptr<Floor> floor = j->second;

            TiXmlElement* object_entry = new TiXmlElement("Floor");
            object_entry->SetAttribute("id",floor->id);
            sector_entry->LinkEndChild(object_entry);

            TiXmlElement* subentry_1 = new TiXmlElement("type");
            subentry_1->LinkEndChild(new TiXmlText(floor->properties->name));
            object_entry->LinkEndChild(subentry_1);

            TiXmlElement* subentry_2 = new TiXmlElement("x");
            subentry_2->LinkEndChild(new TiXmlText(asString(floor->coords.x)));
            object_entry->LinkEndChild(subentry_2);

            TiXmlElement* subentry_3 = new TiXmlElement("y");
            subentry_3->LinkEndChild(new TiXmlText(asString(floor->coords.y)));
            object_entry->LinkEndChild(subentry_3);

            TiXmlElement* subentry_4 = new TiXmlElement("x_velocity");
            subentry_4->LinkEndChild(new TiXmlText(asString(floor->getVelocity().x)));
            object_entry->LinkEndChild(subentry_4);

            TiXmlElement* subentry_5 = new TiXmlElement("y_velocity");
            subentry_5->LinkEndChild(new TiXmlText(asString(floor->getVelocity().y)));
            object_entry->LinkEndChild(subentry_5);

            TiXmlElement* subentry_6 = new TiXmlElement("rotation");
            subentry_6->LinkEndChild(new TiXmlText(asString(floor->sprite.getRotation())));
            object_entry->LinkEndChild(subentry_6);

            TiXmlElement* subentry_7 = new TiXmlElement("rotational_velocity");
            subentry_7->LinkEndChild(new TiXmlText(asString(floor->getRotationalVelocity())));
            object_entry->LinkEndChild(subentry_7);

            TiXmlElement* subentry_8 = new TiXmlElement("x_tiles");
            subentry_8->LinkEndChild(new TiXmlText(asString(floor->tiles_x)));
            object_entry->LinkEndChild(subentry_8);

            TiXmlElement* subentry_9 = new TiXmlElement("y_tiles");
            subentry_9->LinkEndChild(new TiXmlText(asString(floor->tiles_y)));
            object_entry->LinkEndChild(subentry_9);

            TiXmlElement* subentry_10 = new TiXmlElement("prop");
            if(floor->prop){ subentry_10->LinkEndChild(new TiXmlText("true"));}
            else{ subentry_10->LinkEndChild(new TiXmlText("false"));}
            object_entry->LinkEndChild(subentry_10);
        }
        for(map<string,shared_ptr<Poster> >::iterator j = sector->posters.begin(); j!= sector->posters.end(); j++){

            shared_ptr<Poster> poster = j->second;

            TiXmlElement* object_entry = new TiXmlElement("Poster");
            object_entry->SetAttribute("id",poster->id);
            sector_entry->LinkEndChild(object_entry);

            TiXmlElement* subentry_1 = new TiXmlElement("type");
            subentry_1->LinkEndChild(new TiXmlText(poster->properties->name));
            object_entry->LinkEndChild(subentry_1);

            TiXmlElement* subentry_2 = new TiXmlElement("relative_x");
            subentry_2->LinkEndChild(new TiXmlText(asString(poster->mount_position.x)));
            object_entry->LinkEndChild(subentry_2);

            TiXmlElement* subentry_3 = new TiXmlElement("relative_y");
            subentry_3->LinkEndChild(new TiXmlText(asString(poster->mount_position.y)));
            object_entry->LinkEndChild(subentry_3);

            TiXmlElement* subentry_4 = new TiXmlElement("parent_id");
            subentry_4->LinkEndChild(new TiXmlText(poster->parent_id));
            object_entry->LinkEndChild(subentry_4);
        }
        for(map<string,shared_ptr<Field> >::iterator j = sector->fields.begin(); j!= sector->fields.end(); j++){

            shared_ptr<Field> field = j->second;

            TiXmlElement* object_entry = new TiXmlElement("Field");
            object_entry->SetAttribute("id",field->id);
            sector_entry->LinkEndChild(object_entry);

            TiXmlElement* subentry_1 = new TiXmlElement("type");
            subentry_1->LinkEndChild(new TiXmlText(field->type));
            object_entry->LinkEndChild(subentry_1);

            TiXmlElement* subentry_2 = new TiXmlElement("bounds_type");
            if(field->bounds_type == 0){
                subentry_2->LinkEndChild(new TiXmlText("rectangle"));
                object_entry->LinkEndChild(subentry_2);

                TiXmlElement* subentry_3 = new TiXmlElement("tl_x");
                subentry_3->LinkEndChild(new TiXmlText(asString(field->bounds_r.getGlobalBounds().left)));
                object_entry->LinkEndChild(subentry_3);

                TiXmlElement* subentry_4 = new TiXmlElement("tl_y");
                subentry_4->LinkEndChild(new TiXmlText(asString(field->bounds_r.getGlobalBounds().top)));
                object_entry->LinkEndChild(subentry_4);

                TiXmlElement* subentry_5 = new TiXmlElement("br_x");
                subentry_5->LinkEndChild(new TiXmlText(asString(field->bounds_r.getGlobalBounds().left + field->bounds_r.getGlobalBounds().width)));
                object_entry->LinkEndChild(subentry_5);

                TiXmlElement* subentry_6 = new TiXmlElement("br_y");
                subentry_6->LinkEndChild(new TiXmlText(asString(field->bounds_r.getGlobalBounds().top + field->bounds_r.getGlobalBounds().height)));
                object_entry->LinkEndChild(subentry_6);

                TiXmlElement* subentry_7 = new TiXmlElement("r");
                subentry_7->LinkEndChild(new TiXmlText(asString(field->bounds_c.getFillColor().r)));
                object_entry->LinkEndChild(subentry_7);

                TiXmlElement* subentry_8 = new TiXmlElement("g");
                subentry_8->LinkEndChild(new TiXmlText(asString(field->bounds_c.getFillColor().g)));
                object_entry->LinkEndChild(subentry_8);

                TiXmlElement* subentry_9 = new TiXmlElement("b");
                subentry_9->LinkEndChild(new TiXmlText(asString(field->bounds_c.getFillColor().b)));
                object_entry->LinkEndChild(subentry_9);

                TiXmlElement* subentry_10 = new TiXmlElement("a");
                subentry_10->LinkEndChild(new TiXmlText(asString(field->bounds_c.getFillColor().a)));
                object_entry->LinkEndChild(subentry_10);
            }
            else if(field->bounds_type == 1){
                subentry_2->LinkEndChild(new TiXmlText("circle"));
                object_entry->LinkEndChild(subentry_2);

                TiXmlElement* subentry_3 = new TiXmlElement("x");
                subentry_3->LinkEndChild(new TiXmlText(asString(field->bounds_c.getPosition().x)));
                object_entry->LinkEndChild(subentry_3);

                TiXmlElement* subentry_4 = new TiXmlElement("y");
                subentry_4->LinkEndChild(new TiXmlText(asString(field->bounds_c.getPosition().y)));
                object_entry->LinkEndChild(subentry_4);

                TiXmlElement* subentry_5 = new TiXmlElement("radius");
                subentry_5->LinkEndChild(new TiXmlText(asString(field->bounds_c.getRadius())));
                object_entry->LinkEndChild(subentry_5);

                TiXmlElement* subentry_6 = new TiXmlElement("r");
                subentry_6->LinkEndChild(new TiXmlText(asString(field->bounds_c.getFillColor().r)));
                object_entry->LinkEndChild(subentry_6);

                TiXmlElement* subentry_7 = new TiXmlElement("g");
                subentry_7->LinkEndChild(new TiXmlText(asString(field->bounds_c.getFillColor().g)));
                object_entry->LinkEndChild(subentry_7);

                TiXmlElement* subentry_8 = new TiXmlElement("b");
                subentry_8->LinkEndChild(new TiXmlText(asString(field->bounds_c.getFillColor().b)));
                object_entry->LinkEndChild(subentry_8);

                TiXmlElement* subentry_9 = new TiXmlElement("a");
                subentry_9->LinkEndChild(new TiXmlText(asString(field->bounds_c.getFillColor().a)));
                object_entry->LinkEndChild(subentry_9);
            }


            if(field->type=="gate"){
                TiXmlElement* subentry_11 = new TiXmlElement("linked_sector");
                subentry_11->LinkEndChild(new TiXmlText(field->linked_sector_id));
                object_entry->LinkEndChild(subentry_11);

                TiXmlElement* subentry_12 = new TiXmlElement("linked_x");
                subentry_12->LinkEndChild(new TiXmlText(asString(field->linked_sector_spawn.x)));
                object_entry->LinkEndChild(subentry_12);

                TiXmlElement* subentry_13 = new TiXmlElement("linked_y");
                subentry_13->LinkEndChild(new TiXmlText(asString(field->linked_sector_spawn.y)));
                object_entry->LinkEndChild(subentry_13);
            }


        }
        for(map<string,shared_ptr<Interface> >::iterator j = sector->interfaces.begin(); j!= sector->interfaces.end(); j++){

            shared_ptr<Interface> this_interface = j->second;
            if(this_interface->properties->classification=="INTERFACE"){

                TiXmlElement* object_entry = new TiXmlElement("Interface");
                object_entry->SetAttribute("id",this_interface->id);
                sector_entry->LinkEndChild(object_entry);

                TiXmlElement* subentry_1 = new TiXmlElement("type");
                subentry_1->LinkEndChild(new TiXmlText(this_interface->properties->name));
                object_entry->LinkEndChild(subentry_1);

                TiXmlElement* subentry_2 = new TiXmlElement("machine_id");
                subentry_2->LinkEndChild(new TiXmlText(this_interface->machine_id));
                object_entry->LinkEndChild(subentry_2);

                TiXmlElement* subentry_3 = new TiXmlElement("port_id");
                subentry_3->LinkEndChild(new TiXmlText(this_interface->port_id));
                object_entry->LinkEndChild(subentry_3);

                TiXmlElement* subentry_4 = new TiXmlElement("relative_x");
                subentry_4->LinkEndChild(new TiXmlText(asString(this_interface->relative_position[0].x)));
                object_entry->LinkEndChild(subentry_4);

                TiXmlElement* subentry_5 = new TiXmlElement("relative_y");
                subentry_5->LinkEndChild(new TiXmlText(asString(this_interface->relative_position[0].y)));
                object_entry->LinkEndChild(subentry_5);

                for(map<string, double>::iterator var_parser = this_interface->var_num.begin(); var_parser != this_interface->var_num.end(); var_parser++){
                    TiXmlElement* var_subentry = new TiXmlElement("var_num");
                    var_subentry->SetAttribute("id",var_parser->first);
                    var_subentry->LinkEndChild(new TiXmlText(asString(var_parser->second)));
                    object_entry->LinkEndChild(var_subentry);
                }
            }
            else{
                TiXmlElement* object_entry = new TiXmlElement("Transfer");
                object_entry->SetAttribute("id",this_interface->id);
                sector_entry->LinkEndChild(object_entry);

                TiXmlElement* subentry_1 = new TiXmlElement("type");
                subentry_1->LinkEndChild(new TiXmlText(this_interface->properties->name));
                object_entry->LinkEndChild(subentry_1);

                TiXmlElement* subentry_2 = new TiXmlElement("machine_id_a");
                subentry_2->LinkEndChild(new TiXmlText(this_interface->machine_id));
                object_entry->LinkEndChild(subentry_2);

                TiXmlElement* subentry_3 = new TiXmlElement("port_id_a");
                subentry_3->LinkEndChild(new TiXmlText(this_interface->port_id));
                object_entry->LinkEndChild(subentry_3);

                TiXmlElement* subentry_4 = new TiXmlElement("machine_id_b");
                subentry_4->LinkEndChild(new TiXmlText(this_interface->machine_id_2));
                object_entry->LinkEndChild(subentry_4);

                TiXmlElement* subentry_5 = new TiXmlElement("port_id_b");
                subentry_5->LinkEndChild(new TiXmlText(this_interface->port_id_2));
                object_entry->LinkEndChild(subentry_5);

                for(map<string, double>::iterator var_parser = this_interface->var_num.begin(); var_parser != this_interface->var_num.end(); var_parser++){
                    TiXmlElement* var_subentry = new TiXmlElement("var_num");
                    var_subentry->SetAttribute("id",var_parser->first);
                    var_subentry->LinkEndChild(new TiXmlText(asString(var_parser->second)));
                    object_entry->LinkEndChild(var_subentry);
                }
            }
        }
        for(map<string,shared_ptr<Item> >::iterator j = sector->items.begin(); j!= sector->items.end(); j++)
        {
            shared_ptr<Item> item = j->second;

            if(j->second->properties->classification!="CHIP"){
                TiXmlElement* object_entry = new TiXmlElement("Item");
                object_entry->SetAttribute("id",item->id);
                sector_entry->LinkEndChild(object_entry);

                TiXmlElement* subentry_1 = new TiXmlElement("type");
                subentry_1->LinkEndChild(new TiXmlText(item->properties->name));
                object_entry->LinkEndChild(subentry_1);

               // TiXmlElement* subentry_2 = new TiXmlElement("storage_id");
              //  subentry_2->LinkEndChild(new TiXmlText(item->storage_id));
               // object_entry->LinkEndChild(subentry_2);
            }
            else{

                shared_ptr<Chip> chip = getSector(sector->id)->chips[item->id].lock();

                TiXmlElement* object_entry = new TiXmlElement("Chip");
                object_entry->SetAttribute("id",chip->id);
                sector_entry->LinkEndChild(object_entry);

                TiXmlElement* subentry_1 = new TiXmlElement("type");
                subentry_1->LinkEndChild(new TiXmlText(chip->properties->name));
                object_entry->LinkEndChild(subentry_1);

              //  TiXmlElement* subentry_2 = new TiXmlElement("storage_id");
              //  subentry_2->LinkEndChild(new TiXmlText(chip->storage_id));
               // object_entry->LinkEndChild(subentry_2);

                for(map<int, double>::iterator var_parser = chip->var_num.begin(); var_parser != chip->var_num.end(); var_parser++){
                    TiXmlElement* var_subentry = new TiXmlElement("var_num");
                    var_subentry->SetAttribute("id",asString(var_parser->first));
                    var_subentry->LinkEndChild(new TiXmlText(asString(var_parser->second)));
                    object_entry->LinkEndChild(var_subentry);
                }
                for(map<int, string>::iterator var_parser = chip->var_str.begin(); var_parser != chip->var_str.end(); var_parser++){
                    TiXmlElement* var_subentry = new TiXmlElement("var_str");
                    var_subentry->SetAttribute("id",asString(var_parser->first));
                    var_subentry->LinkEndChild(new TiXmlText(var_parser->second));
                    object_entry->LinkEndChild(var_subentry);
                }
            }

        }
        for(map<string,shared_ptr<Connector> >::iterator j = sector->connectors.begin(); j!= sector->connectors.end(); j++)
        {
            shared_ptr<Connector> connector = j->second;

            TiXmlElement* object_entry = new TiXmlElement("Connector");
            object_entry->SetAttribute("id",connector->id);
            sector_entry->LinkEndChild(object_entry);

            TiXmlElement* subentry_1 = new TiXmlElement("type");
            subentry_1->LinkEndChild(new TiXmlText(connector->type));
            object_entry->LinkEndChild(subentry_1);

            TiXmlElement* subentry_2 = new TiXmlElement("A");
            subentry_2->LinkEndChild(new TiXmlText(connector->a_id));
            object_entry->LinkEndChild(subentry_2);

            TiXmlElement* subentry_3 = new TiXmlElement("B");
            subentry_3->LinkEndChild(new TiXmlText(connector->b_id));
            object_entry->LinkEndChild(subentry_3);

            if(connector->has_sprite){
                TiXmlElement* subentry_4 = new TiXmlElement("relative_x");
                subentry_4->LinkEndChild(new TiXmlText(asString(connector->relative_position.x)));
                object_entry->LinkEndChild(subentry_4);

                TiXmlElement* subentry_5 = new TiXmlElement("relative_y");
                subentry_5->LinkEndChild(new TiXmlText(asString(connector->relative_position.y)));
                object_entry->LinkEndChild(subentry_5);

                TiXmlElement* subentry_6 = new TiXmlElement("relative_r");
                subentry_6->LinkEndChild(new TiXmlText(asString(connector->relative_rotation)));
                object_entry->LinkEndChild(subentry_6);
            }
            
        }
        for(map<string,shared_ptr<Rope> >::iterator j = sector->ropes.begin(); j!= sector->ropes.end(); j++){

            shared_ptr<Rope> rope = j->second;

            TiXmlElement* object_entry = new TiXmlElement("Rope");
            object_entry->SetAttribute("id",rope->id);
            sector_entry->LinkEndChild(object_entry);

            TiXmlElement* subentry_1 = new TiXmlElement("type");
            subentry_1->LinkEndChild(new TiXmlText(rope->properties->name));
            object_entry->LinkEndChild(subentry_1);

            for(vector<shared_ptr<RopeNode> >::iterator node_parser = rope->nodes.begin(); node_parser != rope->nodes.end(); node_parser++){

                shared_ptr<RopeNode> node = *node_parser;
                TiXmlElement* node_entry = new TiXmlElement("node");
                object_entry->LinkEndChild(node_entry);

                TiXmlElement* node_subentry_1 = new TiXmlElement("x");
                node_subentry_1->LinkEndChild(new TiXmlText(asString(node->coords.x)));
                node_entry->LinkEndChild(node_subentry_1);

                TiXmlElement* node_subentry_2 = new TiXmlElement("y");
                node_subentry_2->LinkEndChild(new TiXmlText(asString(node->coords.y)));
                node_entry->LinkEndChild(node_subentry_2);

                TiXmlElement* node_subentry_3 = new TiXmlElement("x_velocity");
                node_subentry_3->LinkEndChild(new TiXmlText(asString(node->getVelocity().x)));
                node_entry->LinkEndChild(node_subentry_3);

                TiXmlElement* node_subentry_4 = new TiXmlElement("y_velocity");
                node_subentry_4->LinkEndChild(new TiXmlText(asString(node->getVelocity().y)));
                node_entry->LinkEndChild(node_subentry_4);

                TiXmlElement* node_subentry_5 = new TiXmlElement("attached");
                node_subentry_5->LinkEndChild(new TiXmlText(node->attached_id));
                node_entry->LinkEndChild(node_subentry_5);

                TiXmlElement* node_subentry_6 = new TiXmlElement("attachment_x");
                node_subentry_6->LinkEndChild(new TiXmlText(asString(node->attachment_coords.x)));
                node_entry->LinkEndChild(node_subentry_6);

                TiXmlElement* node_subentry_7 = new TiXmlElement("attachment_y");
                node_subentry_7->LinkEndChild(new TiXmlText(asString(node->attachment_coords.y)));
                node_entry->LinkEndChild(node_subentry_7);
            }
        }

        sector_doc.SaveFile("saves/" + worldsavename + "/" + i->first + ".xml");
    }

    return true;
}

bool World::loadWorldFromFile(string worldsavename)
{
    TiXmlDocument doc("saves/"+worldsavename+"/index.xml");
    bool doc_valid = doc.LoadFile();
    if(not doc_valid){return false;}

    TiXmlElement* world_element = doc.FirstChildElement();
    string test_string = world_element->ValueStr();
    if(test_string!="World"){return false;}

    id = world_element->Attribute("id");

    for(TiXmlElement* index_element = world_element->FirstChildElement(); index_element != NULL; index_element = index_element->NextSiblingElement())
    {
        TiXmlDocument sector_doc(index_element->Attribute("path"));
        doc_valid = sector_doc.LoadFile();
        if(not doc_valid){return false;}

        TiXmlElement* sector_element = sector_doc.FirstChildElement();
        test_string = sector_element->ValueStr();
        if(test_string!="Sector"){return false;}
        string current_sector_id = sector_element->Attribute("id");

        bool planetside = false;
        if(string(sector_element->Attribute("planetside")) == "true"){ planetside = true; }
        sectors[current_sector_id] = make_shared<Sector>(current_sector_id, strtod(sector_element->Attribute("radius"),NULL), sector_element->Attribute("tile"), strtod(sector_element->Attribute("ambient"),NULL), planetside);

        for(TiXmlElement* object_element = sector_element->FirstChildElement("Entity"); object_element!= NULL; object_element = object_element->NextSiblingElement("Entity"))
        {
            if(string(object_element->FirstChildElement("prop")->GetText())=="true"){
                makeNewProp(current_sector_id, object_element->Attribute("id"), strtod(object_element->FirstChildElement("x")->GetText(),NULL), strtod(object_element->FirstChildElement("y")->GetText(),NULL),strtod(object_element->FirstChildElement("rotation")->GetText(),NULL),object_element->FirstChildElement("type")->GetText());
            }
            else{
                makeNewEntity(current_sector_id, object_element->Attribute("id"), strtod(object_element->FirstChildElement("x")->GetText(),NULL), strtod(object_element->FirstChildElement("y")->GetText(),NULL),strtod(object_element->FirstChildElement("rotation")->GetText(),NULL),object_element->FirstChildElement("type")->GetText());
                getSector(current_sector_id)->ents[object_element->Attribute("id")]->setRotationalVelocity(strtod(object_element->FirstChildElement("rotational_velocity")->GetText(),NULL));
                getSector(current_sector_id)->ents[object_element->Attribute("id")]->setVelocity(strtod(object_element->FirstChildElement("x_velocity")->GetText(),NULL),strtod(object_element->FirstChildElement("y_velocity")->GetText(),NULL));
                if(string(object_element->FirstChildElement("slaved")->GetText()) == "true"){
                    getSector(current_sector_id)->ents[object_element->Attribute("id")]->slaved = true;
                }
                int i = 1;
                for(TiXmlElement* sub_element = object_element->FirstChildElement("slot"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("slot")){
                    getSector(current_sector_id)->ents[object_element->Attribute("id")]->contents[i] = sub_element->GetText();
                    i++;
                }
            }

        }
        for(TiXmlElement* object_element = sector_element->FirstChildElement("Player"); object_element!= NULL; object_element = object_element->NextSiblingElement("Player"))
        {
            makeNewPlayer(current_sector_id, object_element->Attribute("id"), strtod(object_element->FirstChildElement("x")->GetText(),NULL), strtod(object_element->FirstChildElement("y")->GetText(),NULL),strtod(object_element->FirstChildElement("rotation")->GetText(),NULL),object_element->FirstChildElement("type")->GetText());
            getSector(current_sector_id)->ents[object_element->Attribute("id")]->setRotationalVelocity(strtod(object_element->FirstChildElement("rotational_velocity")->GetText(),NULL));
            getSector(current_sector_id)->ents[object_element->Attribute("id")]->setVelocity(strtod(object_element->FirstChildElement("x_velocity")->GetText(),NULL),strtod(object_element->FirstChildElement("y_velocity")->GetText(),NULL));
            if(string(object_element->FirstChildElement("slaved")->GetText()) == "true"){
                getSector(current_sector_id)->ents[object_element->Attribute("id")]->slaved = true;
            }
            int i = 1;
            for(TiXmlElement* sub_element = object_element->FirstChildElement("slot"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("slot")){
                getSector(current_sector_id)->ents[object_element->Attribute("id")]->contents[i] = sub_element->GetText();
                i++;
            }
        }
        for(TiXmlElement* object_element = sector_element->FirstChildElement("Machine"); object_element!= NULL; object_element = object_element->NextSiblingElement("Machine"))
        {
            makeNewMachine(current_sector_id, object_element->Attribute("id"), strtod(object_element->FirstChildElement("x")->GetText(),NULL), strtod(object_element->FirstChildElement("y")->GetText(),NULL),strtod(object_element->FirstChildElement("rotation")->GetText(),NULL),object_element->FirstChildElement("type")->GetText(), true);
            getSector(current_sector_id)->ents[object_element->Attribute("id")]->setRotationalVelocity(strtod(object_element->FirstChildElement("rotational_velocity")->GetText(),NULL));
            getSector(current_sector_id)->ents[object_element->Attribute("id")]->setVelocity(strtod(object_element->FirstChildElement("x_velocity")->GetText(),NULL),strtod(object_element->FirstChildElement("y_velocity")->GetText(),NULL));
            if(string(object_element->FirstChildElement("slaved")->GetText()) == "true"){
                getSector(current_sector_id)->ents[object_element->Attribute("id")]->slaved = true;
            }
            int i = 1;
            for(TiXmlElement* sub_element = object_element->FirstChildElement("slot"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("slot")){
                getSector(current_sector_id)->ents[object_element->Attribute("id")]->contents[i] = sub_element->GetText();
                i++;
            }
            for(TiXmlElement* sub_element = object_element->FirstChildElement("cell_var"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("cell_var"))
            {
                getSector(current_sector_id)->getMachine(object_element->Attribute("id"))->internal_cells[sub_element->Attribute("cell_id")]->vars[sub_element->Attribute("var_id")] = strtod(sub_element->GetText(),NULL);
            }
            for(TiXmlElement* sub_element = object_element->FirstChildElement("cell_data"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("cell_data"))
            {
                if(sub_element->GetText() == NULL){
                    getSector(current_sector_id)->getMachine(object_element->Attribute("id"))->internal_cells[sub_element->Attribute("cell_id")]->data[sub_element->Attribute("data_id")] = "";
                }
                else{
                    getSector(current_sector_id)->getMachine(object_element->Attribute("id"))->internal_cells[sub_element->Attribute("cell_id")]->data[sub_element->Attribute("data_id")] = sub_element->GetText();
                }
            }
            for(TiXmlElement* sub_element = object_element->FirstChildElement("port_connection"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("port_connection"))
            {
                getSector(current_sector_id)->getMachine(object_element->Attribute("id"))->ports[sub_element->Attribute("port_id")]->connection = sub_element->GetText();
            }

        }
        for(TiXmlElement* object_element = sector_element->FirstChildElement("Poster"); object_element!= NULL; object_element = object_element->NextSiblingElement("Poster"))
        {
            makeNewPoster(current_sector_id, object_element->Attribute("id"), object_element->FirstChildElement("type")->GetText(), object_element->FirstChildElement("parent_id")->GetText(), Vector2f(strtod(object_element->FirstChildElement("relative_x")->GetText(),NULL),strtod(object_element->FirstChildElement("relative_y")->GetText(),NULL)));
        }
        for(TiXmlElement* object_element = sector_element->FirstChildElement("Field"); object_element!= NULL; object_element = object_element->NextSiblingElement("Field")){

            string type = object_element->FirstChildElement("type")->GetText();
            if(type == "gate"){
                string bounds_type = object_element->FirstChildElement("bounds_type")->GetText();
                if(bounds_type == "rectangle"){//rectangle
                    getSector(current_sector_id)->fields[object_element->Attribute("id")] = make_shared<GateField>(current_sector_id, object_element->Attribute("id"), object_element->FirstChildElement("linked_sector")->GetText(), Vector2f(strtod(object_element->FirstChildElement("linked_x")->GetText(), NULL),strtod(object_element->FirstChildElement("linked_y")->GetText(), NULL)), Vector2f(strtod(object_element->FirstChildElement("tl_x")->GetText(), NULL),strtod(object_element->FirstChildElement("tl_y")->GetText(), NULL)),Vector2f(strtod(object_element->FirstChildElement("br_x")->GetText(), NULL),strtod(object_element->FirstChildElement("br_y")->GetText(), NULL)),Color(strtod(object_element->FirstChildElement("r")->GetText(), NULL),strtod(object_element->FirstChildElement("g")->GetText(), NULL),strtod(object_element->FirstChildElement("b")->GetText(), NULL),strtod(object_element->FirstChildElement("a")->GetText(), NULL)));
                }
                else if(bounds_type == "circle"){ //circle
                    getSector(current_sector_id)->fields[object_element->Attribute("id")] = make_shared<GateField>(current_sector_id, object_element->Attribute("id"), object_element->FirstChildElement("linked_sector")->GetText(), Vector2f(strtod(object_element->FirstChildElement("linked_x")->GetText(), NULL),strtod(object_element->FirstChildElement("linked_y")->GetText(), NULL)), Vector2f(strtod(object_element->FirstChildElement("x")->GetText(), NULL),strtod(object_element->FirstChildElement("y")->GetText(), NULL)),strtod(object_element->FirstChildElement("radius")->GetText(),NULL),Color(strtod(object_element->FirstChildElement("r")->GetText(), NULL),strtod(object_element->FirstChildElement("g")->GetText(), NULL),strtod(object_element->FirstChildElement("b")->GetText(), NULL),strtod(object_element->FirstChildElement("a")->GetText(), NULL)));
                }

            }
        }
        for(TiXmlElement* object_element = sector_element->FirstChildElement("Interface"); object_element!= NULL; object_element = object_element->NextSiblingElement("Interface"))
        {
            makeNewInterface(current_sector_id, object_element->Attribute("id"), object_element->FirstChildElement("port_id")->GetText(), object_element->FirstChildElement("machine_id")->GetText(), Vector2f(strtod(object_element->FirstChildElement("relative_x")->GetText(),NULL), strtod(object_element->FirstChildElement("relative_y")->GetText(),NULL)),object_element->FirstChildElement("type")->GetText());
            for(TiXmlElement* sub_element = object_element->FirstChildElement("var_num"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("var_num"))
            {
                getSector(current_sector_id)->interfaces[object_element->Attribute("id")]->var_num[sub_element->Attribute("id")] = strtod(sub_element->GetText(),NULL);
            }
        }
        for(TiXmlElement* object_element = sector_element->FirstChildElement("Transfer"); object_element!= NULL; object_element = object_element->NextSiblingElement("Transfer"))
        {
            makeNewTransfer(current_sector_id, object_element->Attribute("id"), object_element->FirstChildElement("port_id_a")->GetText(), object_element->FirstChildElement("port_id_b")->GetText(), object_element->FirstChildElement("machine_id_a")->GetText(), object_element->FirstChildElement("machine_id_b")->GetText(),object_element->FirstChildElement("type")->GetText());
            for(TiXmlElement* sub_element = object_element->FirstChildElement("var_num"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("var_num"))
            {
                getSector(current_sector_id)->interfaces[object_element->Attribute("id")]->var_num[sub_element->Attribute("id")] = strtod(sub_element->GetText(),NULL);
            }
        }
        for(TiXmlElement* object_element = sector_element->FirstChildElement("Floor"); object_element!= NULL; object_element = object_element->NextSiblingElement("Floor"))
        {
            if(string(object_element->FirstChildElement("prop")->GetText())=="true"){
                makeNewFloorProp(current_sector_id, object_element->Attribute("id"), strtod(object_element->FirstChildElement("x")->GetText(),NULL), strtod(object_element->FirstChildElement("y")->GetText(),NULL), strtod(object_element->FirstChildElement("x_tiles")->GetText(),NULL), strtod(object_element->FirstChildElement("y_tiles")->GetText(),NULL),strtod(object_element->FirstChildElement("rotation")->GetText(),NULL),object_element->FirstChildElement("type")->GetText());
            }
            else{
                makeNewFloor(current_sector_id, object_element->Attribute("id"), strtod(object_element->FirstChildElement("x")->GetText(),NULL), strtod(object_element->FirstChildElement("y")->GetText(),NULL), strtod(object_element->FirstChildElement("x_tiles")->GetText(),NULL), strtod(object_element->FirstChildElement("y_tiles")->GetText(),NULL),strtod(object_element->FirstChildElement("rotation")->GetText(),NULL),object_element->FirstChildElement("type")->GetText());
                getSector(current_sector_id)->floors[object_element->Attribute("id")]->setRotationalVelocity(strtod(object_element->FirstChildElement("rotational_velocity")->GetText(),NULL));
                getSector(current_sector_id)->floors[object_element->Attribute("id")]->setVelocity(strtod(object_element->FirstChildElement("x_velocity")->GetText(),NULL),strtod(object_element->FirstChildElement("y_velocity")->GetText(),NULL));
            }
        }
        for(TiXmlElement* object_element = sector_element->FirstChildElement("Connector"); object_element!= NULL; object_element = object_element->NextSiblingElement("Connector"))
        {
            makeNewConnector(current_sector_id, object_element->Attribute("id"), object_element->FirstChildElement("type")->GetText(), object_element->FirstChildElement("A")->GetText(), object_element->FirstChildElement("B")->GetText());
            getSector(current_sector_id)->getEnt(object_element->FirstChildElement("A")->GetText())->connectors.insert(object_element->Attribute("id"));
            getSector(current_sector_id)->getEnt(object_element->FirstChildElement("B")->GetText())->connectors.insert(object_element->Attribute("id"));
        
            if(object_element->FirstChildElement("relative_x") != NULL){

                Sprite build_sprite;
                build_sprite.setTexture(getProperties(object_element->FirstChildElement("type")->GetText())->sheet, true);
                IntRect build_rect = build_sprite.getTextureRect();
                build_sprite.setOrigin(build_rect.width/2.0,build_rect.height/2.0);
                getSector(current_sector_id)->connectors[object_element->Attribute("id")]->setSprite(build_sprite);
                getSector(current_sector_id)->connectors[object_element->Attribute("id")]->relative_position = Vector2f(strtod(object_element->FirstChildElement("relative_x")->GetText(),NULL),strtod(object_element->FirstChildElement("relative_y")->GetText(),NULL));
                getSector(current_sector_id)->connectors[object_element->Attribute("id")]->relative_rotation = strtod(object_element->FirstChildElement("relative_r")->GetText(),NULL);
            }
        }
        for(TiXmlElement* object_element = sector_element->FirstChildElement("Item"); object_element!= NULL; object_element = object_element->NextSiblingElement("Item"))
        {
            registerNewItem(current_sector_id, object_element->Attribute("id"), object_element->FirstChildElement("type")->GetText());
            //getSector(current_sector_id)->items[object_element->Attribute("id")]->storage_id = object_element->FirstChildElement("storage_id")->GetText();
        }
        for(TiXmlElement* object_element = sector_element->FirstChildElement("Chip"); object_element!= NULL; object_element = object_element->NextSiblingElement("Chip"))
        {
            registerNewItem(current_sector_id, object_element->Attribute("id"), object_element->FirstChildElement("type")->GetText());
            //getSector(current_sector_id)->items[object_element->Attribute("id")]->storage_id = object_element->FirstChildElement("storage_id")->GetText();
            for(TiXmlElement* sub_element = object_element->FirstChildElement("var_num"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("var_num"))
            {
                getSector(current_sector_id)->chips[object_element->Attribute("id")].lock()->var_num[int(strtod(sub_element->Attribute("id"),NULL))] = strtod(sub_element->GetText(),NULL);
            }
            for(TiXmlElement* sub_element = object_element->FirstChildElement("var_str"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("var_str"))
            {
                getSector(current_sector_id)->chips[object_element->Attribute("id")].lock()->var_str[int(strtod(sub_element->Attribute("id"),NULL))] = sub_element->GetText();
            }
        }
        for(TiXmlElement* object_element = sector_element->FirstChildElement("Rope"); object_element!= NULL; object_element = object_element->NextSiblingElement("Rope"))
        {
            getSector(current_sector_id)->ropes[object_element->Attribute("id")] = make_shared<Rope>(current_sector_id, object_element->Attribute("id"), object_element->FirstChildElement("type")->GetText());
            for(TiXmlElement* sub_element = object_element->FirstChildElement("node"); sub_element!= NULL; sub_element = sub_element->NextSiblingElement("node")){
                string attached_id;
                if(sub_element->FirstChildElement("attached")->GetText()==NULL){
                    attached_id = "";
                }
                else{
                    attached_id = sub_element->FirstChildElement("attached")->GetText();
                }
                getSector(current_sector_id)->ropes[object_element->Attribute("id")]->addExplicitNode(Vector2f(strtod(sub_element->FirstChildElement("x")->GetText(),NULL),strtod(sub_element->FirstChildElement("y")->GetText(),NULL)),Vector2f(strtod(sub_element->FirstChildElement("x_velocity")->GetText(),NULL),strtod(sub_element->FirstChildElement("y_velocity")->GetText(),NULL)),attached_id, Vector2f(strtod(sub_element->FirstChildElement("attachment_x")->GetText(),NULL),strtod(sub_element->FirstChildElement("attachment_y")->GetText(),NULL)));

            }
        }
    }
    for(map<string,shared_ptr<Sector> >::iterator parser = sectors.begin(); parser!=sectors.end(); parser++){
        parser->second->resortAllObjectsIntoBonds();
    }

    TiXmlDocument changes_doc("changes/index.xml");
    changes_doc.LoadFile();

    for(TiXmlElement* changes_element = changes_doc.FirstChildElement(); changes_element != NULL; changes_element = changes_element->NextSiblingElement())
    {
        string change_id = changes_element->Attribute("id");
        double year = strtod(changes_element->FirstChildElement("year")->GetText(), NULL);
        double month = strtod(changes_element->FirstChildElement("month")->GetText(), NULL);
        double day = strtod(changes_element->FirstChildElement("day")->GetText(), NULL);
        double hour = strtod(changes_element->FirstChildElement("hour")->GetText(), NULL);
        double minute = strtod(changes_element->FirstChildElement("minute")->GetText(), NULL);
        double second = strtod(changes_element->FirstChildElement("second")->GetText(), NULL);

        if(changes_element->FirstChildElement("relative_year") != NULL){
            double relative_year = strtod(changes_element->FirstChildElement("relative_year")->GetText(), NULL);
            double relative_month = strtod(changes_element->FirstChildElement("relative_month")->GetText(), NULL);
            double relative_day = strtod(changes_element->FirstChildElement("relative_day")->GetText(), NULL);
            double relative_hour = strtod(changes_element->FirstChildElement("relative_hour")->GetText(), NULL);
            double relative_minute = strtod(changes_element->FirstChildElement("relative_minute")->GetText(), NULL);
            double relative_second = strtod(changes_element->FirstChildElement("relative_second")->GetText(), NULL);
            changes[change_id] = make_shared<Change>(change_id, Date(month,day,year,hour,minute,second), Date(relative_month,relative_day,relative_year,relative_hour,relative_minute,relative_second));
        }
        else{
            changes[change_id] = make_shared<Change>(change_id, Date(month,day,year,hour,minute,second));
        }

        for(TiXmlElement* prereq_element = changes_element->FirstChildElement("prereq"); prereq_element != NULL; prereq_element = prereq_element->NextSiblingElement("prereq")){
            changes[change_id]->prereq_events[prereq_element->GetText()] = false;
        }
    }

    return true;
}

//
//void sendEntireWorld(TcpSocket &socket){
//
//    for(map<string, shared_ptr<Sector> >::iterator parser = world.sectors.begin(); parser != world.sectors.end(); parser++){
//        shared_ptr<Sector> sector = parser->second;
//
//        Packet sector_header;
//        sector_header << "SECTOR" << sector->id << sector->radius << sector->backdrop_tx_path;
//        socket.send(sector_header);
//
//        for(map<string, shared_ptr<Floor> >::iterator i = sector->floors.begin(); i != sector->floors.end(); i++) //run per-loop operations on floor
//        {
//            Packet packet;
//            packet << i->second->properties->name << i->second->id << double(i->second->coords.x) << double(i->second->coords.y);
//            socket.send(packet);
//        }
//        for(map<string, shared_ptr<Entity> >::iterator i = sector->ents.begin(); i != sector->ents.end(); i++) //run per-loop operations on object
//        {
//            Packet packet;
//            packet << i->second->properties->name << i->second->id << double(i->second->coords.x) << double(i->second->coords.y);
//            socket.send(packet);
//        }
//    }
//}
//
//void receiveEntireWorld(TcpSocket &socket){
//
//    world = World();
//
//    string sector_id;
//
//
//    socket.setBlocking(false);
//    Clock timeout_timer;
//
//    while(true){
//
//        Packet packet;
//
//        double timeout = 0;
//        timeout_timer.restart();
//        while(timeout < 1){
//            if(socket.receive(packet) == TcpSocket::Done){
//                break;
//            }
//            timeout += timeout_timer.getElapsedTime().asSeconds();
//            timeout_timer.restart();
//        }
//        if(timeout >= 1){
//            break;
//        }
//
//        string type;
//        packet >> type;
//
//        if(type == "SECTOR"){
//
//            double radius;
//            string texture;
//            packet >> sector_id >> radius >> texture;
//            world.loadSector(sector_id, radius, texture);
//        }
//        else{
//
//            string classification = getProperties(type)->classification;
//
//
//            if(classification == "ENTITY"){
//
//                string id;
//                double x;
//                double y;
//                packet >> id >> x >> y;
//
//                world.loadEntity(sector_id, id, Vector2f(x, y), type, 0, Vector2f(0,0), 0);
//            }
//            if(classification == "FLOOR"){
//
//                string id;
//                double x;
//                double y;
//                packet >> id >> x >> y;
//
//              //  world.loadFloor(sector_id, id, Vector2f(x, y), type, 0, Vector2f(10,0), 0);
//
//            }
//            if(classification == "MACHINE"){
//
//            }
//            if(classification == "CONTAINER"){
//
//            }
//            if(classification == "INTERFACE"){
//
//            }
//            if(classification == "TRANSFER"){
//
//            }
//            if(classification == "ITEM"){
//
//            }
//            if(classification == "SCRAP"){
//
//            }
//            if(classification == "ROPE"){
//
//            }
//        }
//    }
//}
//
//
//Map::Map()
//{
//    backdrop.setTexture(*tx_groups["map"].getTexture(),true);
//    backdrop.setOrigin(tx_groups["map"].getRect()->width/2,tx_groups["map"].getRect()->height/2);
//    backdrop.setPosition(map_center.x,map_center.y);
//
//    map_window = FloatRect(0,0,600,600);
//    map_window.top = map_center.x - map_window.width/2;
//    map_window.left = map_center.y - map_window.height/2;
//
//    for(map<string,shared_ptr<Sector> >::iterator parser = world.sectors.begin(); parser!=world.sectors.end(); parser++)
//    {
//        shared_ptr<Sector> sector = parser->second;
//        Sprite new_sprite;
//        new_sprite.setTexture(*tx_groups["sector"].getTexture(),true);
//        new_sprite.setOrigin(tx_groups["sector"].getRect()->width/2,tx_groups["sector"].getRect()->height/2);
//        new_sprite.setPosition(map_center.x + sector->universe_coords.x*getScale(),map_center.y + sector->universe_coords.y*getScale());
//        sector_sprites[parser->first] = new_sprite;
//
//        CircleShape new_circle;
//        new_circle.setRadius(sector->radius*getScale());
//        new_circle.setOrigin(sector->radius*getScale(),sector->radius*getScale());
//        new_circle.setFillColor(Color(0,0,0,0));
//        new_circle.setOutlineColor(Color(0,255,0,155));
//        new_circle.setOutlineThickness(2);
//        new_circle.setPosition(map_center.x + sector->universe_coords.x*getScale(),map_center.y + sector->universe_coords.y*getScale());
//        sector_circles[parser->first] = new_circle;
//    }
//}
//
//void Map::toggle()
//{
//    enabled = not enabled;
//}
//
//double Map::getScale()
//{
//    return scale/1000;
//}
//
//void Map::update()
//{
//    if(not enabled){return;}
//
//    map_center.x = window.getSize().x/2;
//    map_center.y = window.getSize().y/2;
//
//    backdrop.setPosition(map_center.x,map_center.y);
//    map_window.left = map_center.x - (map_window.width/2);
//    map_window.top = map_center.y - (map_window.height/2);
//
//    for(map<string,shared_ptr<Sector> >::iterator parser = world.sectors.begin(); parser!=world.sectors.end(); parser++)
//    {
//        shared_ptr<Sector> sector = parser->second;
//        if(sector_sprites.count(parser->first)==0)
//        {
//            Sprite new_sprite;
//            new_sprite.setTexture(*tx_groups["sector"].getTexture(),true);
//            new_sprite.setOrigin(tx_groups["sector"].getRect()->width/2,tx_groups["sector"].getRect()->height/2);
//            new_sprite.setPosition(map_center.x + sector->universe_coords.x*getScale(),map_center.y + sector->universe_coords.y*getScale());
//            sector_sprites[parser->first] = new_sprite;
//
//            CircleShape new_circle;
//            new_circle.setRadius(sector->radius*getScale());
//            new_circle.setOrigin(sector->radius*getScale(),sector->radius*getScale());
//            new_circle.setFillColor(Color(0,0,0,0));
//            new_circle.setOutlineColor(Color(0,255,0,155));
//            new_circle.setOutlineThickness(2);
//            new_circle.setPosition(map_center.x + sector->universe_coords.x*getScale(),map_center.y + sector->universe_coords.y*getScale());
//            sector_circles[parser->first] = new_circle;
//        }
//        else
//        {
//            sector_sprites[parser->first].setPosition(map_center.x + sector->universe_coords.x*getScale(),map_center.y + sector->universe_coords.y*getScale());
//            sector_circles[parser->first].setRadius(sector->radius*getScale());
//            sector_circles[parser->first].setOrigin(sector->radius*getScale(),sector->radius*getScale());
//            sector_circles[parser->first].setPosition(map_center.x + sector->universe_coords.x*getScale(),map_center.y + sector->universe_coords.y*getScale());
//        }
//    }
//
//    set<string> to_delete;
//    for(map<string,Sprite>::iterator parser = sector_sprites.begin(); parser!=sector_sprites.end(); parser++)
//    {
//        if(world.sectors.count(parser->first)==0)
//        {
//            to_delete.insert(parser->first);
//        }
//    }
//    for(set<string>::iterator parser = to_delete.begin(); parser != to_delete.end(); parser++)
//    {
//        sector_sprites.erase(*parser);
//        sector_circles.erase(*parser);
//    }
//}
//
//void Map::draw()
//{
//    if(not enabled){return;}
//
//    window.draw(backdrop);
//    for(map<string,Sprite>::iterator parser = sector_sprites.begin(); parser!=sector_sprites.end(); parser++)
//    {
//        Sprite sprite = parser->second;
//        if(sprite.getGlobalBounds().intersects(map_window))
//        {
//            window.draw(sprite);
//        }
//    }
//    for(map<string,CircleShape>::iterator parser = sector_circles.begin(); parser!=sector_circles.end(); parser++)
//    {
//        CircleShape sprite = parser->second;
//        if(sprite.getGlobalBounds().intersects(map_window))
//        {
//            window.draw(sprite);
//        }
//    }
//
//}
//
//shared_ptr<Fadeout> Sector::generateFadeout()
//{
//    shared_ptr<Fadeout> new_fadeout = make_shared<Fadeout>();
//
//   // new_fadeout->background = make_shared<Sprite>(background);
//    for(map<string, shared_ptr<Floor> >::iterator parser = floors.begin(); parser!=floors.end(); parser++)
//    {
//        shared_ptr<Entity> e = shared_ptr<Entity>(parser->second);
//        new_fadeout->sprites.insert(make_shared<Sprite>(e->sprite));
//    }
//    for(map<string, shared_ptr<Entity> >::iterator parser = ents.begin(); parser!=ents.end(); parser++)
//    {
//        shared_ptr<Entity> e = parser->second;
//        new_fadeout->sprites.insert(make_shared<Sprite>(e->sprite));
//    }
//
//    return new_fadeout;
//}
//
//void sendToTheDeep(shared_ptr<Entity> start_entity)
//{
//    set<shared_ptr<Entity> > leaving_group;
//    set<string> leaving_bonds;
//    set<string> leaving_connectors;
//
//    set<shared_ptr<Entity> > new_leavers;
//    new_leavers.insert(start_entity);
//
//    while(true) //collect a set of leaving ents in leaving_group
//    {
//        if(new_leavers.size()==0){break;}
//
//        set<shared_ptr<Entity> > pending_inserts;
//        for(set<shared_ptr<Entity> >::iterator parser = new_leavers.begin(); parser!=new_leavers.end(); parser++)
//        {
//            shared_ptr<Entity> current_ent = *parser;
//            //add all adjoined entitites
//            for(set<string>::iterator w_floor = current_ent->floors.begin(); w_floor!=current_ent->floors.end(); w_floor++)//add floors being stood on
//            {
//                shared_ptr<Entity> floor = getEnt(*w_floor);
//                if(leaving_group.count(floor)==0 and new_leavers.count(floor)==0)
//                {
//                    pending_inserts.insert(floor);
//                }
//            }
//            for(map<string, shared_ptr<Entity> >::iterator c_ent = server.ents.begin(); c_ent!=server.ents.end(); c_ent++)
//            {
//                if(leaving_group.count(c_ent->second)!=0 or new_leavers.count(c_ent->second)!=0)
//                {
//                    continue;
//                }
//                Vector2f dist = c_ent->second->coords-current_ent->coords;
//                double h = hypot(dist.x,dist.y);
//                if(h<=c_ent->second->collision_radius+current_ent->collision_radius+100)
//                {
//                     pending_inserts.insert(c_ent->second);
//                }
//            }
//            for(map<string, shared_ptr<Floor> >::iterator c_floor = server.floors.begin(); c_floor!=server.floors.end(); c_floor++)
//            {
//                if(leaving_group.count(c_floor->second)!=0 or new_leavers.count(c_floor->second)!=0)
//                {
//                    continue;
//                }
//                Vector2f dist = c_floor->second->coords-current_ent->coords;
//                double h = hypot(dist.x,dist.y);
//                if(h<=c_floor->second->collision_radius+current_ent->collision_radius+100)
//                {
//                     pending_inserts.insert(c_floor->second);
//                }
//            }
//            if(current_ent->bonded)//add ents bonded to this one
//            {
//                leaving_bonds.insert(current_ent->bond);
//                shared_ptr<Bond> current_bond = server.bonds[current_ent->bond];
//                for(vector<weak_ptr<Entity> >::iterator w_ent = current_bond->members.begin(); w_ent!=current_bond->members.end(); w_ent++)
//                {
//                    shared_ptr<Entity> ent = w_ent->lock();
//                    if(leaving_group.count(ent)==0 and new_leavers.count(ent)==0)
//                    {
//                        pending_inserts.insert(ent);
//                    }
//                }
//            }
//        }
//        leaving_group.insert(new_leavers.begin(),new_leavers.end());
//        new_leavers = pending_inserts;
//        pending_inserts.clear();
//    }
//    for(map<string,shared_ptr<Connector> >::iterator parser = server.connectors.begin(); parser!=server.connectors.end(); parser++)
//    {
//        shared_ptr<Entity> A = parser->second->A.lock();
//        shared_ptr<Entity> B = parser->second->B.lock();
//        if(leaving_group.count(A)!=0 or leaving_group.count(A)!=0)
//        {
//            leaving_connectors.insert(parser->first);
//        }
//    }
//
//    for(set<shared_ptr<Entity> >::iterator parser = leaving_group.begin(); parser!=leaving_group.end(); parser++)
//    {
//        //if(server.collision_grid.limbo.count(parser->get())==0 and server.floor_grid.limbo.count(parser->get())==0)
//       // {
//         //   //not all leaving members are out of the sector yet
//        //    return;
//       // }
//    }
//    ////////////ACTUALLY SEND THE SET INTO DEEP SPACE//////////////////////////
//
//
//
//
//
//    weak_ptr<Entity> farthest;
//    double farthest_dist = 0;
//    for(set<shared_ptr<Entity> >::iterator parser = leaving_group.begin(); parser!=leaving_group.end(); parser++)
//    {
//        shared_ptr<Entity> current_ent = *parser;
//        double h = hypot(current_ent->coords.x,current_ent->coords.y);
//        if(h>farthest_dist)
//        {
//            farthest = current_ent;
//            farthest_dist = h;
//        }
//    }
//
//    double radius = 0;
//    for(set<shared_ptr<Entity> >::iterator parser = leaving_group.begin(); parser!=leaving_group.end(); parser++)
//    {
//        shared_ptr<Entity> current_ent = *parser;
//        double h = hypot(current_ent->coords.x - farthest.lock()->coords.x,current_ent->coords.y - farthest.lock()->coords.y);
//        if(h>radius)
//        {
//            radius = h;
//        }
//    }
//    radius += 200;
//
//    string new_sector_id = createUniqueId();
//    shared_ptr<Sector> new_sector = make_shared<Sector>(new_sector_id, radius, "bg_deep_space");
//    new_sector->universe_coords = farthest.lock()->getUniverseCoords();
//    new_sector->deep = true;
//    new_sector->anchor_ent = start_entity->id;
//    world.sectors[new_sector_id] = new_sector;
//
//
//    for(set<shared_ptr<Entity> >::iterator parser = leaving_group.begin(); parser!=leaving_group.end(); parser++)
//    {
//        shared_ptr<Entity> current_ent = *parser;
//        current_ent->coords = current_ent->getUniverseCoords()-new_sector->universe_coords;
//    }
//    for(set<shared_ptr<Entity> >::iterator parser = leaving_group.begin(); parser!=leaving_group.end(); parser++)
//    {
//        shared_ptr<Entity> current_ent = *parser;
//        for(map<int,string>::iterator item_parser = current_ent->contents.begin(); item_parser!=current_ent->contents.end(); item_parser++)
//        {
//            new_sector->items[item_parser->second] = server.items[item_parser->second];
//        }
//        if(current_ent->classification==FLOOR)
//        {
//            new_sector->floors[current_ent->id] = server.floors[current_ent->id];
//        }
//        else
//        {
//            if(current_ent->classification==PLAYER)
//            {
//                new_sector->people[current_ent->id] = server.people[current_ent->id];
//            }
//            if(current_ent->classification==CONTAINER)
//            {
//                new_sector->containers[current_ent->id] = server.containers[current_ent->id];
//            }
//            if(current_ent->classification==MACHINE)
//            {
//                new_sector->machines[current_ent->id] = server.machines[current_ent->id];
//            }
//            new_sector->ents[current_ent->id] = server.ents[current_ent->id];
//        }
//    }
//    for(set<string>::iterator parser = leaving_bonds.begin(); parser!= leaving_bonds.end(); parser++)
//    {
//        server.bonds[*parser]->center = server.bonds[*parser]->getUniverseCoords()-new_sector->universe_coords;
//        new_sector->bonds[*parser] = server.bonds[*parser];
//    }
//    for(set<string>::iterator parser = leaving_connectors.begin(); parser!=leaving_connectors.end(); parser++)
//    {
//        new_sector->connectors[*parser] = server.connectors[*parser];
//    }
//
//    ///////REMOVE FROM THIS SECTOR/////////
//    for(set<shared_ptr<Entity> >::iterator parser = leaving_group.begin(); parser!=leaving_group.end(); parser++)
//    {
//        shared_ptr<Entity> current_ent = *parser;
//        for(map<int,string>::iterator item_parser = current_ent->contents.begin(); item_parser!=current_ent->contents.end(); item_parser++)
//        {
//            server.items.erase(item_parser->second);
//        }
//        if(current_ent->classification==FLOOR)
//        {
//            server.floors.erase(current_ent->id);
//        }
//        else
//        {
//            if(current_ent->classification==PLAYER)
//            {
//                server.people.erase(current_ent->id);
//            }
//            if(current_ent->classification==CONTAINER)
//            {
//                server.containers.erase(current_ent->id);
//            }
//            if(current_ent->classification==MACHINE)
//            {
//                server.machines.erase(current_ent->id);
//            }
//            server.ents.erase(current_ent->id);
//        }
//    }
//    for(set<string>::iterator parser = leaving_bonds.begin(); parser!= leaving_bonds.end(); parser++)
//    {
//        server.bonds.erase(*parser);
//    }
//    for(set<string>::iterator parser = leaving_connectors.begin(); parser!=leaving_connectors.end(); parser++)
//    {
//        server.connectors.erase(*parser);
//    }
//
//    shared_ptr<Fadeout> f1 = new_sector->generateFadeout();
//    f1->convertToUniversal(new_sector->id);
//    f1->convertToLocal(world.sector);
//    shared_ptr<Fadeout> f2 = server.generateFadeout();
//    f2->convertToUniversal(world.sector);
//    f2->convertToLocal(new_sector->id);
//    server.fadeouts.insert(f1);
//    new_sector->fadeouts.insert(f2);
//
//
//    if(server.people.count(server.player_id)==0)
//    {
//        server.openSector(new_sector_id);
//    }
//
//
//}
