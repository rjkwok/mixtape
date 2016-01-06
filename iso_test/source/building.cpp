#include "main.h"

using namespace std;
using namespace sf;

extern RenderWindow window;
extern View view;
extern View window_view;
extern string view_direction;


Building::Building(){}
void Building::addMember(string id){

	members.insert(id);
}
