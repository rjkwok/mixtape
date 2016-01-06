#ifndef ASH_UTIL
#define ASH_UTIL

#include "main.h"

using namespace std;
using namespace sf;

//SFML shape extensions
class DiamondShape: public Shape{

public:

	DiamondShape();

	Vector2f point_a;
	Vector2f point_b;
	Vector2f point_c;
	Vector2f point_d;

	void setHeight(double height);
	void setWidth(double width);

	virtual unsigned int getPointCount() const;
	virtual Vector2f getPoint(unsigned int index) const;
};

class IsoSideShape: public Shape{

public:

	IsoSideShape();

	Vector2f point_a;
	Vector2f point_b;
	Vector2f point_c;
	Vector2f point_d;

	void setSize(double width, double height);

	virtual unsigned int getPointCount() const;
	virtual Vector2f getPoint(unsigned int index) const;
};
//

//generators
char rndChar();
string createUniqueId();
//

//useful transformations/conversions
string asString(int number);
string asString(double number);
Vector2f getPerpendicularAxis(Vector2f axis);
Vector2f transformTo2DView(Vector3f point_iso, string direction);
Vector2f transformToIsoView(Vector2f point_2d, string direction);
//

//file and memory management
void loadConfigs();
void destroyTextures();
//

//retrieval functions
Texture* getTexture(string key);
IntRect getTextureRect(string key);
double getIsoHeight(string key);
//

//operator extensions
Vector2f operator *(const Vector2f& left, const Vector2f& right);
Vector2f operator /(const Vector2f& left, const Vector2f& right);
Vector2f operator +(const Vector2f& left, const double& right);
Vector2f operator -(const Vector2f& left, const double& right);
Vector2f operator *(const Vector2f& left, const double& right);
Vector2f operator /(const Vector2f& left, const double& right);
//

#endif