#ifndef WEC_CONTROLBOARD
#define WEC_CONTROLBOARD

#include "main.h"

using namespace std;
using namespace sf;


class ControlBoard: public Machine{

public:

    ControlBoard(string new_sector_id, string e_id, double x, double y);
    virtual bool performFunction(double dt);



    double progression = 0;
};

#endif WEC_CONTROLBOARD
