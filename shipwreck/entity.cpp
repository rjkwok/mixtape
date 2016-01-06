#include "main.h"

using namespace std;
using namespace sf;


map<string, shared_ptr<Properties> > properties_map;
extern World world;
extern RenderWindow window;

Properties::Properties(){}

double Properties::get(string key){

    return properties[key];
}

bool propertiesExistFor(string key){

    return (properties_map.count(key) != 0);
}

shared_ptr<Properties> getProperties(string key){

    return properties_map[key];
}

void loadAllProperties()
{
    TiXmlDocument doc("info.xml");
    doc.LoadFile();

    string sheets_path = "sheets\\";
    string icons_path = "icons\\";

    for(TiXmlElement* parser = doc.FirstChildElement("Type"); parser!=NULL; parser = parser->NextSiblingElement("Type"))
    {
        shared_ptr<Properties> loader = make_shared<Properties>();

        loader->name = parser->Attribute("name");
        loader->classification = parser->Attribute("class");

        loader->details = "";
        if(parser->FirstChildElement("details")!=NULL){
            loader->details = parser->FirstChildElement("details")->GetText();
        }
        if(parser->Attribute("sheet")!=NULL){
            loader->sheet.loadFromFile(sheets_path + parser->Attribute("sheet"));
        }
        if(parser->Attribute("icon")!=NULL){
            loader->icon.loadFromFile(icons_path + parser->Attribute("icon"));
        }

        for(TiXmlElement* property_parser = parser->FirstChildElement("property"); property_parser!=NULL; property_parser = property_parser->NextSiblingElement("property")){

            loader->properties[property_parser->Attribute("name")] = strtod(property_parser->GetText(),NULL);
        }

        properties_map[loader->name] = loader;
    }
}
//
//void generateScrapMaps(){
//
//
//        for(map<string, Info>::iterator entry = information.begin(); entry!=information.end(); entry++){
//
//            vector<pair<string, Vector2f> > composition = entry->second.composition;
//            if(composition.size()==0){continue;}
//
//            Sprite ent_test_sprite;
//            ent_test_sprite.setTexture(*tx_groups[entry->second.tx_group_key].getTexture(),true);
//            IntRect ent_test_r = ent_test_sprite.getTextureRect();
//            ent_test_sprite.setOrigin(ent_test_r.width/2,ent_test_r.height/2);
//            ent_test_sprite.setRotation(0);
//            ent_test_sprite.setPosition(0,0);
//            FloatRect ent_rect = ent_test_sprite.getGlobalBounds();
//
//            vector<Sprite> test_sprites;
//            for(vector<pair<string, Vector2f> >::iterator parser = composition.begin(); parser!=composition.end(); parser++){
//
//                Sprite sprite;
//                sprite.setTexture(*tx_groups[getInfo(parser->first).tx_group_key].getTexture(),true);
//                IntRect r = sprite.getTextureRect();
//                sprite.setOrigin(r.width/2,r.height/2);
//                sprite.setPosition(ent_rect.left,ent_rect.top);
//
//                test_sprites.push_back(sprite);
//            }
//
//
//            int index = 0;
//            for(vector<Sprite>::iterator parser = test_sprites.begin(); parser!=test_sprites.end(); parser++){
//
//                while(parser->getPosition().y<ent_rect.top+ent_rect.height){
//                    //////perform check
//                    FloatRect test_rect = parser->getGlobalBounds();
//                    //if fully inside
//                    if(test_rect.left>=ent_rect.left and test_rect.top>=ent_rect.top and test_rect.left+test_rect.width<=ent_rect.left+ent_rect.width and test_rect.top+test_rect.height<=ent_rect.top+ent_rect.height){
//                        //if not colliding with any other test sprites
//                        bool overlapping = false;
//                        for(vector<Sprite>::iterator parser_2 = test_sprites.begin(); parser_2!=parser; parser_2++){
//                            FloatRect test_rect_2 = parser_2->getGlobalBounds();
//                            if(test_rect.intersects(test_rect_2)){
//                                overlapping = true;
//                                break;
//                            }
//                        }
//                        if(not overlapping){
//                            //spot was found, done with this sprite!
//                            composition[index].second = Vector2f(parser->getPosition().x,parser->getPosition().y);
//                            break;
//                        }
//                    }
//                    //////////////
//
//                    parser->setPosition(parser->getPosition().x+2,parser->getPosition().y);
//                    if(parser->getPosition().x>ent_rect.left+ent_rect.width){
//                        parser->setPosition(ent_rect.left,parser->getPosition().y+2);
//                    }
//                }
//                index++;
//
//
//            }
//
//            entry->second.composition.clear();
//            for(vector<pair<string, Vector2f> >::iterator parser = composition.begin(); parser!=composition.end(); parser++){
//                if(parser->second.x!=-1000 and parser->second.y!=-1000){
//
//                    entry->second.composition.push_back(*parser);
//                }
//            }
//        }
//
//
//
//
//
//
//
//}

Animator::Animator(){
    play(0,0);
}
Vector2f Animator::getFrameTopLeft(int frame_index, Sprite &sprite){

    IntRect frame_window = sprite.getTextureRect();
    double x_index = (int(frame_index * frame_window.width) % int(sprite.getTexture()->getSize().x))/double(frame_window.width);
    double y_index = ((frame_index - x_index)*frame_window.width)/sprite.getTexture()->getSize().x;

    return Vector2f(x_index*frame_window.width, y_index*frame_window.height);
}

void Animator::play(int new_frame_index_1, int new_frame_index_2){

    frame_index_1 = new_frame_index_1;
    frame_index_2 = new_frame_index_2;

}


void Animator::update(double dt, Sprite &sprite){

    loop_completed = false;

    if(previous_frame_index_1 != frame_index_1 || previous_frame_index_2 != frame_index_2){
        //reset
        elapsed_loop_time = 0;
    }

    int current_frame;

    if(frame_index_1 < frame_index_2){
        current_frame = frame_index_1 + floor(fps*elapsed_loop_time);
        if(current_frame > frame_index_2){ //loops the loop and sets flag so rest of entity knows a loop completed this tick
            loop_completed = true;
            current_frame = frame_index_1;
            elapsed_loop_time = 0;
        }

    }
    else{
        current_frame = frame_index_1 - floor(fps*elapsed_loop_time);
        if(current_frame < frame_index_2){ //loops the loop and sets flag so rest of entity knows a loop completed this tick
            loop_completed = true;
            current_frame = frame_index_1;
            elapsed_loop_time = 0;
        }

    }

    IntRect current_window = sprite.getTextureRect();
    Vector2f new_window_coords = getFrameTopLeft(current_frame, sprite);
    if(current_window.top != new_window_coords.y || current_window.left != new_window_coords.x){
        //if frame has changed
        current_window.top = int(new_window_coords.y);
        current_window.left = int(new_window_coords.x);
        sprite.setTextureRect(current_window);
    }

    previous_frame_index_1 = frame_index_1;
    previous_frame_index_2 = frame_index_2;

    elapsed_loop_time += dt;
}

Entity::Entity(){}

Entity::Entity(string new_sector_id, string e_id, double x, double y, string type)
{
    constructEntity(new_sector_id, e_id, x, y, type);
}

void Entity::constructEntity(string new_sector_id, string e_id, double x, double y, string type)
{
    properties = getProperties(type);

    id = e_id;
    sprite.setTexture(properties->sheet, false);
    sprite.setTextureRect(IntRect(0,0,properties->get("width"),properties->get("height")));
    sprite.setOrigin(properties->get("width")/2,properties->get("height")/2);
    collision_radius = hypot(properties->get("width")/2,properties->get("height")/2);

    sector_id  = new_sector_id;
    coords = Vector2f(x,y);
    current_health = properties->get("max_health");

    sprite.setPosition(coords.x,coords.y);

    if(properties->classification == "CONTAINER"){

        setSlots(properties->get("slots"));
    }
}

void Entity::runPhysicsAndGraphics(double dt)
{
    animator.update(dt, sprite);

    if(not bonded && not slaved)
    {
        coords = coords + (getVelocity()*dt);
        sprite.setRotation(getRotation() + (getRotationalVelocity()*dt));
    }

    temperature -= 1.5*dt;
    if(temperature < 0){ temperature = 0; }

    double r_percent = 1;
    double g_percent = 1;
    double b_percent = 1;

    if(temperature > 273.15){

        if(temperature-273.15 > 200){
            if(properties->classification != "PLAYER"){
                getSector(sector_id)->trash(id);
                createEffect(sector_id, getPosition(), getVelocity(), "Smoke");
            }

        }
        if((temperature-273.15)>125){
            double percent = (temperature-398.15)/75;
            if(percent > 1){percent = 1;}
            castLight(sector_id, getPosition(), 0.8, (155*percent) + (sin(world.date.second)*(8*percent)));
        }
        if((temperature-273.15)>125 && abs(1-sin(world.date.second*2))<0.01){

             double neg_x = 1;
            double neg_y = 1;
            if(rand() % 100<50){neg_x=-1;}
            if(rand() % 100<50){neg_y=-1;}
            Vector2f excitement(neg_x*(rand()%50),neg_y*(rand()%50));

            createEffect(sector_id, getPosition(), excitement, "Embers");
        }

        double percent = (temperature-273.15)/400;
        if(percent > 1){percent = 1;}

        g_percent -= percent*0.75;
        b_percent -= percent;
    }

    sprite.setColor(Color(r_percent*(getSector(sector_id)->ambient_light+light_level),g_percent*(getSector(sector_id)->ambient_light+light_level),b_percent*(getSector(sector_id)->ambient_light+light_level)));
    light_level = 0;

    if(light_override!=Color(0,0,0))
    {
        sprite.setColor(light_override);
        light_override = Color(0,0,0);
    }

    if(not slaved){
        sprite.setPosition(coords.x,coords.y);
    }

}


double Entity::getInertia()
{
    if(bonded){return getSector(sector_id)->bonds[bond]->getInertia();}
    return getMyInertia();
}

double Entity::getMyInertia(){

    return getMyMass() + (convertToMeters(collision_radius)*3*getMyMass());
}

double Entity::getRotation()
{
    return sprite.getRotation();
}

Vector2f Entity::getMomentum()
{
    if(bonded){return getSector(sector_id)->bonds[bond]->p;}
    return p;
}

double Entity::getRotationalMomentum()
{
    if(bonded){return getSector(sector_id)->bonds[bond]->rot_p;}
    return rot_p;
}

double Entity::getMyMass(){

    return properties->get("mass");
}

double Entity::getMass()
{
    if(bonded){return getSector(sector_id)->bonds[bond]->mass;}
    return getMyMass();
}

Vector2f Entity::getVelocity()
{
    //references to the session cause this to break when the sector is not loaded but this is accessed

    if(bonded){return getSector(sector_id)->bonds[bond]->p/getSector(sector_id)->bonds[bond]->mass;}
    return p/getMass();
}

void Entity::setPosition(double x, double y)
{
    coords = Vector2f(x,y);
    sprite.setPosition(coords.x,coords.y);
}

void Entity::displace(double x, double y)
{
    if(bonded)
    {
        shared_ptr<Bond> my_bond = getSector(sector_id)->bonds[bond];
        my_bond->center = my_bond->center + Vector2f(x,y);
        double Q = my_bond->rotation*(M_PI/180); //rotation in radians

        for(set<string>::iterator i = my_bond->members.begin(); i != my_bond->members.end(); i++)
        {
            if(shared_ptr<Entity> e = getSector(sector_id)->getEnt(*i)){

                Vector2f newPosition = my_bond->center + Vector2f(my_bond->positions[e->id].x*cos(Q)-my_bond->positions[e->id].y*sin(Q),my_bond->positions[e->id].x*sin(Q)+my_bond->positions[e->id].y*cos(Q));
                e->setPosition(newPosition.x,newPosition.y);
                if(getSector(sector_id)->floors.count(*i)!=0){
                    for(set<string>::iterator parser = getSector(sector_id)->floors[*i]->passengers.begin(); parser != getSector(sector_id)->floors[*i]->passengers.end(); parser++){
                        if(my_bond->members.count(*parser)==0){
                            getSector(sector_id)->getEnt(*parser)->displace(x,y);
                        }
                    }
                }
            }
        }

        return;
    }
    else{
        coords = coords + Vector2f(x,y);
        sprite.setPosition(coords.x,coords.y);
    }

}
void Entity::setRotation(double d)
{
    sprite.setRotation(d);
}

void Entity::applyImpulse(double x,double y,bool apply_damage, bool apply_spin)
{
    if(apply_damage){
        applyImpulseDamage(x,y);
    }

    if(bonded){getSector(sector_id)->bonds[bond]->applyImpulse(x,y,getPosition(),apply_spin); return;}
    p = p + Vector2f(x,y);
}



void Entity::applyImpulse(double x,double y, Vector2f point,bool apply_damage, bool apply_spin)
{
    if(apply_damage){
        applyImpulseDamage(x,y);
    }

    if(bonded){getSector(sector_id)->bonds[bond]->applyImpulse(x,y,point, apply_spin); return;}


    Vector2f dist = point-coords;

    //angular impulse
    if(abs(dist.x)<1){dist.x = 0;}
    if(abs(dist.y)<1){dist.y = 0;}

    Vector2f normal = getPerpendicularAxis(Vector2f(dist.x,dist.y));
    double th = (x*normal.x + y*normal.y)*convertToMeters(hypot(dist.x,dist.y));


    Vector2f vec_p = Vector2f(x,y);

    if(apply_spin){
        rot_p += th;
        //vec_p = Vector2f(x-(x*normal.x),y-(y*normal.y));
    }


    p = p + vec_p;


}


void Entity::applyRotationalImpulse(double d)
{
    if(bonded)
    {
       getSector(sector_id)->bonds[bond]->rot_p = getSector(sector_id)->bonds[bond]->rot_p + d;

        return;
    }
    rot_p = rot_p+d;
}

void Entity::setVelocity(double x, double y)
{
    if(bonded){
        getSector(sector_id)->bonds[bond]->p = Vector2f(x,y)*getSector(sector_id)->bonds[bond]->mass;
        return;
    }
    p = Vector2f(x,y)*getMass();
}

void Entity::setRotationalVelocity(double d)
{
    if(bonded){
        getSector(sector_id)->bonds[bond]->rot_p = d*getSector(sector_id)->bonds[bond]->getInertia();
        return;
    }
    rot_p = d*getInertia();
}
double Entity::getRotationalVelocity()
{
    if(bonded){return getSector(sector_id)->bonds[bond]->rot_p/getSector(sector_id)->bonds[bond]->getInertia();}
    return rot_p/getInertia();
}
Vector2f Entity::getAngularVelocity(double dt)
{
    if(not bonded){return Vector2f(0,0);}


}

Vector2f Entity::getAngularVelocity(double dt, Vector2f point)
{
    if(bonded)
    {
        shared_ptr<Bond> my_bond = getSector(sector_id)->bonds[bond];

        //calculate angular_v

        //ROT P IS IN DEGREES NOT RADIANS HERE!!! PROBABLY CAUSING PROBLEMS!
        Vector2f relative_point = point-my_bond->center;
        return (getPerpendicularAxis(relative_point)*(my_bond->rot_p/convertToMeters(hypot(relative_point.x,relative_point.y))))/my_bond->getInertia();
    }

    else if(point != coords)
    {
        Vector2f relative_point = point-coords;
        return (getPerpendicularAxis(relative_point)*(rot_p/convertToMeters(hypot(relative_point.x,relative_point.y))))/getInertia();


    }

    return Vector2f(0,0);

}

Vector2f Entity::getPosition()
{
    return coords;
}




void Entity::cleanup(){

    dead = true;

    for(set<string>::iterator parser = connectors.begin(); parser!=connectors.end(); parser++){
        getSector(sector_id)->trash(*parser);
    }
}



void Entity::run(double dt)
{
    runPhysicsAndGraphics(dt);
}

void Entity::draw()
{
    if(not slaved){
        sprite.setPosition(coords.x,coords.y);
        window.draw(sprite);
    }
}

double Entity::getHealthPercentage(){

    return (current_health/properties->get("max_health"))*100;
}

void Entity::applyImpulseDamage(double x,double y){
    if(properties->classification=="ITEM"){return;}
    double magnitude = hypot(x,y)/100;

    //set<string> connections_to_break;
   // for(set<string>::iterator parser = connectors.begin(); parser!=connectors.end(); parser++){
      //  if(magnitude>=getSector(sector_id)->connectors[*parser]->durability){
      //      connections_to_break.insert(*parser);
      //  }
   // }
   // for(set<string>::iterator parser = connections_to_break.begin(); parser!=connections_to_break.end(); parser++){
//        getSector(sector_id)->trash(*parser);
   // }

    //averages ~1-10 for casual walking
    //bumping is between 10 and 50 (tested 30 for player jumping directly into small machine)    //ANY REASONBLY SOUND OBJECT SHOULD HAVE A MINIMUM DURABILITY OF ~25
    //colliding is between 50 and 100?
    //smashing is >100?
    if(magnitude>properties->get("durability")){
        current_health -= magnitude;
    }

//    set<string> connections_to_break;
//    double connection_strength = 10000;
//    for(set<string>::iterator parser = connectors.begin(); parser!=connectors.end(); parser++){
//        if(getSector(sector_id)->connectors.count(*parser)==0){continue;}
//        connection_strength += getSector(sector_id)->connectors[*parser]->durability;
//    }
//
//    if(magnitude>connection_strength){
//        connections_to_break = connectors;
//    }
//
//    for(set<string>::iterator parser = connections_to_break.begin(); parser!=connections_to_break.end(); parser++){
//        if(getSector(sector_id)->connectors.count(*parser)==0){continue;}
//        removeConnector(*parser);
//    }

    if(current_health<=0){
       // getSector(sector_id)->garbage_bin.push_back(id);
       getSector(sector_id)->trash(id);
    }
}


void Entity::light(double amount)
{
    light_level += amount;
    if(getSector(sector_id)->ambient_light + light_level > 255){ light_level =  255-getSector(sector_id)->ambient_light; }
}

void Entity::overrideLight(int r,int g,int b)
{
    light_override = Color(r,g,b);
}

shared_ptr<Entity> Entity::getThis()
{
    return getSector(sector_id)->getEnt(id);
}

bool Entity::contains(Vector2f point){

    Sprite test_sprite = sprite;
    Vector2f relative = point - Vector2f(test_sprite.getPosition().x,test_sprite.getPosition().y);

    double degrees = -test_sprite.getRotation();
    double Q = degrees*(M_PI/180);

    Vector2f new_position = Vector2f(test_sprite.getPosition().x,test_sprite.getPosition().y) + Vector2f(relative.x*cos(Q)-relative.y*sin(Q),relative.x*sin(Q)+relative.y*cos(Q));
    test_sprite.rotate(degrees);

    if(test_sprite.getGlobalBounds().contains(new_position.x,new_position.y)){
        return true;
    }

    return false;
}

bool Entity::intersects(Vector2f point, double radius){

    vector<Vector2f> axises;

    axises.push_back(getMyParallelAxis());
    axises.push_back(getMyPerpendicularAxis());

    for(vector<Vector2f>::iterator i = axises.begin(); i != axises.end(); i++)
    {
         //*i is unit vector of axis to project onto
        Vector2f axis = *i;

        Vector2f maxA = getMaxPoint(axis); //get the corner of A that projects farthest "up" the axis P
        Vector2f minB = getMinPoint(axis); //get the corner of B that projects farthest "down" the axis P
        Vector2f maxB = point + (axis*radius);
        Vector2f minA = point - (axis*radius);

        double pCB = point.x*axis.x + point.y*axis.y;
        double pCA = getPosition().x*axis.x + getPosition().y*axis.y;

        double pC = pCB-pCA;


        double pA = maxA.x*axis.x + maxA.y*axis.y; //calculate the X and Y components of a diagonal line drawn between the center of A and its maxpoint on P

        double pB = minB.x*axis.x + minB.y*axis.y; //calculate the X and Y components of a diagonal line drawn between the center of B and its minpoint on P

        double pmA = abs(pA-pCA); //calculate the magnitude of the projection of A's center-to-corner diagonal on P
        double pmB = abs(radius); //calculate the magnitude of the projection of B's center-to-corner diagonal on P

        if(((abs(pC) - pmA) - pmB)>0) //if pC is greater than pA + pB then A and B are not intersecting
        {

           return false;
        }
    }

    return true;
}

bool Entity::intersects(string other_id)
{
    shared_ptr<Entity> other = getSector(sector_id)->getEnt(other_id);

    vector<Vector2f> axises;

    axises.push_back(getMyParallelAxis());
    axises.push_back(getMyPerpendicularAxis());
    axises.push_back(other->getMyParallelAxis());
    axises.push_back(other->getMyPerpendicularAxis());

    for(vector<Vector2f>::iterator i = axises.begin(); i != axises.end(); i++)
    {
         //*i is unit vector of axis to project onto

        if(getGap(getThis(), other, *i)>0) //if pC is greater than pA + pB then A and B are not intersecting
        {
           return false;
        }
    }

    //otherwise they are

    return true;
}

Vector2f Entity::getBottomLeft()
{
    FloatRect testRect = sprite.getLocalBounds();
    double x = coords.x - testRect.width/2;
    double y = coords.y + testRect.height/2;
    return Vector2f(x,y);
}

Vector2f Entity::getTopRight()
{
    FloatRect testRect = sprite.getLocalBounds();
    double x = coords.x + testRect.width/2;
    double y = coords.y - testRect.height/2;
    return Vector2f(x,y);
}

Vector2f Entity::getd(int index)
{
    //calculate coordinates of specified corner
    Vector2f returnval;
    double x1 = getTopRight().x-getPosition().x;
    double x2 = getBottomLeft().x-getPosition().x;
    double y1 = getTopRight().y-getPosition().y;
    double y2 = getBottomLeft().y-getPosition().y;
    double Q = sprite.getRotation()*M_PI/180; //rotation in radians

    switch(index)
    {
        case 1:
            returnval = Vector2f(x1*cos(Q)-y1*sin(Q),x1*sin(Q)+y1*cos(Q));
            break;
        case 2:
            returnval = Vector2f(x1*cos(Q)-y2*sin(Q),x1*sin(Q)+y2*cos(Q));
            break;
        case 3:
            returnval = Vector2f(x2*cos(Q)-y2*sin(Q),x2*sin(Q)+y2*cos(Q));
            break;
        case 4:
            returnval = Vector2f(x2*cos(Q)-y1*sin(Q),x2*sin(Q)+y1*cos(Q));
            break;
    }

    returnval.x += getPosition().x;
    returnval.y += getPosition().y;
    return returnval;

}

void Entity::collectNormals(vector<Vector2f> &axises)
{
    Vector2f side1 = Vector2f((getd(1).x-getd(4).x),(getd(1).y-getd(4).y));
    double mag1 = hypot(side1.x,side1.y);
    axises.push_back(Vector2f(side1.x/mag1,side1.y/mag1));

    Vector2f side2 = Vector2f(getd(2).x-getd(1).x,getd(2).y-getd(1).y);
    double mag2 = hypot(side2.x,side2.y);
    axises.push_back(Vector2f(side2.x/mag2,side2.y/mag2));
}

Vector2f Entity::getClosestPoint(Vector2f point)
{
    Vector2f current = getd(1);
    double dist = abs(hypot(current.x-point.x,current.y-point.y));
    Vector2f returnval = current;
    double lowest = dist;

    for(int i = 2; i <= 4; i++)
    {
        current = getd(i);
        dist = abs(hypot(current.x-point.x,current.y-point.y));
        if(dist<lowest)
        {
            returnval = current;
            lowest = dist;
        }
        if(dist==lowest)
        {
            returnval = Vector2f((returnval.x+current.x)/2,(returnval.y+current.y)/2);
        }
    }
    return returnval;
}

Vector2f Entity::getMaxPoint(Vector2f axis)
{
    Vector2f current = getd(1);
    double projection = axis.x*current.x+axis.y*current.y;
    Vector2f returnval = current;
    double highest = projection;

    for(int i = 2; i <= 4; i++)
    {
        current = getd(i);
        projection = axis.x*current.x+axis.y*current.y;
        if(projection>highest)
        {
            returnval = current;
            highest = projection;
        }
    }
    return returnval;
}

Vector2f Entity::getMinPoint(Vector2f axis)
{
    Vector2f current = getd(1);
    double projection = axis.x*current.x+axis.y*current.y;
    Vector2f returnval = current;
    double lowest = projection;

    for(int i = 2; i <= 4; i++)
    {
        current = getd(i);
        projection = axis.x*current.x+axis.y*current.y;
        if(projection<lowest)
        {
            returnval = current;
            lowest = projection;
        }
    }
    return returnval;
}

Vector2f Entity::getMyParallelAxis()
{
     Vector2f side = Vector2f(getd(1).y-getd(4).y,getd(1).x-getd(4).x);
    double mag = hypot(side.x,side.y);
    Vector2f axis = Vector2f(side.x/mag,side.y/mag);


    if(getRotation()<=90)
    {
        if(axis.y<0)
        {
            axis = Vector2f(abs(axis.x),axis.y);
        }
        else
        {
            axis = Vector2f(abs(axis.x),-axis.y);
        }
    }
    else if(getRotation()<=180)
    {
        axis = Vector2f(abs(axis.x),abs(axis.y));
    }
    else if(getRotation()<=270)
    {
        if(axis.x<0){axis = Vector2f(axis.x,abs(axis.y));}
        else{axis = Vector2f(-axis.x,abs(axis.y));}
    }
    else if(getRotation()<=360)
    {
        if(axis.y<0 and axis.x<0){axis = Vector2f(axis.x,axis.y);}
        else if(axis.y<0){axis = Vector2f(-axis.x,axis.y);}
        else if(axis.x<0){axis = Vector2f(axis.x,-axis.y);}
        else {axis = Vector2f(-axis.x,-axis.y);}
    }

    return axis;
}

Vector2f Entity::getMyPerpendicularAxis()
{
    Vector2f pl_axis = getMyParallelAxis();
    Vector2f pp_axis = getPerpendicularAxis(pl_axis);
    return pp_axis;
}

void Entity::getConnectedNoPiston(set<string>* return_set)
{

    cleanupDeadConnectors();

    for(set<string>::iterator parser = connectors.begin(); parser!=connectors.end(); parser++){

        shared_ptr<Connector> c = getSector(sector_id)->connectors[*parser];

        if(c->type == "PISTON"){ continue; }

        if(shared_ptr<Entity> A = getSector(sector_id)->getEnt(c->a_id)){

            if(return_set->count(c->a_id)==0){

                return_set->insert(c->a_id);
                A->getConnectedNoPiston(return_set);
            }
        }
        if(shared_ptr<Entity> B = getSector(sector_id)->getEnt(c->b_id)){

            if(return_set->count(c->b_id)==0){
                return_set->insert(c->b_id);
                B->getConnectedNoPiston(return_set);
            }
        }
    }
}

void Entity::getConnected(set<string>* return_set)
{
    cleanupDeadConnectors();

    for(set<string>::iterator parser = connectors.begin(); parser!=connectors.end(); parser++){

        shared_ptr<Connector> c = getSector(sector_id)->connectors[*parser];

        if(shared_ptr<Entity> A = getSector(sector_id)->getEnt(c->a_id)){

            if(return_set->count(c->a_id)==0){
                return_set->insert(c->a_id);
                A->getConnected(return_set);
            }
        }
        if(shared_ptr<Entity> B = getSector(sector_id)->getEnt(c->b_id)){

            if(return_set->count(c->b_id)==0){
                return_set->insert(c->b_id);
                B->getConnected(return_set);
            }
        }
    }
}

void Entity::cleanupDeadConnectors()
{
    vector<string> to_delete;
    for(set<string>::iterator parser = connectors.begin(); parser!=connectors.end(); parser++)
    {

        if(getSector(sector_id)->connectors.count(*parser)==0 or getSector(sector_id)->connectors[*parser]->queued_for_deletion)
        {
            to_delete.push_back(*parser);
        }
    }
    for(vector<string>::iterator parser = to_delete.begin(); parser!=to_delete.end(); parser++)
    {
        connectors.erase(*parser);
    }
}

void Entity::setSlots(int slots){

    for(int i = 1; i <= slots; i++){
        contents[i] = "";
    }
}

int Entity::getSlots(){

    int max_index = 0; //find amount of slots in entity

    for(map<int,string>::iterator parser = contents.begin(); parser!=contents.end(); parser++){
        if(parser->first>max_index){
            max_index = parser->first;
        }
    }
    return max_index;
}

bool Entity::addToContents(string item_id){


    int slots = getSlots();
    if(slots<=0){
        return false;
    }

    for(int i = 1; i <= slots; i++){
        if(addToContents(item_id,i)){
            return true;
        }
    }

    //if function has failed to find an empty slot by this point
    return false;
}

bool Entity::addToContents(string item_id, int slot_index){

    if(contents[slot_index]=="" and getSector(sector_id)->items.count(item_id)!=0){
        contents[slot_index] = item_id;
        getSector(sector_id)->items[item_id]->storage_id = id;
        return true;
    }
    else{
        return false;
    }
}

int Entity::getSlotIndexOf(string item_id){

    for(map<int,string>::iterator parser = contents.begin(); parser!=contents.end(); parser++){
        if(parser->second==item_id){

            return parser->first;
        }
    }
    //if item_id not found in any slots
    return 0;
}

void Entity::removeFromContents(string item_id){

    int index = getSlotIndexOf(item_id);
    if(index!=0){
        if(getSector(sector_id)->items.count(contents[index])!=0){
            getSector(sector_id)->items[contents[index]]->storage_id = "";
        }
        contents[index] = "";
    }
}


//void Entity::breakdown(){

//    for(vector<pair<string, Vector2f> >::iterator parser = info.composition.begin(); parser!=info.composition.end(); parser++){
//
//        Info parsed_info = getInfo(parser->first);
//
//        Vector2f relative_coords = parser->second;
//        if(relative_coords.x==-1000 and relative_coords.y==-1000){continue;}
//
//        Vector2f center = getPosition();
//        double Q = sprite.getRotation()*M_PI/180; //rotation in radians
//        Vector2f new_coords = center + Vector2f(relative_coords.x*cos(Q)-relative_coords.y*sin(Q),relative_coords.x*sin(Q)+relative_coords.y*cos(Q));
//
//        if(parsed_info.item_classification==I_STRUCTURE){
//            string new_id = makeNewEntity(sector_id, createUniqueId(), new_coords.x, new_coords.y, parser->first);
//            getSector(sector_id)->ents[new_id]->sprite.setRotation(sprite.getRotation());
//            getSector(sector_id)->ents[new_id]->setVelocity(getVelocity().x,getVelocity().y);
//            getSector(sector_id)->ents[new_id]->setRotationalVelocity(getRotationalVelocity());
//        }
//        else{
//
//            string item_id = registerNewItem(sector_id, parser->first);
//            shared_ptr<Item> item = getSector(sector_id)->items[item_id];
//            item->storage_id = makeNewScrap(sector_id, createUniqueId(), new_coords.x,new_coords.y);
//            getSector(sector_id)->scrap[item->storage_id].lock()->setItem(item->id);
//            getSector(sector_id)->scrap[item->storage_id].lock()->sprite.setRotation(sprite.getRotation());
//
//            double rot_amount = rand()%100;
//            if(rand() % 100<50){rot_amount*=-1;}
//            getSector(sector_id)->scrap[item->storage_id].lock()->setVelocity((rot_amount)+getVelocity().x/10,(rot_amount)+getVelocity().y/10);
//            getSector(sector_id)->scrap[item->storage_id].lock()->setRotationalVelocity(getRotationalVelocity()+(rot_amount));
//
//        }
//    }
//    for(map<int,string>::iterator parser = contents.begin(); parser!=contents.end(); parser++){
//
//        if(parser->second=="" or getSector(sector_id)->items.count(parser->second)==0){continue;}
//
//
//    }
//
//    setVelocity(0,0);
//    setRotationalVelocity(0);

//}


string makeNewEntity(string sector_id, string ent_id, double x, double y, string type){

    getSector(sector_id)->ents[ent_id] = make_shared<Entity>(sector_id,ent_id,x,y,type);
    return ent_id;
}

string makeNewEntity(string sector_id, string ent_id, double x, double y, double rotation, string type){

    getSector(sector_id)->ents[ent_id] = make_shared<Entity>(sector_id,ent_id,x,y,type);
    getSector(sector_id)->ents[ent_id]->setRotation(rotation);
    return ent_id;
}

bool operator<(const weak_ptr<Entity> &A, const weak_ptr<Entity> &B)
{
    return true;
}

bool operator==(const weak_ptr<Entity> &A, const weak_ptr<Entity> &B)
{
    if(A.lock()==B.lock())
    {
        return true;
    }
}

