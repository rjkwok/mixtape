#include "main.h"

using namespace std;
using namespace sf;

extern map<string, Texture*> textures;
extern map<string, IntRect> texture_rects;
extern set<int> collidable_terrain_types;
extern map<int, TileProperties> tile_properties;
extern map<string, StructureProperties> structure_properties;
extern map<string, ShipProperties> ship_properties;
extern map<string, CharacterProperties> character_properties;
extern map<string, Font> fonts;


char randChar()
{
    return (char)( (rand() % 52) + 65);
}

int randInt(int ubound){

	return (rand() % ubound)+1;
}

int randSign(){

    if(randInt(2) == 1){ return -1; }
    else{ return 1; }
}

Vector2f getPerpendicularAxis(Vector2f axis)
{

    Vector2f perp = Vector2f(0,0);

    double x1 = axis.x;
    double y1 = axis.y;
    double h1 = hypot(x1,y1);

    if(h1 == 0){
        return perp;
    }

    perp.x = -1*(y1/h1);
    perp.y = (x1/h1);

    return perp;
}

string createUniqueId()
{
    int suffixlength = 8;

    string newid = "";

    for(int i = 0; i < suffixlength; i++)
    {
        newid += randChar();
    }

    return newid;
}


Caption::Caption(){}
Caption::Caption(string contents, string font, Vector2f coords, int char_size, Color color, string bias){

    text.setString(contents);
    text.setFont(fonts[font]);
    text.setCharacterSize(char_size);

    FloatRect rect = text.getGlobalBounds();

    if(bias == "left"){
        text.setOrigin(0,char_size/2.0);
    }
    else if(bias == "middle"){
        text.setOrigin(rect.width/2.0,char_size/2.0);
    }
    else if(bias == "right"){
        text.setOrigin(rect.width,char_size/2.0);
    }

    text.setColor(color);
    text.setScale(0.5,0.5);
    text.setPosition(coords);
}
void Caption::draw(RenderWindow &window){

    window.draw(text);
}

Animation::Animation(){}
Animation::Animation(int c_start, int c_end, bool c_looping){

    start_frame = c_start;
    end_frame = c_end;
    is_looping = c_looping;

    current_frame = start_frame;
    time_since_last_frame = 0;
}



bool Animation::play(double dt, Sprite &sprite, double fps){

    if(fps == 0){ return false;}

    bool return_signal = false;
    time_since_last_frame += dt;
    if(time_since_last_frame >= 1.0/fps){
        current_frame++;
        time_since_last_frame -= (1.0/fps);
        if(current_frame > end_frame){
            if(is_looping){
                current_frame = start_frame;
            }
            else{
                current_frame = end_frame;
                return_signal = true; //signals animation complete
            }
        }
    }
    sprite.setTextureRect(getFrame(current_frame, sprite));
}

RectangleShape createBoundingRectangle(Sprite sprite, Color color){

    RectangleShape new_rectangle(Vector2f(sprite.getTextureRect().width, sprite.getTextureRect().height));
    new_rectangle.setOrigin(sprite.getTextureRect().width/2.0, sprite.getTextureRect().height/2.0);
    new_rectangle.setRotation(sprite.getRotation());
    new_rectangle.setPosition(sprite.getPosition());
    new_rectangle.setFillColor(Color(0,0,0,0));
    new_rectangle.setOutlineColor(color);
    new_rectangle.setOutlineThickness(4);

    return new_rectangle;
}

RectangleShape createBoundingRectangle(FloatRect rect, Color color){

    RectangleShape new_rectangle(Vector2f(rect.width, rect.height));
    new_rectangle.setOrigin(rect.width/2.0, rect.height/2.0);
    new_rectangle.setPosition(rect.left + (rect.width/2.0), rect.top + (rect.height/2.0));
    new_rectangle.setFillColor(Color(0,0,0,0));
    new_rectangle.setOutlineColor(color);
    new_rectangle.setOutlineThickness(4);

    return new_rectangle;
}

CircleShape createBoundingCircle(Sprite sprite, Color color, double margin){

    double radius = (hypot(sprite.getLocalBounds().width, sprite.getLocalBounds().height)/2) + margin;
    CircleShape new_circle(radius);
    new_circle.setOrigin(radius, radius);
    new_circle.setPosition(sprite.getPosition());
    new_circle.setFillColor(Color(0,0,0,0));
    new_circle.setOutlineColor(color);
    new_circle.setOutlineThickness(2.5);

    return new_circle;
}

CircleShape createCircle(double radius, Vector2f position, Color colour){

    CircleShape new_circle(radius);
    new_circle.setOrigin(radius, radius);
    new_circle.setPosition(position);
    new_circle.setFillColor(colour);
    new_circle.setOutlineColor(Color(0,0,0,0));
    new_circle.setOutlineThickness(0);

    return new_circle;
}

RectangleShape createRectangle(Vector2f new_position, Vector2f new_size, int outline_width, Color fill_color, Color outline_color){

    RectangleShape new_rectangle(new_size);
    new_rectangle.setOrigin(new_size.x/2.0, new_size.y/2.0);
    new_rectangle.setOutlineThickness(outline_width);
    new_rectangle.setPosition(new_position);
    new_rectangle.setFillColor(fill_color);
    new_rectangle.setOutlineColor(outline_color);


    return new_rectangle;
}

RectangleShape createLine(Vector2f start_position, Vector2f axis, double length, Color color){

    RectangleShape new_rectangle;
    new_rectangle.setSize(Vector2f(3, length));
    new_rectangle.setOrigin(1.5,0);
    new_rectangle.setPosition(start_position);
    new_rectangle.setOutlineThickness(0);
    new_rectangle.setFillColor(color);
    new_rectangle.setRotation(getRotationFromAxis(axis));

    return new_rectangle;
}

Sprite createSprite(string texture_id, Vector2f position, string bias){

    Sprite sprite = Sprite(*getTexture(texture_id));
    sprite.setTextureRect(getTextureRect(texture_id));
    FloatRect rect = sprite.getGlobalBounds();

    if(bias == "left"){
        sprite.setOrigin(0,rect.height/2);
    }
    else if(bias == "middle"){
        sprite.setOrigin(rect.width/2,rect.height/2);
    }
    else if(bias == "bottom middle"){
        sprite.setOrigin(rect.width/2,rect.height);
    }
    else if(bias == "right"){
        sprite.setOrigin(rect.width,rect.height/2);
    }

    sprite.setPosition(position.x,position.y);
    return sprite;
}

Sprite* createNewSprite(string texture_id, Vector2f position, string bias){

    Sprite* sprite = new Sprite(*getTexture(texture_id));
    sprite->setTextureRect(getTextureRect(texture_id));
    FloatRect rect = sprite->getGlobalBounds();

    if(bias == "left"){
        sprite->setOrigin(0,rect.height/2);
    }
    else if(bias == "middle"){
        sprite->setOrigin(rect.width/2,rect.height/2);
    }
    else if(bias == "bottom middle"){
        sprite->setOrigin(rect.width/2,rect.height);
    }
    else if(bias == "right"){
        sprite->setOrigin(rect.width,rect.height/2);
    }

    sprite->setPosition(position.x,position.y);
    return sprite;
}

string asString(int number)
{
    stringstream converter;
    string returnstring = "";
    converter << number;
    getline(converter, returnstring);
    return returnstring;
}

string asString(double number)
{
    stringstream converter;
    string returnstring = "";
    converter << number;
    getline(converter, returnstring);
    return returnstring;
}


Vector2f operator *(const Vector2f& left, const Vector2f& right){

    double X = left.x * right.x;
    double Y = left.y * right.y;
    return Vector2f(X,Y);
}

Vector2f operator /(const Vector2f& left, const Vector2f& right){

    double X = left.x / right.x;
    double Y = left.y / right.y;
    return Vector2f(X,Y);
}

Vector2f operator +(const Vector2f& left, const double& right)
{
    double X = left.x + right;
    double Y = left.y + right;
    return Vector2f(X,Y);
}

Vector2f operator -(const Vector2f& left, const double& right)
{
    double X = left.x - right;
    double Y = left.y - right;
    return Vector2f(X,Y);
}

Vector2f operator *(const Vector2f& left, const double& right)
{
    double X = left.x * right;
    double Y = left.y * right;
    return Vector2f(X,Y);
}

Vector2f operator /(const Vector2f& left, const double& right)
{
    double X = left.x / right;
    double Y = left.y / right;
    return Vector2f(X,Y);
}

void loadConfigs(){

	TiXmlDocument doc("sheets/index.xml");
    doc.LoadFile();

    for(TiXmlElement* parser = doc.FirstChildElement("Tx"); parser!=NULL; parser = parser->NextSiblingElement("Tx"))
    {
        string name = parser->Attribute("id");
        IntRect rect;
        rect.width = strtod(parser->Attribute("width"),NULL);
        rect.height = strtod(parser->Attribute("height"),NULL);
        rect.left = 0;
        rect.top = 0;

 		Texture* texture = new Texture();

 		string path = parser->Attribute("file_path");
 		texture->loadFromFile("sheets/" + path);
 		string tiled = parser->Attribute("tiled");
 		if(tiled=="yes"){
 			texture->setRepeated(true);
 		}

 		texture_rects[name] = rect;
 		textures[name] = texture;

 		
    }

    TiXmlDocument doc2("tile_index.xml");
    doc2.LoadFile();

    for(TiXmlElement* parser = doc2.FirstChildElement("Tile"); parser!=NULL; parser = parser->NextSiblingElement("Tile"))
    {
    	TileProperties new_properties = TileProperties();

        new_properties.type_name = parser->Attribute("name");
        new_properties.type_id = static_cast<int>(strtod(parser->Attribute("id"),NULL));
        for(TiXmlElement* i = parser->FirstChildElement("index"); i != NULL; i = i->NextSiblingElement("index")){

        	new_properties.texture_indexes.push_back(static_cast<int>(strtod(i->GetText(),NULL)));
        }
        string collidable = parser->Attribute("collidable");
        if(collidable == "true"){ collidable_terrain_types.insert(new_properties.type_id); }
        tile_properties[new_properties.type_id] = new_properties;
    }

    TiXmlDocument doc3("structures.xml");
    doc3.LoadFile();

    for(TiXmlElement* parser = doc3.FirstChildElement("Structure"); parser!=NULL; parser = parser->NextSiblingElement("Structure"))
    {
        StructureProperties new_properties = StructureProperties();

        new_properties.type_name = parser->Attribute("name");

        if(parser->Attribute("tiles_across") != NULL){
            new_properties.grid_size.x = static_cast<int>(strtod(parser->Attribute("tiles_across"),NULL));
        }
        if(parser->Attribute("tiles_up") != NULL){
            new_properties.grid_size.y = static_cast<int>(strtod(parser->Attribute("tiles_up"),NULL));
        }
        if(parser->Attribute("grid_origin_x") != NULL){
            new_properties.grid_origin.x = strtod(parser->Attribute("grid_origin_x"),NULL);
        }
        if(parser->Attribute("grid_origin_y") != NULL){
            new_properties.grid_origin.y = strtod(parser->Attribute("grid_origin_y"),NULL);
        }

        new_properties.backdrop_texture_id = parser->Attribute("backdrop_texture_id");
        new_properties.door_texture_id = parser->Attribute("door_texture_id");
        new_properties.exterior_texture_id = parser->Attribute("exterior_texture_id");
        
        if(parser->Attribute("exterior_offset_x") != NULL){
            new_properties.exterior_offset.x = strtod(parser->Attribute("exterior_offset_x"),NULL);
        }
        if(parser->Attribute("exterior_offset_y") != NULL){
            new_properties.exterior_offset.y = strtod(parser->Attribute("exterior_offset_y"),NULL);
        }
        if(parser->Attribute("backdrop_offset_x") != NULL){
            new_properties.backdrop_offset.x = strtod(parser->Attribute("backdrop_offset_x"),NULL);
        }
        if(parser->Attribute("backdrop_offset_y") != NULL){
            new_properties.backdrop_offset.y = strtod(parser->Attribute("backdrop_offset_y"),NULL);
        }

        for(TiXmlElement* i = parser->FirstChildElement("tile"); i != NULL; i = i->NextSiblingElement("tile")){

            new_properties.grid[static_cast<int>(strtod(i->Attribute("x"),NULL))][static_cast<int>(strtod(i->Attribute("y"),NULL))] = static_cast<int>(strtod(i->GetText(),NULL));
        }
        for(TiXmlElement* i = parser->FirstChildElement("deco"); i != NULL; i = i->NextSiblingElement("deco")){

            new_properties.deco_position[i->Attribute("id")] = Vector2f(strtod(i->Attribute("x"),NULL),strtod(i->Attribute("y"),NULL));
            new_properties.deco_texture_id[i->Attribute("id")] = i->GetText();
        }

        structure_properties[new_properties.type_name] = new_properties;
    }

    TiXmlDocument doc4("ships.xml");
    doc4.LoadFile();

    for(TiXmlElement* parser = doc4.FirstChildElement("Ship"); parser!=NULL; parser = parser->NextSiblingElement("Ship"))
    {
        ShipProperties new_properties = ShipProperties();

        new_properties.type_name = parser->Attribute("name");
        new_properties.texture_id = parser->Attribute("texture_id");
        new_properties.icon_id = parser->Attribute("icon_id");
        
        if(parser->Attribute("parent") != NULL){
            new_properties.parent = parser->Attribute("parent");
        }
        if(parser->Attribute("render_order") != NULL){
            new_properties.render_order = static_cast<int>(strtod(parser->Attribute("render_order"),NULL));
        }
        if(parser->Attribute("x") != NULL){
            new_properties.relative_position.x = strtod(parser->Attribute("x"),NULL);
        }
        if(parser->Attribute("y") != NULL){
            new_properties.relative_position.y = strtod(parser->Attribute("y"),NULL);
        }
        if(parser->FirstChildElement("max_gear") != NULL){
            new_properties.max_gear = static_cast<int>(strtod(parser->FirstChildElement("max_gear")->GetText(),NULL));
        }
        if(parser->FirstChildElement("max_fuel") != NULL){
            new_properties.max_fuel = strtod(parser->FirstChildElement("max_fuel")->GetText(),NULL);
        }
        if(parser->FirstChildElement("max_speed") != NULL){
            new_properties.max_speed = strtod(parser->FirstChildElement("max_speed")->GetText(),NULL);
        }
        if(parser->FirstChildElement("handling") != NULL){
            new_properties.handling = strtod(parser->FirstChildElement("handling")->GetText(),NULL);
        }
        if(parser->FirstChildElement("fuel_consumption") != NULL){
            new_properties.fuel_consumption = strtod(parser->FirstChildElement("fuel_consumption")->GetText(),NULL);
        }

        for(TiXmlElement* i = parser->FirstChildElement("anim"); i != NULL; i = i->NextSiblingElement("anim")){

            new_properties.start_index[i->GetText()] = static_cast<int>(strtod(i->Attribute("start"),NULL));
            new_properties.end_index[i->GetText()] = static_cast<int>(strtod(i->Attribute("end"),NULL));
            string looping = i->Attribute("looping");
            if(looping=="yes"){
                new_properties.is_looping[i->GetText()] = true;
            }
            else{
                new_properties.is_looping[i->GetText()] = false;
            }
        }

        ship_properties[new_properties.type_name] = new_properties;
    }

    TiXmlDocument doc5("characters.xml");
    doc5.LoadFile();

    for(TiXmlElement* parser = doc5.FirstChildElement("Character"); parser!=NULL; parser = parser->NextSiblingElement("Character"))
    {
        CharacterProperties new_properties = CharacterProperties();

        new_properties.type_name = parser->Attribute("name");
        new_properties.texture_id = parser->Attribute("texture_id");

        for(TiXmlElement* i = parser->FirstChildElement("anim"); i != NULL; i = i->NextSiblingElement("anim")){

            new_properties.start_index[i->GetText()] = static_cast<int>(strtod(i->Attribute("start"),NULL));
            new_properties.end_index[i->GetText()] = static_cast<int>(strtod(i->Attribute("end"),NULL));
            string looping = i->Attribute("looping");
            if(looping=="yes"){
                new_properties.is_looping[i->GetText()] = true;
            }
            else{
                new_properties.is_looping[i->GetText()] = false;
            }
        }

        character_properties[new_properties.type_name] = new_properties;
    }
}

void destroyTextures(){

	for(map<string, Texture*>::iterator i = textures.begin(); i != textures.end(); i++){
		delete i->second;
	}
}


Texture* getTexture(string key){

	return textures[key];
}

IntRect getTextureRect(string key){

	return texture_rects[key];
}

IntRect getFrame(int frame_index, Sprite &sprite){

    IntRect frame_window = sprite.getTextureRect();
    double x_index = (int(frame_index * frame_window.width) % int(sprite.getTexture()->getSize().x))/double(frame_window.width);
    double y_index = ((frame_index - x_index)*frame_window.width)/sprite.getTexture()->getSize().x;

    frame_window.left = int(x_index*frame_window.width);
    frame_window.top = int(y_index*frame_window.height);
    
    return frame_window;
}

double getRotationFromAxis(Vector2f axis)
{
    double theta = 0;
    double h = hypot(axis.x,axis.y);

    if(axis.x>=0 and axis.y < 0)//quad 1
    {
        theta = 180/M_PI*asin(abs(axis.x)/h)+0;
    }
    else if(axis.x>=0 and axis.y >= 0)//quad 2
    {
        theta = 180/M_PI*asin(abs(axis.y)/h)+90;
    }
    else if(axis.x<0 and axis.y >= 0)//quad 3
    {
        theta = 180/M_PI*asin(abs(axis.x)/h)+180;
    }
    else if(axis.x<0 and axis.y <0)//quad 4
    {
        theta = 180/M_PI*asin(abs(axis.y)/h)+270;
    }

    return theta;
}

double dot(Vector2f a, Vector2f b){

	return ((a.x*b.x) + (a.y*b.y));
}

double returnHigher(double a, double b){

	if(a > b){ return a;}
	return b;
}

double returnLower(double a, double b){

	if(a < b){ return a;}
	return b;
}

Vector2f rotateAboutOrigin(Vector2f p, double degrees){
	//this doesn't get used
	double theta = degrees*(M_PI/180.0);
	return Vector2f((p.x*cos(theta))-(p.y*sin(theta)),(p.x*sin(theta))+(p.y*cos(theta)));
}

Vector2f normalize(Vector2f v){

	double h = hypot(v.x,v.y);
	Vector2f return_v = Vector2f(v.x/h,v.y/h);
	return return_v;
}

