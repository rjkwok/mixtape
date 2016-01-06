#include "main.h"

using namespace std;
using namespace sf;

extern map<string, Properties*> properties;
extern map<string, Texture*> textures;
extern map<string, Font*> fonts;
extern RenderWindow window;

Ship* randomNewCard(int c_owner){

    map<string, Properties*>::iterator p = properties.begin();
    advance(p, rand() % properties.size());

    return new Ship(c_owner, p->first);
}

string asString(double x){

    stringstream converter;
    string return_string = "";
    converter << x;
    getline(converter, return_string);
    return return_string;
}

string asString(int x){

    stringstream converter;
    string return_string = "";
    converter << x;
    getline(converter, return_string);
    return return_string;
}

CircleShape makeCircle(double radius, Color fill_colour, Vector2f position){

    CircleShape new_circle;
    new_circle.setRadius(radius);
    new_circle.setOrigin(radius,radius);
    new_circle.setFillColor(fill_colour);
    new_circle.setOutlineThickness(0);
    new_circle.setPosition(position.x,position.y);
    return new_circle;
}

RectangleShape makeRect(double left, double top, double width, double height, Color colour){

    RectangleShape rect;
    rect.setSize(Vector2f(width,height));
    rect.setOutlineThickness(0);
    rect.setFillColor(colour);
    rect.setPosition(left, top);
    return rect;
}


CardGraphic::CardGraphic(){}
CardGraphic::CardGraphic(string card_name, double strength, Color frame_colour, Color strength_colour, Color name_colour, bool hidden){

    scale = 1.0;

    hidden_mode = hidden;

    frame_sprite.setTexture(*getTexture("frame"),true);
    frame_sprite.setOrigin(250,312);
    frame_sprite.setColor(frame_colour);

    if(hidden_mode){

        if(card_name == "VOID"){
            main_sprite.setTexture(*getTexture("blank"),true);
        }
        else if(getProperties(card_name)->classification == "Pirate"){
            main_sprite.setTexture(*getTexture("pirate_logo"),true);
        }
        else if(getProperties(card_name)->classification == "Military"){
            main_sprite.setTexture(*getTexture("military_logo"),true);
        }
        else if(getProperties(card_name)->classification == "Merchant"){
            main_sprite.setTexture(*getTexture("merchant_logo"),true);
        }
        else if(getProperties(card_name)->classification == "Derelict"){
            main_sprite.setTexture(*getTexture("derelict_logo"),true);
        }
        else if(getProperties(card_name)->classification == "Robot"){
            main_sprite.setTexture(*getTexture("robot_logo"),true);
        }
        main_sprite.setOrigin(250,250);
    }
    else{

        if(card_name == "VOID"){
            main_sprite.setTexture(*getTexture("blank"),true);
            main_sprite.setOrigin(250,250);
        }
        else{
            main_sprite.setTexture(*getProperties(card_name)->main_tex,true);
            main_sprite.setOrigin(250,250);
        }
       

    }

    name_text.setFont(*getFont("font1"));
    name_text.setCharacterSize(36);
    name_text.setColor(name_colour);
    name_text.setString(card_name);
    name_text.setOrigin(0,20);

    strength_text.setFont(*getFont("font1"));
    strength_text.setCharacterSize(36);
    strength_text.setColor(strength_colour);
    strength_text.setString(asString(strength));
    strength_text.setOrigin(30,0);

}

void CardGraphic::setScale(double factor){

    scale = factor;

    frame_sprite.setScale(factor,factor);
    main_sprite.setScale(factor,factor);
    name_text.setScale(factor,factor);
    strength_text.setScale(factor,factor);

    setCenter(frame_sprite.getPosition());
}
void CardGraphic::setCenter(Vector2f position){

    frame_sprite.setPosition(position.x,position.y);
    main_sprite.setPosition(position.x,position.y);
    name_text.setPosition(position.x - (scale*215),position.y + (scale*270));
    strength_text.setPosition(position.x + (scale*235),position.y - (scale*280));
}
void CardGraphic::draw(){

    window.draw(frame_sprite);
    window.draw(main_sprite);
    if(!hidden_mode){
        window.draw(name_text);
    }
    window.draw(strength_text);
}

Properties::Properties(){}
Properties* getProperties(string type_id){

    return properties[type_id];
}

Properties::~Properties(){

    delete main_tex;
    delete thumb_tex;
}

void loadProperties(){
    
    TiXmlDocument doc("types.xml");
    doc.LoadFile();

    for(TiXmlElement* parser = doc.FirstChildElement("Type"); parser!=NULL; parser = parser->NextSiblingElement("Type"))
    {
        Properties* loader = new Properties();

        loader->name = parser->Attribute("name");
        loader->classification = parser->Attribute("class");
        loader->main_tex = new Texture();
        loader->thumb_tex = new Texture();
        loader->main_tex->loadFromFile(parser->Attribute("main_tex"));
        loader->thumb_tex->loadFromFile(parser->Attribute("thumb_tex"));
        loader->base_strength = int(strtod(parser->FirstChildElement("base_strength")->GetText(),NULL));
        loader->main_window_x = int(strtod(parser->FirstChildElement("main_window_x")->GetText(),NULL));
        loader->main_window_y = int(strtod(parser->FirstChildElement("main_window_y")->GetText(),NULL));

        for(TiXmlElement* ability_parser = parser->FirstChildElement("ability"); ability_parser != NULL; ability_parser = ability_parser->NextSiblingElement("ability")){
            loader->abilities.insert(ability_parser->GetText());
        }
        
        properties[loader->name] = loader;
    }

}

void destroyProperties(){

    for(map<string, Properties*>::iterator i = properties.begin(); i != properties.end(); i++){
        delete i->second;
    }
}

Texture* getTexture(string txid){

    return textures[txid];
}

void loadTextures(){
    
    TiXmlDocument doc("textures.xml");
    doc.LoadFile();

    for(TiXmlElement* parser = doc.FirstChildElement("Tx"); parser!=NULL; parser = parser->NextSiblingElement("Tx"))
    {
        Texture* loader = new Texture();
        loader->loadFromFile(parser->Attribute("path"));
        textures[parser->Attribute("id")] = loader;
    }

}

void destroyTextures(){

    for(map<string, Texture*>::iterator i = textures.begin(); i != textures.end(); i++){
        delete i->second;
    }
}

Font* getFont(string font_id){

    return fonts[font_id];
}
void loadFonts(){

    TiXmlDocument doc("fonts.xml");
    doc.LoadFile();

    for(TiXmlElement* parser = doc.FirstChildElement("Font"); parser!=NULL; parser = parser->NextSiblingElement("font"))
    {
        Font* loader = new Font();
        loader->loadFromFile(parser->Attribute("path"));
        fonts[parser->Attribute("id")] = loader;
    }
}
void destroyFonts(){
    for(map<string, Font*>::iterator i = fonts.begin(); i != fonts.end(); i++){
        delete i->second;
    }
}


Ship::Ship(){}
Ship::Ship(int c_owner, string c_type_id){

    owner = c_owner;
    type_id = c_type_id;

   /* sprite.setTexture(*getProperties(type_id)->main_tex, false);
    sprite.setTextureRect(IntRect(0,0,getProperties(type_id)->main_window_x,getProperties(type_id)->main_window_y));
    sprite.setOrigin(getProperties(type_id)->main_window_x/2.0,getProperties(type_id)->main_window_y/2.0);
*/
    strength = 0;

    support_blocked = false;
    nullify_strength = false;
}

string Ship::getClass(){

    return getProperties(type_id)->classification;
}

set<string>* Ship::getAbilities(){

    return &getProperties(type_id)->abilities;
}

double Ship::getBase(){

    return getProperties(type_id)->base_strength;
}

int Ship::getOwner(){
    return owner;
}

Scroller::Scroller(){}
Scroller::Scroller(Vector2f c_anchor, double width, double height){

    bounds = IntRect(0,0,width,height);
    anchor = c_anchor;

    a.setTextureRect(bounds);
    b.setTextureRect(bounds);
    a.setTexture(*getTexture("end_of_reel"), false);
    b.setTexture(*getTexture("end_of_reel"), false);
}

Scroller::~Scroller(){

}

void Scroller::process(double dt){

    double amount = 1;

    if(current_active=='a' && b.getTextureRect().height > 0){
        //scrolling down to get to a
        if(scroll_direction == -1){
            IntRect b_rect = b.getTextureRect();
            b_rect.height -= amount;
            if(b_rect.height < 0){ b_rect.height = 0;}
            b.setTextureRect(b_rect);
            b.move(0, amount);

            IntRect a_rect = a.getTextureRect();
            a_rect.height += amount;
            a_rect.top -= amount;
            a.setTextureRect(a_rect);
        }
        else{//scrolling up to get to a
            IntRect b_rect = b.getTextureRect();
            b_rect.height -= amount;
            b_rect.top += amount;
            if(b_rect.height < 0){ b_rect.height = 0;}
            b.setTextureRect(b_rect);

            IntRect a_rect = a.getTextureRect();
            a_rect.height += amount;
            a.setTextureRect(a_rect);
            a.move(0,-amount);
        }
    }
    else if(current_active=='b' && a.getTextureRect().height > 0){
        //scrolling down to get to b
        if(scroll_direction == -1){
            IntRect a_rect = a.getTextureRect();
            a_rect.height -= amount;
            if(a_rect.height < 0){ a_rect.height = 0;}
            a.setTextureRect(a_rect);
            a.move(0, amount);

            IntRect b_rect = b.getTextureRect();
            b_rect.height += amount;
            b_rect.top -= amount;
            b.setTextureRect(b_rect);
        }
        else{//scrolling up to get to b
            IntRect a_rect = a.getTextureRect();
            a_rect.height -= amount;
            a_rect.top += amount;
            if(a_rect.height < 0){ a_rect.height = 0;}
            a.setTextureRect(a_rect);

            IntRect b_rect = b.getTextureRect();
            b_rect.height += amount;
            b.setTextureRect(b_rect);
            b.move(0,-amount);
        }
    }
}

void Scroller::draw(){

    window.draw(a);
    window.draw(b);
}

void Scroller::scrollDownTo(string type_id){

    //place both at top of box
    if(current_active == 'a'){

        
        b.setTexture(*getProperties(type_id)->main_tex,false);
        a.setPosition(anchor.x-(bounds.width/2.0),anchor.y-(bounds.height/2.0));
        b.setPosition(anchor.x-(bounds.width/2.0),anchor.y-(bounds.height/2.0));
        IntRect temp_rect = b.getTextureRect();
        temp_rect.height = 0;
        temp_rect.top = bounds.height;
        b.setTextureRect(temp_rect);
        a.setTextureRect(bounds);
        current_active = 'b';
    }
    else{

        a.setTexture(*getProperties(type_id)->main_tex,false);
        a.setPosition(anchor.x-(bounds.width/2.0),anchor.y-(bounds.height/2.0));
        b.setPosition(anchor.x-(bounds.width/2.0),anchor.y-(bounds.height/2.0));
        IntRect temp_rect = a.getTextureRect();
        temp_rect.height = 0;
        temp_rect.top = bounds.height;
        a.setTextureRect(temp_rect);
        b.setTextureRect(bounds);
        current_active = 'a';
    }

    scroll_direction = -1;
}

void Scroller::scrollUpTo(string type_id){

    //place primary at top and secondary at bottom
    if(current_active == 'a'){


        b.setTexture(*getProperties(type_id)->main_tex,false);
        a.setPosition(anchor.x-(bounds.width/2.0),anchor.y-(bounds.height/2.0));
        b.setPosition(anchor.x-(bounds.width/2.0),anchor.y+(bounds.height/2.0));
        IntRect temp_rect = b.getTextureRect();
        temp_rect.height = 0;
        temp_rect.top = 0;
        b.setTextureRect(temp_rect);
        a.setTextureRect(bounds);
        current_active = 'b';
    }
    else{

        a.setTexture(*getProperties(type_id)->main_tex,false);
        a.setPosition(anchor.x-(bounds.width/2.0),anchor.y+(bounds.height/2.0));
        b.setPosition(anchor.x-(bounds.width/2.0),anchor.y-(bounds.height/2.0));
        IntRect temp_rect = a.getTextureRect();
        temp_rect.height = 0;
        temp_rect.top = 0;
        a.setTextureRect(temp_rect);
        b.setTextureRect(bounds);
        current_active = 'a';
    }

    scroll_direction = 1;
}


bool Scroller::contains(double x, double y){

    if(a.getGlobalBounds().contains(x,y) || b.getGlobalBounds().contains(x,y)){
        return true;
    }
    return false;
}


map<int, map<int, Vector2f> > calculateSockets(Vector2f core_a, Vector2f core_b){

    map<int, map<int, Vector2f> > return_map;

    double board_width = core_b.x - core_a.x;
    double arc_spacing= board_width/7.0;

    for(int index = 1; index < 4; index++){

        double radius = index*arc_spacing;
        int amount_of_sockets = 1 + (2*index);
        double angle_spacing = 80.0/amount_of_sockets;

        for(int j = 0; j < amount_of_sockets; j++){
            Vector2f coordinates;
            double angle_from_vertical = (angle_spacing*j) + 50.0 + (angle_spacing/2.0);
            if(angle_from_vertical >= 90){
                angle_from_vertical -= 90;
                double radians_from_vertical = angle_from_vertical * (3.14159/180.0);
                coordinates.x = cos(radians_from_vertical)*radius;
                coordinates.y = sin(radians_from_vertical)*radius;
            }
            else{
                double radians_from_vertical = angle_from_vertical * (3.14159/180.0);
                coordinates.x = sin(radians_from_vertical)*radius;
                coordinates.y = -cos(radians_from_vertical)*radius;
            }
            coordinates = coordinates + core_a;
            return_map[index][j] = coordinates;
        }
    }
    for(int index = 1; index < 4; index++){

        double radius = index*arc_spacing;
        int amount_of_sockets = 1 + (2*index);
        double angle_spacing = 80.0/amount_of_sockets;
        for(int j = 0; j < amount_of_sockets; j++){
            Vector2f coordinates;
            double angle_from_vertical = (angle_spacing*j) + 50.0 + (angle_spacing/2.0);
            if(angle_from_vertical >= 90){
                angle_from_vertical -= 90;
                double radians_from_vertical = angle_from_vertical * (3.14159/180.0);
                coordinates.x = cos(radians_from_vertical)*radius;
                coordinates.y = sin(radians_from_vertical)*radius;
            }
            else{
                double radians_from_vertical = angle_from_vertical * (3.14159/180.0);
                coordinates.x = sin(radians_from_vertical)*radius;
                coordinates.y = -cos(radians_from_vertical)*radius;
            }
            coordinates.x *= -1;
            coordinates = coordinates + core_b;
            return_map[6-(index-1)][j] = coordinates;
        }
    }

    return return_map;
}
