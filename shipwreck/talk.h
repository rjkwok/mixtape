#ifndef WEC_TALK
#define WEC_TALK

#include "main.h"

using namespace std;using namespace sf;
using namespace sf;

struct ChatEntry{

    ChatEntry();
    ChatEntry(string, string);

    string source;
    string contents;

    Text text;

    double seconds_remaining;
};

class Chat{

public:

    Chat();
    Chat(Vector2f, int);

    Vector2f fixed_coords;
    map<int,ChatEntry> entries;
    int max_entries;

    FloatRect click_box;
    bool accepting_input = false;
    string input_string;

    void add(string,string);
    void update(double dt);
};

#endif
