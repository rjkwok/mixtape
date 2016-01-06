#include "main.h"

using namespace std;
using namespace sf;

extern map<string, Texture*> textures;
extern map<string, IntRect> texture_rects;
extern map<string, double> iso_heights;

DiamondShape::DiamondShape(){

	//Shape();

	point_a = Vector2f(0,0);
	point_b = Vector2f(0,0);
	point_c = Vector2f(0,0);
	point_d = Vector2f(0,0);
}

void DiamondShape::setHeight(double height){

	point_a.y = 0;
	point_c.y = height;
	point_b.y = height/2.0;
	point_d.y = height/2.0;
}

void DiamondShape::setWidth(double width){
	
	point_b.x = width;
	point_d.x = 0;
	point_a.x = width/2.0;
	point_c.x = width/2.0;
}

unsigned int DiamondShape::getPointCount() const{

	return 4;
}

Vector2f DiamondShape::getPoint(unsigned int index) const{

	if(index == 0){
		return point_a;
	}
	else if(index == 1){
		return point_b;
	}
	else if(index == 2){
		return point_c;
	}
	else if(index == 3){
		return point_d;
	}
}


IsoSideShape::IsoSideShape(){

	//Shape();

	point_a = Vector2f(0,0);
	point_b = Vector2f(0,0);
	point_c = Vector2f(0,0);
	point_d = Vector2f(0,0);
}

void IsoSideShape::setSize(double width, double height){

	point_a.y = 0;
	point_a.x = 0;
	point_b.x = width;
	point_b.y = 0.5*abs(width);
	point_c.y = height + (0.5*abs(width));
	point_c.x = width;
	point_d.x = 0;
	point_d.y = height;
}

unsigned int IsoSideShape::getPointCount() const{

	return 4;
}

Vector2f IsoSideShape::getPoint(unsigned int index) const{

	if(index == 0){
		return point_a;
	}
	else if(index == 1){
		return point_b;
	}
	else if(index == 2){
		return point_c;
	}
	else if(index == 3){
		return point_d;
	}
}

char randChar()
{
    return (char)( (rand() % 52) + 65);
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

    for(TiXmlElement* parser = doc.FirstChildElement("Sqr"); parser!=NULL; parser = parser->NextSiblingElement("Sqr"))
    {
        string name = parser->Attribute("id");
        IntRect rect;
        rect.width = strtod(parser->Attribute("width"),NULL);
        rect.height = strtod(parser->Attribute("height"),NULL);
        rect.left = 0;
        rect.top = 0;

 		Texture* ne_texture = new Texture();
 		Texture* se_texture = new Texture();
 		Texture* sw_texture = new Texture();
 		Texture* nw_texture = new Texture();

 		string path = parser->Attribute("file_path");
 		ne_texture->loadFromFile("sheets/" + path + "_ne.png");
 		se_texture->loadFromFile("sheets/" + path + "_se.png");
 		sw_texture->loadFromFile("sheets/" + path + "_sw.png");
 		nw_texture->loadFromFile("sheets/" + path + "_nw.png");

 		texture_rects[name] = rect;
 		textures[name + "_ne"] = ne_texture;
 		textures[name + "_se"] = se_texture;
 		textures[name + "_sw"] = sw_texture;
 		textures[name + "_nw"] = nw_texture;

 		double new_height = strtod(parser->Attribute("isoheight"),NULL);
 		iso_heights[name] = new_height;
    }
    for(TiXmlElement* parser = doc.FirstChildElement("Background"); parser!=NULL; parser = parser->NextSiblingElement("Background"))
    {
    	string name = parser->Attribute("id");

    	Texture* ne_texture = new Texture();
 		Texture* se_texture = new Texture();
 		Texture* sw_texture = new Texture();
 		Texture* nw_texture = new Texture();

    	string path = parser->Attribute("file_path");
 		ne_texture->loadFromFile("sheets/" + path + "_ne.png");
 		se_texture->loadFromFile("sheets/" + path + "_se.png");
 		sw_texture->loadFromFile("sheets/" + path + "_sw.png");
 		nw_texture->loadFromFile("sheets/" + path + "_nw.png");

 		ne_texture->setRepeated(true);
 		se_texture->setRepeated(true);
 		sw_texture->setRepeated(true);
 		nw_texture->setRepeated(true);

 		textures[name + "_ne"] = ne_texture;
 		textures[name + "_se"] = se_texture;
 		textures[name + "_sw"] = sw_texture;
 		textures[name + "_nw"] = nw_texture;
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

double getIsoHeight(string key){

	return iso_heights[key];
}

Vector2f transformToIsoView(Vector2f point_2d, string direction){

	Vector2f return_point = Vector2f(0,0);

	if(direction == "_ne"){

		return_point.x = ((-0.5)*point_2d.x) + ((-1)*point_2d.y);
		return_point.y = ((0.5)*point_2d.x) + ((1)*point_2d.y);
	}
	else if(direction == "_se"){

		return_point.x = ((-0.5)*point_2d.x) + ((-1)*point_2d.y);
		return_point.y = ((0.5)*point_2d.x) + ((-1)*point_2d.y);
	}
	else if(direction == "_sw"){

		return_point.x = ((-0.5)*point_2d.x) + ((1)*point_2d.y);
		return_point.y = ((-0.5)*point_2d.x) + ((-1)*point_2d.y);
	}
	else if(direction == "_nw"){

		return_point.x = ((0.5)*point_2d.x) + ((1)*point_2d.y);
		return_point.y = ((-0.5)*point_2d.x) + ((1)*point_2d.y);
	}

	return return_point;
}
Vector2f transformTo2DView(Vector3f point_iso, string direction){

	Vector2f return_point = Vector2f(0,0);
	
	if(direction == "_ne"){

		return_point.x = ((1)*point_iso.x) + ((1)*point_iso.y);
		return_point.y = ((-0.5)*point_iso.x) + ((0.5)*point_iso.y) - point_iso.z;
	}
	else if(direction == "_se"){

		return_point.x = ((-1)*point_iso.x) + ((1)*point_iso.y);
		return_point.y = ((-0.5)*point_iso.x) + ((-0.5)*point_iso.y) - point_iso.z;
	}
	else if(direction == "_sw"){

		return_point.x = ((-1)*point_iso.x) + ((-1)*point_iso.y);
		return_point.y = ((0.5)*point_iso.x) + ((-0.5)*point_iso.y) - point_iso.z;
	}
	else if(direction == "_nw"){

		return_point.x = ((1)*point_iso.x) + ((-1)*point_iso.y);
		return_point.y = ((0.5)*point_iso.x) + ((0.5)*point_iso.y) - point_iso.z;
	}
	
	return return_point;
}

