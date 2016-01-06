#ifndef WEC_UTIL
#define WEC_UTIL

#include "main.h"

using namespace std;
using namespace sf;

char rndChar();

string createUniqueId();
string asString(int number);
string asString(double number);
double asDouble(string st);
int closestLowestInt(double);

double convertToMeters(double);
double convertToPixels(double);
double convertToPSI(double mols_per_litre);

void playSound(string);

struct InputData{

    InputData();

    string player_id;
    string sector_id;
    double dt;

    Vector2f mouse;
    Vector2f fixed_mouse;
    bool lmb_released;
    bool rmb_released;
    bool rmb_held;
    bool lmb_held;
    double mmb_delta;
    set<string> keys_released;
    set<string> keys_held;
    set<char> text_entered;

    void reset();
};

Packet& operator <<(Packet& packet, const InputData& m);
Packet& operator >>(Packet& packet, InputData& m);

struct ObData{

    ObData();

    string ob_id;
    string ob_type;
    Vector2f ob_coords;
    double ob_rotation;
};

Packet& operator <<(Packet& packet, const ObData& m);
Packet& operator >>(Packet& packet, ObData& m);

struct Date
{
    int month;
    int day;
    int year;

    int hour = -1;
    int minute = -1;
    double second = -1;

    bool timeRecorded = false;

    Date();
    Date(int,int,int);
    Date(int,int,int,int,int,double);

    string asString();
    void update(double);
};

bool operator >(const Date& left, const Date& right);
bool operator <(const Date& left, const Date& right);
Date operator +(const Date& left, const Date& right);

Vector2f operator *(const Vector2f& left, const Vector2f& right);
Vector2f operator /(const Vector2f& left, const Vector2f& right);
Vector2f operator +(const Vector2f& left, const double& right);
Vector2f operator -(const Vector2f& left, const double& right);
Vector2f operator *(const Vector2f& left, const double& right);
Vector2f operator /(const Vector2f& left, const double& right);
Color operator -(const Color& left, const Color& right);

Vector2f getPerpendicularAxis(Vector2f axis);
double getRotationFromAxis(Vector2f);
double formatRotation(double radians);

#endif // WEC_UTIL
