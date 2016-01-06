#include "main.h"

using namespace std;
using namespace sf;


extern World world;


InputData::InputData(){

    reset();
}

void InputData::reset(){

    dt = 0.01;
    player_id = "";
    sector_id = "";
    mouse = Vector2f(0,0);
    fixed_mouse = Vector2f(0,0);
    mmb_delta = 0;
    lmb_released = false;
    rmb_released = false;
    rmb_held = false;
    lmb_held = false;
    keys_released.clear();
    keys_held.clear();
    text_entered.clear();
}

ObData::ObData(){

    ob_id = "";
    ob_type = "";
    ob_coords = Vector2f(0,0);
    ob_rotation = 0;
}

Color operator -(const Color& left, const Color& right){

    Color return_color;
    return_color = left + (Color(-1,-1,-1,-1)*right);
    return return_color;
}

Packet& operator <<(Packet& packet, const InputData& m){

    packet << m.mouse.x << m.mouse.y << m.lmb_released << m.rmb_released << m.mmb_delta << m.keys_released.size();
    for(set<string>::iterator parser = m.keys_released.begin(); parser!=m.keys_released.end(); parser++){
        packet << *parser;
    }
    packet << m.keys_held.size();
    for(set<string>::iterator parser = m.keys_held.begin(); parser!=m.keys_held.end(); parser++){
        packet << *parser;
    }
    return packet;
}

Packet& operator >>(Packet& packet, InputData& m){

    packet >> m.mouse.x >> m.mouse.y >> m.lmb_released >> m.rmb_released >> m.mmb_delta;
    int released_size = 0;
    packet >> released_size;
    for(int i = 0; i < released_size; i++){
        string temp_str;
        packet >> temp_str;
        m.keys_released.insert(temp_str);
    }
    int held_size = 0;
    packet >> held_size;
    for(int i = 0; i < held_size; i++){
        string temp_str;
        packet >> temp_str;
        m.keys_held.insert(temp_str);
    }
}

Packet& operator <<(Packet& packet, const ObData& m){

    return packet << m.ob_id << m.ob_type << m.ob_coords.x << m.ob_coords.y << m.ob_rotation;
}

Packet& operator >>(Packet& packet, ObData& m){
    return packet >> m.ob_id >> m.ob_type >> m.ob_coords.x >> m.ob_coords.y >> m.ob_rotation;
}

void playSound(string sound_id){

    shared_ptr<Sound> sound = make_shared<Sound>();
   // sound->setBuffer(*sounds[sound_id]);
   // server.audio[server.audio.size()] = sound;
    sound->play();
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
/*
    if(h1==0){return Vector2f(0,0);}
    perp = Vector2f(y1/h1,-x1/h1);

    if(axis.x>=0 and axis.y < 0)//quad 1
    {
        perp.x = abs(perp.x);
        perp.y = abs(perp.y);
    }
    else if(axis.x>=0 and axis.y >= 0)//quad 2
    {
        perp.x = -1*abs(perp.x);
        perp.y = abs(perp.y);
    }
    else if(axis.x<0 and axis.y >= 0)//quad 3
    {
        perp.x = -1*abs(perp.x);
        perp.y = -1*abs(perp.y);
    }
    else if(axis.x<0 and axis.y <0)//quad 4
    {
        perp.x = abs(perp.x);
        perp.y = -1*abs(perp.y);
    }
*/
    return perp;

}

string createUniqueId()
{
    int suffixlength = 8;

    string newid = world.date.asString();

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

double asDouble(string st)
{
    stringstream converter;
    double returndouble = 0;
    converter << st;
    converter >> returndouble;
    return returndouble;
}

double convertToMeters(double pixels)
{
    return (pixels/80);
}

double convertToPixels(double meters)
{
    return (meters*80);
}


double convertToPSI(double mols_per_litre){

    //rounds to two decimal places
    double return_val = 14.69595*mols_per_litre*0.08206;
    return_val = (floor(return_val*100))/100;
    return return_val; //not entirely accurate conversion, but it makes the numbers work nicely

}

int closestLowestInt(double d)
{
    int c = ceil(d);
    int f = floor(d);

    if(abs(c)<=abs(f)){return c;}
    else{return f;}
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

double formatRotation(double radians){

    while(radians > 2*M_PI){
        radians -= (2*M_PI);
    }

    while(radians < 0){
        radians += (2*M_PI);
    }

    return radians;

}

Date::Date()
{

}

Date::Date(int m, int d, int y)
{
    month = m;
    day = d;
    year = y;
}

Date::Date(int m, int d, int y, int h, int mi, double s)
{
    month = m;
    day = d;
    year = y;

    hour = h;
    minute = mi;
    second = s;

    timeRecorded = true;
}

string Date::asString()
{
    stringstream converter;

    if(timeRecorded)
    {
        converter << hour << "-" << minute << "-" << second << " on " << month << "-" << day << "-" << year;
    }
    else
    {
        converter << month << "-" << day << "-" << year;
    }

    string returnString;
    getline(converter, returnString);

    return returnString;
}

void Date::update(double dt)
{
    second += dt;
    if(second>=60)
    {
        minute += 1;
        second -= 60;
        if(minute>=60)
        {
            hour += 1;
            minute -= 60;
            if(hour>=24)
            {
                day += 1;
                hour -= 24;
                if(day>=30)
                {
                    month += 1;
                    day = 1;
                    if(month >= 13)
                    {
                        year += 1;
                        month = 1;
                    }
                }
            }
        }
    }
}

bool operator >(const Date& left, const Date& right){

    if(left.year == right.year){
        if(left.month == right.month){
            if(left.day == right.day){
                if(left.hour == right.hour){
                    if(left.minute == right.minute){
                        if(left.second == right.second){
                            return false;
                        }
                        else if(left.second > right.second){
                            return true;
                        }
                    }
                    else if(left.minute > right.minute){
                        return true;
                    }
                }
                else if(left.hour > right.hour){
                    return true;
                }
            }
            else if(left.day > right.day){
                return true;
            }
        }
        else if(left.month > right.month){
            return true;
        }
    }
    else if(left.year > right.year){
        return true;
    }

    return false;
}

bool operator <(const Date& left, const Date& right){

    if(right > left){
        return true;
    }

    return false;
}

Date operator +(const Date& left, const Date& right){

    double year = left.year;
    double month = left.month;
    double day = left.day;
    double hour = left.hour;
    double minute = left.minute;
    double second = left.second;

    second += right.second;
    while(second >= 60){
        minute++;
        second -= 60;
    }

    minute += right.minute;
    while(minute >= 60){
        hour++;
        minute -= 60;
    }

    hour += right.hour;
    while(hour >= 24){
        day++;
        hour -= 24;
    }

    day += right.day;
    while(day >= 30){
        month++;
        day -= 30;
    }

    month += right.month;
    while(month >= 13){
        year++;
        month -= 12;
    }

    year += right.year;

    return Date(month,day,year,hour,minute,second);
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


