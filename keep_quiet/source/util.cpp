#include "main.h"

using namespace std;
using namespace sf;

extern map<string, Texture*> textures;
extern map<string, IntRect> texture_rects;
extern set<int> collidable_terrain_types;
extern map<int, TileProperties> tile_properties;
extern map<string, StructureProperties> structure_properties;
extern map<string, Font> fonts;

TileProperties::TileProperties(){}

StructureProperties::StructureProperties(){

    max_workers = 0;
    max_ammunition = 0;
    max_fuel = 0;
    power_contribution = 0;
    construction_contribution = 0;
    supply_contribution = 0;
    fuel_consumption = 0;
    construction_cost = 0;
}

int StructureProperties::getStartFrame(string anim_name){

    return anim_starts[anim_name];
}

int StructureProperties::getEndFrame(string anim_name){

    return anim_ends[anim_name];
}

Worker::Worker(){

    tasked_structure_id = "";
}

Structure::Structure(){

    contributing = false;
}

void Structure::update(double dt, int total_construction, int surplus_power){

    

    //if construction of the structure has not been completed:
    if(construction_progress < structure_properties[type_name].construction_cost){
        
        //display tasked/max workers above structure
        int max_construction_workers = 4;
        for(int index = 0; index < max_construction_workers; index++){

            if(index < tasked_workers.size()){
                //display stick-man as filled instead of hollow to indicate the tasked worker
            }
        }
        //

        double construction_rate = total_construction * tasked_workers.size();
        construction_progress += (construction_rate*dt);
        //determine texture to use
        if(construction_progress >= structure_properties[type_name].construction_cost){
            //if progress is completed set texture to complete texture

            //also dismiss all construction workers
            for(vector<Worker*>::iterator i = tasked_workers.begin(); i != tasked_workers.end(); i++){
                Worker* w = *i;
                w->tasked_structure_id = "";
            }
            tasked_workers.clear();
            //
        }
        return; //don't bother displaying any other info if still in construction
    }
    //

    //display tasked/max workers above structure
    for(int index = 0; index < structure_properties[type_name].max_workers; index++){

        if(index < tasked_workers.size()){
            //display stick-man as filled instead of hollow to indicate the tasked worker
        }
    }
    //

    //display power, construction and food contributions if non-zero. If power is required but not provided (power_contribution < 0, base power < required) or max_workers > 0 and no workers are tasked or fuel_consumption*dt > fuel, then display contributions in grey
    contributing = !(structure_properties[type_name].power_contribution < 0 && surplus_power < 0) && !(structure_properties[type_name].max_workers > 0 && tasked_workers.size() == 0) && !(structure_properties[type_name].fuel_consumption*dt > fuel);
    if(contributing){
        int index = 0; //this tracks which contributions were displayed so that the subsequent one can shift over to make room
        if(structure_properties[type_name].power_contribution != 0){
            index++;
        }
        if(structure_properties[type_name].construction_contribution != 0){
            index++;
        }
        if(structure_properties[type_name].supply_contribution != 0){

        }
    }
    //

    //randomly animate

    //  
    
}

void Structure::draw(RenderWindow &window){

    window.draw(sprite);
}

char randChar()
{
    return (char)( (rand() % 52) + 65);
}

int randInt(int ubound){

	return (rand() % ubound)+1;
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


RectangleShape createBoundingRectangle(Sprite sprite, Color color){

    RectangleShape new_rectangle(Vector2f(sprite.getTextureRect().width, sprite.getTextureRect().height));
    new_rectangle.setOrigin(sprite.getTextureRect().width/2, sprite.getTextureRect().height/2);
    new_rectangle.setRotation(sprite.getRotation());
    new_rectangle.setPosition(sprite.getPosition());
    new_rectangle.setFillColor(Color(0,0,0,0));
    new_rectangle.setOutlineColor(color);
    new_rectangle.setOutlineThickness(2.5);

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
    FloatRect rect = sprite.getGlobalBounds();

    if(bias == "left"){
        sprite.setOrigin(0,rect.height/2);
    }
    else if(bias == "middle"){
        sprite.setOrigin(rect.width/2,rect.height/2);
    }
    else if(bias == "right"){
        sprite.setOrigin(rect.width,rect.height/2);
    }

    sprite.setPosition(position.x,position.y);
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
        new_properties.texture_id = parser->Attribute("texture_id");
        new_properties.icon_id = parser->Attribute("icon_id");

        if(parser->FirstChildElement("max_workers") != NULL){
            new_properties.max_workers = static_cast<int>(strtod(parser->FirstChildElement("max_workers")->GetText(),NULL));
        }
        if(parser->FirstChildElement("max_ammunition") != NULL){
            new_properties.max_ammunition = static_cast<int>(strtod(parser->FirstChildElement("max_ammunition")->GetText(),NULL));
        }
        if(parser->FirstChildElement("max_fuel") != NULL){
            new_properties.max_fuel = static_cast<int>(strtod(parser->FirstChildElement("max_fuel")->GetText(),NULL));
        }
        if(parser->FirstChildElement("power_contribution") != NULL){
            new_properties.power_contribution = static_cast<int>(strtod(parser->FirstChildElement("power_contribution")->GetText(),NULL));
        }
        if(parser->FirstChildElement("construction_contribution") != NULL){
            new_properties.power_contribution = static_cast<int>(strtod(parser->FirstChildElement("construction_contribution")->GetText(),NULL));
        }
        if(parser->FirstChildElement("supply_contribution") != NULL){
            new_properties.supply_contribution = static_cast<int>(strtod(parser->FirstChildElement("supply_contribution")->GetText(),NULL));
        }
        if(parser->FirstChildElement("fuel_consumption") != NULL){
            new_properties.fuel_consumption = strtod(parser->FirstChildElement("fuel_consumption")->GetText(),NULL);
        }
        if(parser->FirstChildElement("construction_cost") != NULL){
            new_properties.construction_cost = strtod(parser->FirstChildElement("construction_cost")->GetText(),NULL);
        }

        for(TiXmlElement* i = parser->FirstChildElement("anim"); i != NULL; i = i->NextSiblingElement("anim")){

            new_properties.anim_starts[i->GetText()] = static_cast<int>(strtod(i->Attribute("start"),NULL));
            new_properties.anim_ends[i->GetText()] = static_cast<int>(strtod(i->Attribute("end"),NULL));
        }

        structure_properties[new_properties.type_name] = new_properties;
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

Vector2f wrapPoint(Vector2f point, int terrain_max_x){

	Vector2f return_point = point;
	if(point.x < 0){ return_point.x = (terrain_max_x*64.0) + point.x; }
	if(point.x > terrain_max_x*64.0){ return_point.x = point.x - (terrain_max_x*64.0); }
	return return_point;
}

void untangleSprites(Sprite &a, Sprite &b){

	//determine how much the sprites are overlapping on each of the 4 axises defined by their rotations
	//move the sprites apart along the axis with the least

    FloatRect rect_a = a.getLocalBounds();
    FloatRect rect_b = b.getLocalBounds();
	
	Vector2f a_tr = Vector2f(rect_a.width/2,-rect_a.height/2);
    Vector2f a_tl = Vector2f(-rect_a.width/2,-rect_a.height/2);
    Vector2f a_br = Vector2f(rect_a.width/2,rect_a.height/2);
    Vector2f a_bl = Vector2f(-rect_a.width/2,rect_a.height/2);

    Vector2f a_ctr = Vector2f(a.getPosition().x, a.getPosition().y);

    Vector2f b_tr = Vector2f(rect_b.width/2,-rect_b.height/2);
    Vector2f b_tl = Vector2f(-rect_b.width/2,-rect_b.height/2);
    Vector2f b_br = Vector2f(rect_b.width/2,rect_b.height/2);
    Vector2f b_bl = Vector2f(-rect_b.width/2,rect_b.height/2);

    Vector2f b_ctr = Vector2f(b.getPosition().x, b.getPosition().y);

    Vector2f axis_1 = normalize(a_bl - a_tl);
    Vector2f axis_2 = normalize(a_tr - a_tl);
    Vector2f axis_3 = normalize(b_bl - b_tl);
    Vector2f axis_4 = normalize(b_tr - b_tl);

    double overlap_1 = 0;
	double overlap_2 = 0;
	double overlap_3 = 0;
	double overlap_4 = 0;
	bool a_before_b_1 = dot(axis_1, a_ctr) < dot(axis_1, b_ctr);
	bool a_before_b_2 = dot(axis_2, a_ctr) < dot(axis_2, b_ctr);
	bool a_before_b_3 = dot(axis_3, a_ctr) < dot(axis_3, b_ctr);
	bool a_before_b_4 = dot(axis_4, a_ctr) < dot(axis_4, b_ctr);

	a_tr = a_ctr + a_tr;
	a_tl = a_ctr + a_tl;
	a_br = a_ctr + a_br;
	a_bl = a_ctr + a_bl;

	b_tr = b_ctr + b_tr;
	b_tl = b_ctr + b_tl;
	b_br = b_ctr + b_br;
	b_bl = b_ctr + b_bl;

    if(a_before_b_1){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_1, a_tr);
    	if(dot(axis_1, a_tl) > max_a){ max_a = dot(axis_1, a_tl); }
    	if(dot(axis_1, a_bl) > max_a){ max_a = dot(axis_1, a_bl); }
    	if(dot(axis_1, a_br) > max_a){ max_a = dot(axis_1, a_br); }

    	double min_b = dot(axis_1, b_tr);
    	if(dot(axis_1, b_tl) < min_b){ min_b = dot(axis_1, b_tl); }
    	if(dot(axis_1, b_bl) < min_b){ min_b = dot(axis_1, b_bl); }
    	if(dot(axis_1, b_br) < min_b){ min_b = dot(axis_1, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_1 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_1, b_tr);
    	if(dot(axis_1, b_tl) > max_b){ max_b = dot(axis_1, b_tl); }
    	if(dot(axis_1, b_bl) > max_b){ max_b = dot(axis_1, b_bl); }
    	if(dot(axis_1, b_br) > max_b){ max_b = dot(axis_1, b_br); }

    	double min_a = dot(axis_1, a_tr);
    	if(dot(axis_1, a_tl) < min_a){ min_a = dot(axis_1, a_tl); }
    	if(dot(axis_1, a_bl) < min_a){ min_a = dot(axis_1, a_bl); }
    	if(dot(axis_1, a_br) < min_a){ min_a = dot(axis_1, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_1 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }
    if(a_before_b_2){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_2, a_tr);
    	if(dot(axis_2, a_tl) > max_a){ max_a = dot(axis_2, a_tl); }
    	if(dot(axis_2, a_bl) > max_a){ max_a = dot(axis_2, a_bl); }
    	if(dot(axis_2, a_br) > max_a){ max_a = dot(axis_2, a_br); }

    	double min_b = dot(axis_2, b_tr);
    	if(dot(axis_2, b_tl) < min_b){ min_b = dot(axis_2, b_tl); }
    	if(dot(axis_2, b_bl) < min_b){ min_b = dot(axis_2, b_bl); }
    	if(dot(axis_2, b_br) < min_b){ min_b = dot(axis_2, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_2 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_2, b_tr);
    	if(dot(axis_2, b_tl) > max_b){ max_b = dot(axis_2, b_tl); }
    	if(dot(axis_2, b_bl) > max_b){ max_b = dot(axis_2, b_bl); }
    	if(dot(axis_2, b_br) > max_b){ max_b = dot(axis_2, b_br); }

    	double min_a = dot(axis_2, a_tr);
    	if(dot(axis_2, a_tl) < min_a){ min_a = dot(axis_2, a_tl); }
    	if(dot(axis_2, a_bl) < min_a){ min_a = dot(axis_2, a_bl); }
    	if(dot(axis_2, a_br) < min_a){ min_a = dot(axis_2, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_2 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }
    if(a_before_b_3){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_3, a_tr);
    	if(dot(axis_3, a_tl) > max_a){ max_a = dot(axis_3, a_tl); }
    	if(dot(axis_3, a_bl) > max_a){ max_a = dot(axis_3, a_bl); }
    	if(dot(axis_3, a_br) > max_a){ max_a = dot(axis_3, a_br); }

    	double min_b = dot(axis_3, b_tr);
    	if(dot(axis_3, b_tl) < min_b){ min_b = dot(axis_3, b_tl); }
    	if(dot(axis_3, b_bl) < min_b){ min_b = dot(axis_3, b_bl); }
    	if(dot(axis_3, b_br) < min_b){ min_b = dot(axis_3, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_3 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_3, b_tr);
    	if(dot(axis_3, b_tl) > max_b){ max_b = dot(axis_3, b_tl); }
    	if(dot(axis_3, b_bl) > max_b){ max_b = dot(axis_3, b_bl); }
    	if(dot(axis_3, b_br) > max_b){ max_b = dot(axis_3, b_br); }

    	double min_a = dot(axis_3, a_tr);
    	if(dot(axis_3, a_tl) < min_a){ min_a = dot(axis_3, a_tl); }
    	if(dot(axis_3, a_bl) < min_a){ min_a = dot(axis_3, a_bl); }
    	if(dot(axis_3, a_br) < min_a){ min_a = dot(axis_3, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_3 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }
    if(a_before_b_4){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_4, a_tr);
    	if(dot(axis_4, a_tl) > max_a){ max_a = dot(axis_4, a_tl); }
    	if(dot(axis_4, a_bl) > max_a){ max_a = dot(axis_4, a_bl); }
    	if(dot(axis_4, a_br) > max_a){ max_a = dot(axis_4, a_br); }

    	double min_b = dot(axis_4, b_tr);
    	if(dot(axis_4, b_tl) < min_b){ min_b = dot(axis_4, b_tl); }
    	if(dot(axis_4, b_bl) < min_b){ min_b = dot(axis_4, b_bl); }
    	if(dot(axis_4, b_br) < min_b){ min_b = dot(axis_4, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_4 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_4, b_tr);
    	if(dot(axis_4, b_tl) > max_b){ max_b = dot(axis_4, b_tl); }
    	if(dot(axis_4, b_bl) > max_b){ max_b = dot(axis_4, b_bl); }
    	if(dot(axis_4, b_br) > max_b){ max_b = dot(axis_4, b_br); }

    	double min_a = dot(axis_4, a_tr);
    	if(dot(axis_4, a_tl) < min_a){ min_a = dot(axis_4, a_tl); }
    	if(dot(axis_4, a_bl) < min_a){ min_a = dot(axis_4, a_bl); }
    	if(dot(axis_4, a_br) < min_a){ min_a = dot(axis_4, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_4 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }

    double lowest_overlap = returnLower(returnLower(returnLower(overlap_1, overlap_2),overlap_3),overlap_4);
    Vector2f b_displacement;
    Vector2f a_displacement;
    if(lowest_overlap == overlap_1){

    	if(a_before_b_1){

    		b_displacement = axis_1*(overlap_1/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_1*(overlap_1/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }
    else if(lowest_overlap == overlap_2){

    	if(a_before_b_2){

    		b_displacement = axis_2*(overlap_2/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_2*(overlap_2/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }
    else if(lowest_overlap == overlap_3){

    	if(a_before_b_3){

    		b_displacement = axis_3*(overlap_3/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_3*(overlap_3/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }
    else if(lowest_overlap == overlap_4){

    	if(a_before_b_4){

    		b_displacement = axis_4*(overlap_4/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_4*(overlap_4/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }

    a.move(a_displacement.x,a_displacement.y);
    b.move(b_displacement.x,b_displacement.y);
}

void untangleSprite(Sprite &sprite, FloatRect rect, int terrain_max_x){

	//determine how much the sprites are overlapping on each of the 4 axises defined by their rotations
	//move the sprites apart along the axis with the least

    FloatRect rect_a = sprite.getLocalBounds();
    FloatRect rect_b = rect;
	
	Vector2f a_tr = Vector2f(rect_a.width/2,-rect_a.height/2);
    Vector2f a_tl = Vector2f(-rect_a.width/2,-rect_a.height/2);
    Vector2f a_br = Vector2f(rect_a.width/2,rect_a.height/2);
    Vector2f a_bl = Vector2f(-rect_a.width/2,rect_a.height/2);

    Vector2f a_ctr = wrapPoint(Vector2f(sprite.getPosition().x, sprite.getPosition().y), terrain_max_x);

    Vector2f b_tr = Vector2f(rect_b.width/2,-rect_b.height/2);
    Vector2f b_tl = Vector2f(-rect_b.width/2,-rect_b.height/2);
    Vector2f b_br = Vector2f(rect_b.width/2,rect_b.height/2);
    Vector2f b_bl = Vector2f(-rect_b.width/2,rect_b.height/2);

    Vector2f b_ctr = wrapPoint(Vector2f(rect.left + (rect.width/2.0), rect.top + (rect.height/2.0)), terrain_max_x);

    Vector2f axis_1 = normalize(a_bl - a_tl);
    Vector2f axis_2 = normalize(a_tr - a_tl);
    Vector2f axis_3 = normalize(b_bl - b_tl);
    Vector2f axis_4 = normalize(b_tr - b_tl);

    double overlap_1 = 0;
	double overlap_2 = 0;
	double overlap_3 = 0;
	double overlap_4 = 0;
	bool a_before_b_1 = dot(axis_1, a_ctr) < dot(axis_1, b_ctr);
	bool a_before_b_2 = dot(axis_2, a_ctr) < dot(axis_2, b_ctr);
	bool a_before_b_3 = dot(axis_3, a_ctr) < dot(axis_3, b_ctr);
	bool a_before_b_4 = dot(axis_4, a_ctr) < dot(axis_4, b_ctr);

	a_tr = wrapPoint(a_ctr + rotateAboutOrigin(a_tr,sprite.getRotation()), terrain_max_x);
	a_tl = wrapPoint(a_ctr + rotateAboutOrigin(a_tl,sprite.getRotation()), terrain_max_x);
	a_br = wrapPoint(a_ctr + rotateAboutOrigin(a_br,sprite.getRotation()), terrain_max_x);
	a_bl = wrapPoint(a_ctr + rotateAboutOrigin(a_bl,sprite.getRotation()), terrain_max_x);

	b_tr = wrapPoint(b_ctr + b_tr, terrain_max_x);
	b_tl = wrapPoint(b_ctr + b_tl, terrain_max_x);
	b_br = wrapPoint(b_ctr + b_br, terrain_max_x);
	b_bl = wrapPoint(b_ctr + b_bl, terrain_max_x);

    if(a_before_b_1){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_1, a_tr);
    	if(dot(axis_1, a_tl) > max_a){ max_a = dot(axis_1, a_tl); }
    	if(dot(axis_1, a_bl) > max_a){ max_a = dot(axis_1, a_bl); }
    	if(dot(axis_1, a_br) > max_a){ max_a = dot(axis_1, a_br); }

    	double min_b = dot(axis_1, b_tr);
    	if(dot(axis_1, b_tl) < min_b){ min_b = dot(axis_1, b_tl); }
    	if(dot(axis_1, b_bl) < min_b){ min_b = dot(axis_1, b_bl); }
    	if(dot(axis_1, b_br) < min_b){ min_b = dot(axis_1, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_1 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_1, b_tr);
    	if(dot(axis_1, b_tl) > max_b){ max_b = dot(axis_1, b_tl); }
    	if(dot(axis_1, b_bl) > max_b){ max_b = dot(axis_1, b_bl); }
    	if(dot(axis_1, b_br) > max_b){ max_b = dot(axis_1, b_br); }

    	double min_a = dot(axis_1, a_tr);
    	if(dot(axis_1, a_tl) < min_a){ min_a = dot(axis_1, a_tl); }
    	if(dot(axis_1, a_bl) < min_a){ min_a = dot(axis_1, a_bl); }
    	if(dot(axis_1, a_br) < min_a){ min_a = dot(axis_1, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_1 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }
    if(a_before_b_2){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_2, a_tr);
    	if(dot(axis_2, a_tl) > max_a){ max_a = dot(axis_2, a_tl); }
    	if(dot(axis_2, a_bl) > max_a){ max_a = dot(axis_2, a_bl); }
    	if(dot(axis_2, a_br) > max_a){ max_a = dot(axis_2, a_br); }

    	double min_b = dot(axis_2, b_tr);
    	if(dot(axis_2, b_tl) < min_b){ min_b = dot(axis_2, b_tl); }
    	if(dot(axis_2, b_bl) < min_b){ min_b = dot(axis_2, b_bl); }
    	if(dot(axis_2, b_br) < min_b){ min_b = dot(axis_2, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_2 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_2, b_tr);
    	if(dot(axis_2, b_tl) > max_b){ max_b = dot(axis_2, b_tl); }
    	if(dot(axis_2, b_bl) > max_b){ max_b = dot(axis_2, b_bl); }
    	if(dot(axis_2, b_br) > max_b){ max_b = dot(axis_2, b_br); }

    	double min_a = dot(axis_2, a_tr);
    	if(dot(axis_2, a_tl) < min_a){ min_a = dot(axis_2, a_tl); }
    	if(dot(axis_2, a_bl) < min_a){ min_a = dot(axis_2, a_bl); }
    	if(dot(axis_2, a_br) < min_a){ min_a = dot(axis_2, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_2 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }
    if(a_before_b_3){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_3, a_tr);
    	if(dot(axis_3, a_tl) > max_a){ max_a = dot(axis_3, a_tl); }
    	if(dot(axis_3, a_bl) > max_a){ max_a = dot(axis_3, a_bl); }
    	if(dot(axis_3, a_br) > max_a){ max_a = dot(axis_3, a_br); }

    	double min_b = dot(axis_3, b_tr);
    	if(dot(axis_3, b_tl) < min_b){ min_b = dot(axis_3, b_tl); }
    	if(dot(axis_3, b_bl) < min_b){ min_b = dot(axis_3, b_bl); }
    	if(dot(axis_3, b_br) < min_b){ min_b = dot(axis_3, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_3 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_3, b_tr);
    	if(dot(axis_3, b_tl) > max_b){ max_b = dot(axis_3, b_tl); }
    	if(dot(axis_3, b_bl) > max_b){ max_b = dot(axis_3, b_bl); }
    	if(dot(axis_3, b_br) > max_b){ max_b = dot(axis_3, b_br); }

    	double min_a = dot(axis_3, a_tr);
    	if(dot(axis_3, a_tl) < min_a){ min_a = dot(axis_3, a_tl); }
    	if(dot(axis_3, a_bl) < min_a){ min_a = dot(axis_3, a_bl); }
    	if(dot(axis_3, a_br) < min_a){ min_a = dot(axis_3, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_3 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }
    if(a_before_b_4){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_4, a_tr);
    	if(dot(axis_4, a_tl) > max_a){ max_a = dot(axis_4, a_tl); }
    	if(dot(axis_4, a_bl) > max_a){ max_a = dot(axis_4, a_bl); }
    	if(dot(axis_4, a_br) > max_a){ max_a = dot(axis_4, a_br); }

    	double min_b = dot(axis_4, b_tr);
    	if(dot(axis_4, b_tl) < min_b){ min_b = dot(axis_4, b_tl); }
    	if(dot(axis_4, b_bl) < min_b){ min_b = dot(axis_4, b_bl); }
    	if(dot(axis_4, b_br) < min_b){ min_b = dot(axis_4, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_4 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_4, b_tr);
    	if(dot(axis_4, b_tl) > max_b){ max_b = dot(axis_4, b_tl); }
    	if(dot(axis_4, b_bl) > max_b){ max_b = dot(axis_4, b_bl); }
    	if(dot(axis_4, b_br) > max_b){ max_b = dot(axis_4, b_br); }

    	double min_a = dot(axis_4, a_tr);
    	if(dot(axis_4, a_tl) < min_a){ min_a = dot(axis_4, a_tl); }
    	if(dot(axis_4, a_bl) < min_a){ min_a = dot(axis_4, a_bl); }
    	if(dot(axis_4, a_br) < min_a){ min_a = dot(axis_4, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_4 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }

    double lowest_overlap = returnLower(returnLower(returnLower(overlap_1, overlap_2),overlap_3),overlap_4);
    Vector2f b_displacement;
    Vector2f a_displacement;
    if(lowest_overlap == overlap_1){

    	if(a_before_b_1){

    		b_displacement = axis_1*(overlap_1/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_1*(overlap_1/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }
    else if(lowest_overlap == overlap_2){

    	if(a_before_b_2){

    		b_displacement = axis_2*(overlap_2/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_2*(overlap_2/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }
    else if(lowest_overlap == overlap_3){

    	if(a_before_b_3){

    		b_displacement = axis_3*(overlap_3/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_3*(overlap_3/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }
    else if(lowest_overlap == overlap_4){

    	if(a_before_b_4){

    		b_displacement = axis_4*(overlap_4/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_4*(overlap_4/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }

    sprite.move(a_displacement.x*2.0,a_displacement.y*2.0);

}

void keepSpriteOutOfTerrain(Sprite &sprite, Vector2f grid_ref, map<int,map<int,int> > &terrain, int terrain_max_x, int terrain_max_y){

	FloatRect rect = sprite.getLocalBounds();
	vector<Vector2f> checkpoints;

	//subdivide the texture rect into enough sample points to get an accurate check against this grid resolution
	double terrain_grid_size = 64.0;
	int checkpoints_across = ceil(rect.width/64.0);
	int checkpoints_down = ceil(rect.height/64.0);

	double cell_size_across = rect.width/checkpoints_across;
	double cell_size_down = rect.height/checkpoints_down;

	checkpoints.push_back(Vector2f(-rect.width/2,-rect.height/2));
	for(int x = 0; x <= checkpoints_across; x++){
		for(int y = 0; y <= checkpoints_down; y++){
			if(x == 0 && y == 0){ continue;}

			checkpoints.push_back(checkpoints[0] + Vector2f(x*cell_size_across,y*cell_size_down));
		}
	}
	//

	//apply sprite's rotation and global position, then find relative position to grid_ref, then get a representative terrain square if intersecting immovable terrain
	FloatRect tile_rect;
	tile_rect.width = 64.0;
	tile_rect.height = 64.0;

	for(int index = 0; index < checkpoints.size(); index++){

		checkpoints[index] = wrapPoint(sprite.getPosition() + rotateAboutOrigin(checkpoints[index], sprite.getRotation()) - grid_ref, terrain_max_x);

		int terrain_index_x = floor(checkpoints[index].x/64.0);
		int terrain_index_y = floor(checkpoints[index].y/-64.0);
		
		if(terrain_index_y > terrain_max_y){ continue; } // won't be a collision possible if this point is not in the terrain layer

		if(collidable_terrain_types.count(terrain[terrain_index_x][terrain_index_y])==0){ continue; } //if this terrain block is not collidable don't continue

		tile_rect.left = (terrain_index_x*64.0) + grid_ref.x;
		tile_rect.top = (terrain_index_y*-64.0) - 64.0 + grid_ref.y;
		
		untangleSprite(sprite, tile_rect, terrain_max_x);
	}

	//

}

bool spritesIntersecting(Sprite a, Sprite b, double tolerance){

     FloatRect rect_a = a.getLocalBounds();
     FloatRect rect_b = b.getLocalBounds();

     Vector2f a_tr = Vector2f(rect_a.width/2,-rect_a.height/2);
     Vector2f a_tl = Vector2f(-rect_a.width/2,-rect_a.height/2);
     Vector2f a_br = Vector2f(rect_a.width/2,rect_a.height/2);
     Vector2f a_bl = Vector2f(-rect_a.width/2,rect_a.height/2);

     Vector2f a_ctr = Vector2f(a.getPosition().x, a.getPosition().y);

     Vector2f b_tr = Vector2f(rect_b.width/2,-rect_b.height/2);
     Vector2f b_tl = Vector2f(-rect_b.width/2,-rect_b.height/2);
     Vector2f b_br = Vector2f(rect_b.width/2,rect_b.height/2);
     Vector2f b_bl = Vector2f(-rect_b.width/2,rect_b.height/2);

     Vector2f b_ctr = Vector2f(b.getPosition().x, b.getPosition().y);

     double rot_a = a.getRotation();
     double rot_b = b.getRotation();

     double Q_a = rot_a * (M_PI/180);
     double Q_b = rot_b * (M_PI/180);

     a_tr = a_ctr + Vector2f(a_tr.x*cos(Q_a)-a_tr.y*sin(Q_a),a_tr.x*sin(Q_a)+a_tr.y*cos(Q_a));
     a_tl = a_ctr + Vector2f(a_tl.x*cos(Q_a)-a_tl.y*sin(Q_a),a_tl.x*sin(Q_a)+a_tl.y*cos(Q_a));
     a_br = a_ctr + Vector2f(a_br.x*cos(Q_a)-a_br.y*sin(Q_a),a_br.x*sin(Q_a)+a_br.y*cos(Q_a));
     a_bl = a_ctr + Vector2f(a_bl.x*cos(Q_a)-a_bl.y*sin(Q_a),a_bl.x*sin(Q_a)+a_bl.y*cos(Q_a));

     b_tr = b_ctr + Vector2f(b_tr.x*cos(Q_b)-b_tr.y*sin(Q_b),b_tr.x*sin(Q_b)+b_tr.y*cos(Q_b));
     b_tl = b_ctr + Vector2f(b_tl.x*cos(Q_b)-b_tl.y*sin(Q_b),b_tl.x*sin(Q_b)+b_tl.y*cos(Q_b));
     b_br = b_ctr + Vector2f(b_br.x*cos(Q_b)-b_br.y*sin(Q_b),b_br.x*sin(Q_b)+b_br.y*cos(Q_b));
     b_bl = b_ctr + Vector2f(b_bl.x*cos(Q_b)-b_bl.y*sin(Q_b),b_bl.x*sin(Q_b)+b_bl.y*cos(Q_b));

     //everything above here just calculated the exact position of the four corners of each block

     vector<Vector2f> axises;
     Vector2f axis_1 = a_tr - a_tl;
     axises.push_back(axis_1/hypot(axis_1.x,axis_1.y));
     Vector2f axis_2 = a_br - a_tr;
     axises.push_back(axis_2/hypot(axis_2.x,axis_2.y));
     Vector2f axis_3 = b_tr - b_tl;
     axises.push_back(axis_3/hypot(axis_3.x,axis_3.y));
     Vector2f axis_4 = b_br - b_tr;
     axises.push_back(axis_4/hypot(axis_4.x,axis_4.y));

    //the "axises" vector is just a list of axises we want to project onto to check collision

     for(vector<Vector2f>::iterator parser = axises.begin(); parser!=axises.end(); parser++){

        double min_a = parser->x*a_tr.x+parser->y*a_tr.y;
        double max_a = parser->x*a_tr.x+parser->y*a_tr.y;
        if(parser->x*a_tl.x+parser->y*a_tl.y<min_a){min_a = parser->x*a_tl.x+parser->y*a_tl.y;}
        if(parser->x*a_tl.x+parser->y*a_tl.y>max_a){max_a = parser->x*a_tl.x+parser->y*a_tl.y;}
        if(parser->x*a_bl.x+parser->y*a_bl.y<min_a){min_a = parser->x*a_bl.x+parser->y*a_bl.y;}
        if(parser->x*a_bl.x+parser->y*a_bl.y>max_a){max_a = parser->x*a_bl.x+parser->y*a_bl.y;}
        if(parser->x*a_br.x+parser->y*a_br.y<min_a){min_a = parser->x*a_br.x+parser->y*a_br.y;}
        if(parser->x*a_br.x+parser->y*a_br.y>max_a){max_a = parser->x*a_br.x+parser->y*a_br.y;}

        double min_b = parser->x*b_tr.x+parser->y*b_tr.y;
        double max_b = parser->x*b_tr.x+parser->y*b_tr.y;
        if(parser->x*b_tl.x+parser->y*b_tl.y<min_b){min_b = parser->x*b_tl.x+parser->y*b_tl.y;}
        if(parser->x*b_tl.x+parser->y*b_tl.y>max_b){max_b = parser->x*b_tl.x+parser->y*b_tl.y;}
        if(parser->x*b_bl.x+parser->y*b_bl.y<min_b){min_b = parser->x*b_bl.x+parser->y*b_bl.y;}
        if(parser->x*b_bl.x+parser->y*b_bl.y>max_b){max_b = parser->x*b_bl.x+parser->y*b_bl.y;}
        if(parser->x*b_br.x+parser->y*b_br.y<min_b){min_b = parser->x*b_br.x+parser->y*b_br.y;}
        if(parser->x*b_br.x+parser->y*b_br.y>max_b){max_b = parser->x*b_br.x+parser->y*b_br.y;}

        double ctr_a = parser->x*a_ctr.x+parser->y*a_ctr.y;
        double ctr_b = parser->x*b_ctr.x+parser->y*b_ctr.y;

        if(abs(max_a-min_b)<abs(max_b-min_a)){
            double dist = ctr_a - ctr_b;
            double diag_a = max_a - ctr_a;
            double diag_b = min_b - ctr_b;

            if(abs(diag_a) + abs(diag_b) < abs(dist) + tolerance){
                return false;
            }
        }
        else{
            double dist = ctr_a - ctr_b;
            double diag_a = min_a - ctr_a;
            double diag_b = max_b - ctr_b;
            if(abs(diag_a) + abs(diag_b) < abs(dist) + tolerance){
                return false;
            }
        }

     }

     return true;
}

Sprite* formatSprite(Sprite* sprite, double x, double y){

	IntRect sprite_rect = sprite->getTextureRect();
	sprite->setOrigin(sprite_rect.width/2.0,sprite_rect.height/2.0);

	sprite->setPosition(x,y);
	return sprite;
}

Sprite* formatSprite(Sprite* sprite, double x, double y, double width, double height){

	IntRect sprite_rect = sprite->getTextureRect();
	sprite_rect.width = width;
	sprite_rect.height = height;
	sprite->setTextureRect(sprite_rect);

	sprite->setOrigin(sprite_rect.width/2.0,sprite_rect.height/2.0);

	sprite->setPosition(x,y);

	return sprite;
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

