#include "main.h"

using namespace sf;
using namespace std;

extern World world;
extern RenderWindow window;

Connector::Connector(){}

Connector::Connector(string new_sector_id, string c_id, string c_type ,string a, string b)
{
    sector_id = new_sector_id;
    id = c_id;
    type = c_type;
    a_id = a;
    b_id = b;

    if(propertiesExistFor(type)){
        classification = getProperties(type)->classification;
    }

}

void Connector::light(double amount)
{
    light_level += amount;
    if(light_level + getSector(sector_id)->ambient_light > 255){light_level = 255 - getSector(sector_id)->ambient_light;}
}

void Connector::setSprite(Sprite new_sprite){

    has_sprite = true;

    shared_ptr<Entity> ob = getSector(sector_id)->getEnt(a_id);
    double Q = ob->sprite.getRotation()*M_PI/180; //rotation in radians

    Vector2f position = new_sprite.getPosition() - ob->getPosition();
    relative_position = Vector2f(position.x*cos(-Q)-position.y*sin(-Q),position.x*sin(-Q)+position.y*cos(-Q));
    relative_rotation = new_sprite.getRotation() - ob->sprite.getRotation();
    if(relative_rotation < 0){relative_rotation += 360; }

    sprite = new_sprite;
    sprite.setColor(Color(255,255,255,255));

    collision_radius = hypot(sprite.getTextureRect().width/2, sprite.getTextureRect().height/2);

    updateSprite();

}

void Connector::updateSprite(){

    if(!has_sprite){return;}

    shared_ptr<Entity> ob = getSector(sector_id)->getEnt(a_id);

    double Q = ob->sprite.getRotation() * (M_PI/180);
    Vector2f sprite_position = ob->getPosition() + Vector2f(relative_position.x*cos(Q)-relative_position.y*sin(Q),relative_position.x*sin(Q)+relative_position.y*cos(Q));
    double sprite_rotation = relative_rotation + ob->sprite.getRotation();

    sprite.setPosition(sprite_position);
    sprite.setRotation(sprite_rotation);

    sprite.setColor(Color((getSector(sector_id)->ambient_light+light_level),(getSector(sector_id)->ambient_light+light_level),(getSector(sector_id)->ambient_light+light_level)));
    light_level = 0;
}

void Connector::draw(){

    if(!has_sprite){return;}
    window.draw(sprite);
}

void Connector::cleanup(){


    if(shared_ptr<Entity> ent = getSector(sector_id)->getEnt(a_id)){
        ent->connectors.erase(id);
    }
    if(shared_ptr<Entity> ent = getSector(sector_id)->getEnt(b_id)){
        ent->connectors.erase(id);
    }

    queued_for_deletion = true;
   // getSector(sector_id)->resortIntoBonds(temp_set);
}
//
//void showConnectors(set<string> ent_ids, ConnectorType t, Color color)
//{
//    vector<weak_ptr<Connector> > connectors;
//    for(set<string>::iterator parser = ent_ids.begin(); parser!=ent_ids.end(); parser++)
//    {
//        if(shared_ptr<Entity> e = getEnt(id, *parser)){
//            for(set<string>::iterator parser2 = e->connectors.begin(); parser2!=e->connectors.end(); parser2++)
//            {
//                shared_ptr<Connector> c = getSector(server.sector_id)->connectors[*parser2];
//                if(c->type==t)
//                {
//                    connectors.push_back(weak_ptr<Connector>(c));
//                }
//            }
//        }
//
//    }
//
//    if(t==CT_Weld)
//    {
//        for(vector<weak_ptr<Connector> >::iterator parser = connectors.begin(); parser!=connectors.end(); parser++)
//        {
//            shared_ptr<Connector> c = parser->lock();
//            weak_ptr<Entity> A = c->A;
//            weak_ptr<Entity> B = c->B;
//            overlayBetween(A,B,color,"Welded");
//        }
//    }
//    else if(t==CT_Rivet)
//    {
//        for(vector<weak_ptr<Connector> >::iterator parser = connectors.begin(); parser!=connectors.end(); parser++)
//        {
//            shared_ptr<Connector> c = parser->lock();
//            weak_ptr<Entity> A = c->A;
//            weak_ptr<Entity> B = c->B;
//            overlayFlippedBetween(A,B,color,"Welded");
//        }
//    }
//
//
//}

Bond::Bond(string new_sector_id)
{
    sector_id = new_sector_id;
    id = createUniqueId();
    center = Vector2f(0,0);
}

Bond::Bond(string new_sector_id,string cid)
{
    sector_id = new_sector_id;
    id = cid;
    center = Vector2f(0,0);
}

Vector2f Bond::getUniverseCoords()
{
    return center+getSector(sector_id)->universe_coords;
}

set<string> Bond::getGroupWith(string ent_id){

    for(vector<set<string> >::iterator parser = subgroups.begin(); parser != subgroups.end(); parser++){

        if(parser->count(ent_id) != 0){
            return *parser;
        }
    }
}


void Bond::displaceGroupContaining(string ent_id, Vector2f disp){

    double Q = rotation*(M_PI/180);
    for(vector<set<string> >::iterator parser = subgroups.begin(); parser != subgroups.end(); parser++){

        if(parser->count(ent_id) != 0){
            for(set<string>::iterator ent_parser = parser->begin(); ent_parser != parser->end(); ent_parser++){

                getSector(sector_id)->getEnt(*ent_parser)->coords = getSector(sector_id)->getEnt(*ent_parser)->coords + disp;
                if(getSector(sector_id)->floors.count(*ent_parser)!=0){
                    for(set<string>::iterator parser = getSector(sector_id)->floors[*ent_parser]->passengers.begin(); parser != getSector(sector_id)->floors[*ent_parser]->passengers.end(); parser++){
                        if(members.count(*parser)==0){
                            getSector(sector_id)->getEnt(*parser)->displace(disp.x,disp.y);
                        }
                    }
                }

            }
            break;
        }
    }
   // applyBond(0.00000000000000000001);
    resetProperties();
    applyBond(0.000000000000000001);
}

void Bond::addMember(string new_member_id)
{
    if(shared_ptr<Entity> new_member = getSector(sector_id)->getEnt(new_member_id)){

        Vector2f impulse = new_member->getMomentum();
        double new_rot_v = ((rot_p + new_member->getRotationalMomentum())/(getInertia() + new_member->getInertia()));

        new_member->setVelocity(0,0);
        new_member->setRotationalVelocity(0);

        members.insert(new_member_id); //members stores the ids of member entities

        resetProperties();

        p =  p + impulse;
        rot_p = new_rot_v*getInertia();

        new_member->bonded = true;
        new_member->bond = id;

        if(new_member->prop){ moveable = false; }

        if(new_member->properties->classification=="FLOOR"){floor_attached=true;}
    }
}

bool Bond::inBond(string e_id)
{
    if(members.count(e_id)!=0){
        return true;
    }
    return false;
}

void Bond::removeMember(string leaving_member_id)
{
    if(shared_ptr<Entity> leaving_member = getSector(sector_id)->getEnt(leaving_member_id)){

        leaving_member->bond = "";
        leaving_member->bonded = false;

        double rot_v = rot_p/getInertia();
        leaving_member->setVelocity(p.x/mass,p.y/mass);
        leaving_member->setRotationalVelocity(rot_v);

        members.erase(leaving_member_id);
        positions.erase(leaving_member_id);
        orientations.erase(leaving_member_id);

        resetProperties();
        p = p - leaving_member->p;
        rot_p = rot_v*getInertia();
       // p = p - leaving_member->p;
      //  rot_p = rot_p - leaving_member->rot_p;



        floor_attached = false; //check if bond still has a floor in it
        for(set<string>::iterator parser = members.begin(); parser!=members.end(); parser++){
            if(shared_ptr<Entity> e = getSector(sector_id)->getEnt(*parser)){
                if(e->properties->classification=="FLOOR"){
                    floor_attached = true;
                    break;
                }
            }
        }

        moveable = true;
        for(set<string>::iterator parser = members.begin(); parser!=members.end(); parser++){
            if(shared_ptr<Entity> e = getSector(sector_id)->getEnt(*parser)){
                if(e->prop){
                    moveable = false;
                    break;
                }
            }
        }
    }
}

void Bond::resetProperties()
{
    //determines the center of mass of the bond, and resets positions and orientations of objects relative to that.

    double totalmass = 0;
    Vector2f c_o_m = Vector2f(0,0);

    for(set<string>::iterator i = members.begin(); i != members.end(); i++){
        if(shared_ptr<Entity> e = getSector(sector_id)->getEnt(*i)){
            totalmass += e->getMyMass();
            c_o_m = c_o_m + (e->getPosition() * e->getMyMass());
        }
    }

    if(totalmass<=0){return;}

    c_o_m = c_o_m/(totalmass);

    mass = totalmass;
    center = c_o_m;

    bond_radius = 0;
    rotation = 0;
    for(set<string>::iterator i = members.begin(); i != members.end(); i++){
        if(shared_ptr<Entity> e = getSector(sector_id)->getEnt(*i)){
            positions[*i] = e->getPosition() - center; //stores relative positions of entities to bond center at join

            double possible_max_radius = hypot(positions[*i].x,positions[*i].y) + e->collision_radius;
            if(possible_max_radius > bond_radius){
                bond_radius = possible_max_radius;
            }
            orientations[*i] = e->getRotation()-rotation; //orientations stores the relative orientation to the bond an entity had when it was joined
        }
    }
}

void Bond::applyBond(double dt)
{
    
    backup_p = p;
    if(mass<=0){return;}
    if(members.size()==0){return;}

    if(moveable){
        center = center + ((p/mass)*dt);
        rotation += ((rot_p/getInertia())*dt);
    }
    else{
        p = Vector2f(0,0);
        rot_p = 0;
    }

    double Q = rotation*M_PI/180; //rotation in radians


    for(set<string>::iterator i = members.begin(); i != members.end(); i++){
        if(shared_ptr<Entity> e = getSector(sector_id)->getEnt(*i)){
            Vector2f newPosition = center + Vector2f(positions[*i].x*cos(Q)-positions[*i].y*sin(Q),positions[*i].x*sin(Q)+positions[*i].y*cos(Q));
            e->setPosition(newPosition.x,newPosition.y);
            e->setRotation(rotation + orientations[*i]);
            e->sprite.setPosition(newPosition);
            e->sprite.setRotation(rotation + orientations[*i]);
        }
    }
}

//void Bond::showAllConnectors()
//{
//    showConnectors(members,CT_Weld,Color(200,200,0,155));
//    showConnectors(members,CT_Rivet,Color(150,150,0,155));
//}
//
//void Bond::showAllConnectors(Color color_override)
//{
//    showConnectors(members,CT_Weld,color_override);
//    showConnectors(members,CT_Rivet,color_override);
//}

void Bond::setVelocity(double x, double y)
{
    p = Vector2f(x*mass,y*mass);
}

Vector2f Bond::getVelocity()
{
    return Vector2f(p.x/mass,p.y/mass);
}
void Bond::setRotationalVelocity(double d)
{
    rot_p = d*getInertia();
}

void Bond::setRotation(double d)
{
    rotation = d;
}

double Bond::getInertia(){

    return mass + ((convertToMeters(bond_radius)*3*mass));
}

void Bond::applyImpulse(double x, double y, Vector2f point, bool apply_spin)
{
    Vector2f dist = point-center;

    //angular impulse
    if(abs(dist.x)<1){dist.x = 0;}
    if(abs(dist.y)<1){dist.y = 0;}

    Vector2f normal = getPerpendicularAxis(Vector2f(dist.x,dist.y));
    double th = (x*normal.x + y*normal.y)*convertToMeters(hypot(dist.x,dist.y));

    Vector2f vec_p = Vector2f(x,y);

    if(apply_spin){
        rot_p += th;
     //   vec_p = Vector2f(x-(x*normal.x),y-(y*normal.y));
    }


    p = p + vec_p;


}

void Bond::applyRotationalImpulseOnly(double x, double y, Vector2f point)
{

    Vector2f dist = point-center;

    //angular impulse
    if(abs(dist.x)<1){dist.x = 0;}
    if(abs(dist.y)<1){dist.y = 0;}

    Vector2f normal = getPerpendicularAxis(Vector2f(dist.x,dist.y));
    double th = (x*normal.x + y*normal.y)*convertToMeters(hypot(dist.x,dist.y));


        rot_p += th;
}

void Bond::createSubgroups(){

    subgroups.clear();

    set<string> already_grouped;
    for(set<string>::iterator parser = members.begin(); parser != members.end(); parser++){
        if(already_grouped.count(*parser) != 0){ continue; }

        set<string> new_group;
        shared_ptr<Entity> ent = getSector(sector_id)->getEnt(*parser);

        new_group.insert(*parser);
        ent->getConnectedNoPiston(&new_group);
        for(set<string>::iterator parser_2 = new_group.begin(); parser_2 != new_group.end(); parser_2++){
            already_grouped.insert(*parser_2);
        }

        subgroups.push_back(new_group);
    }
}

void Bond::cleanup(){

    for(set<string>::iterator parser = members.begin(); parser!=members.end(); parser++){
        if(shared_ptr<Entity> e = getSector(sector_id)->getEnt(*parser)){
            e->bonded = false;
            e->bond = "";
            e->setVelocity(p.x/mass,p.y/mass);
            e->setRotationalVelocity(rot_p/getInertia());
        }
    }
}


string makeNewConnector(string new_sector_id, string c_id, string c_type, string a_id , string b_id){

    getSector(new_sector_id)->connectors[c_id] = make_shared<Connector>(new_sector_id,c_id,c_type,a_id,b_id);
    getSector(new_sector_id)->getEnt(a_id)->connectors.insert(c_id);
    getSector(new_sector_id)->getEnt(b_id)->connectors.insert(c_id);

    return c_id;
}

void makeNewBond(string sector_id, string b_id, set<string> new_members){

    getSector(sector_id)->bonds[b_id] = make_shared<Bond>(sector_id, b_id);
    for(set<string>::iterator parser = new_members.begin(); parser!=new_members.end(); parser++){
        getSector(sector_id)->bonds[b_id]->addMember(*parser);
    }
    getSector(sector_id)->bonds[b_id]->createSubgroups();
}


void makeNewBond(string sector_id, set<string> new_members){

    makeNewBond(sector_id, createUniqueId(), new_members);

}

