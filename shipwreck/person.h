#ifndef WEC_PERSON
#define WEC_PERSON

#include "main.h"

//using namespace std;using namespace sf;
//
//enum Nature{
//
//            N_Null, //No value. Slot is empty and will not factor into person's mind, causing simple-mindedness. Hodor!
//            N_Intellectual, //Predisposed to seek knowledge and answers...what about is determined by influences/circumstance.
//            N_Creative, //Unshakable urge to create and express one's ideas and imagination, and predisposed to act more out-of-the-box. How is determined by influences/circumstance.
//            N_Giving, //Selfless and driven to help others/contribute to society/mankind. From teaching to healing to third-world-aid, the form this takes is determined by influences.
//            N_Vain, //Subconscious concern of one's image, not necessarily physical, but of status and concept as well. May cause a desire to conform, to achieve high social status (fame/wealth/etc.), or to build a legacy. Again, determined by influences.
//            N_Simple, //Existence in itself is motivation enough to exist. This "higher calling" is to reject higher calling and be satisfied with the privilege of living here and now.
//            N_Strong //Driven to seek power to protect against all foes. Financial power, political power, physical power, all kinds. With a strong will, this might result in a general - weak will might result in a financially stable guy with a guard dog in his yard. Influences also factor in.
//
//            };
//
//enum Influence{ //NOT TO BE CONFUSED WITH LIKES/DISLIKES/SKILLS (Ex. You can be drawn to music but suck at it), an Influence is something that defines how a person approaches life
//
//               I_Null, //No value.
//               I_Musical, //Tends towards musical expression
//               I_Artistic, //Tends towards artistic (drawing/painting/graphic) pursuits
//               I_Thespian, //Craves an audience, and may tend towards pursuits with such. Often dramatic.
//               I_Page_Turner, //Pursues stories of all kinds
//               I_Political, //Tends towards drawing strength from others, and using words as weapons or to solve problems
//               I_Mathematical, //Calculating. Tends towards logical solutions and logic/mathematical pursuits, finds most efficient solution.
//               I_Scientific, //Tends towards applied-knowledge solutions and scientific pursuits
//               I_Creator, //Desire to make things
//               I_Physical, //Tends towards force-based solutions and pursuits
//               I_Athletic, //Values personal physical fitness and capability highly
//               I_Philosphical, //Tends towards abstract high-minded thinking
//               I_Spiritual, //Holds religious aspects of life in high regard
//               I_Righteous, //Believes one's own course morally infallible
//               I_Open_Minded, //Capable of self-critcism and questioning of actions
//               I_Pessimistic, //Anticipates worst outcome, good at spotting flaws and gaps in solutions
//               I_Optimistic, //Anticipates best outcome, good at finding the upsides in developments
//               I_Orderly, //Craves structure and order, IE chain of command.
//               I_Independent, //Tends towards making own decisions. Doesn't handle teamwork well, but good at self-direction.
//               I_Codependent, //Tends towards teamwork over self-direction. Needs external affirmation of choices.
//               I_Traditional, //Sticks to the society's and family's customs and traditions
//               I_Cynic, //Questions everything
//               I_Trusting, //Accepts concepts easily
//               I_Loving, //Craves companionship
//               I_Solitary, //Prefers to work/be by oneself. Does not form emotional attachment easily.
//               I_Visionary, //The eternal revolutionary always sees a better world that could be
//               I_Patient, //Paradise can always wait, setbacks are no issue. Looks for the best solution for the problem.
//               I_Impatient, //Easily angered by lack of immediate accomplishment. Finds fastest solutions if possible.
//               I_Just, //Not cruel, but punishes rule-breakers with sentences that fit the crime well.
//               I_Forgiving, //Everyone makes mistakes. Lenient and merciful with punishment, and doesn't hold grudges for long if at all.
//               I_Unforgiving, //Makes an example of rule-breakers. Holds a grudge.
//               I_Ideological, //Holds own ideology in high regard and will fight anyone who challenges it
//               I_Bold, //Tends towards the risky or unusual course of action
//               I_Reserved, //Tends to be cautious and careful when choosing courses of action
//               I_Light_Hearted, //Life is fun!
//               I_Serious //DO YOU THINK THIS IS HUMOUROUS?
//
//
//
//              };
//
//enum Align{
//
//           A_Lawful, //The laws were made for a reason, and must be respected. All good goals can be achieved within them.
//           A_Neutral, //I'll follow the laws so long as they suit me, but if they conflict with my goals then I'll break them.
//           A_Chaotic, //I do not agree with the law. I live outside of it, without authority to govern me.
//
//          };
//
//enum Rating{
//
//            None,
//            Low,
//            Normal,
//            High,
//            Extreme
//
//           };
//
//enum SkillType{
//
//              S_Null, //blank
//              S_Writing, //writing skill
//              S_Literary_Knowledge, //grant access to databank (about books)
//              S_Public_Speaking, //ability to speak well to large audiences
//              S_Diplomacy, //ability to placate and charm others
//              S_Negotiation, //ability to get what you want
//              S_Calculation, //mental calculation ability
//              S_Math_Theory, //grant access to databank
//              S_Chemical_Theory, //grant access to databank
//              S_Physics_Theory, //grant access to databank
//              S_Chemical_Application, //hands-on chemistry ability
//              S_Physics_Application, //hands-on physics ability
//              S_Electrical_Knowledge, //grant access to databank
//              S_Computational_Knowledge, //grant access to databank
//              S_Mechanical_Knowledge, //grant access to databank
//              S_Biological_Knowledge, //grant access to databank
//              S_Plant_Biology_Applied, //ability to work with plants hands-on, IE botanist work
//              S_Animal_Biology_Applied, //ability to work with animals, IE veterinary work
//              S_Human_Biology_Applied, //ability to work with human biology, IE physician
//              S_Business, //ability to spot financial opportunities
//              S_Law_Knowledge, //grant access to databank
//              S_Speed, //speed of running
//              S_Stamina, //ability to physically exert for long periods of time
//              S_Strength, //physical strength
//              S_Martial_Ability, //combat ability
//              S_Sporting_Ability, //ability to play physical sports
//              S_Shooting_Ability, //ability to fire/maintain guns
//              S_History_Knowledge, //grant access to databank
//              S_Philosophy_Knowledge, //grant access to databank
//              S_Cooking, //grant access to databank + applied skill
//              S_Musical_Ability, //ability to play musical instruments
//              S_Musical_Knowledge, //grant access to database
//              S_D_and_P, //ability to draw and paint
//              S_Aesthetics, //ability to design buildings and layouts to look pleasing
//              S_Humour, //ability to see the humour in situations and tell jokes (grant access to joke database)
//              S_Acting, //ability to perform as a character...and also to lie.
//              S_Vehicular_Knowledge, //grant access to databank
//              S_Vehicular_Operation, //ability to pilot/drive
//              S_Cartography, //ability to read maps and charts + grant access to databank about locations
//              S_Leadership //not Stephen Harper leadership, but the kind of natural leadership that draws others to follow
//
//              };
//struct Skill
//{
//    Rating proficiency = None;
//    SkillType type = S_Null;
//
//    Skill();
//    Skill(SkillType, Rating);
//
//};
//

enum NodeStatus{OPEN, CLOSED, BLOCKED};
struct Node{

    public:

    Node();
    Node(Vector2f);


    string id;
    weak_ptr<Node> parent;

    Vector2f coords;
    double g;

    NodeStatus status;

    void getPath(vector<Vector2f> &nodes);
};



class Person: public Player
{
    public:

    Person();
    Person(string new_sector_id, string e_id, double x, double y, string type);


    void calculatePath(Vector2f);
    vector<Vector2f> path; //coordinates relative to unrotated frame of reference - rotational math can be applied to get real coordinates in the scene
    void proceedAlongPath(double dt);
    Vector2f pathing_vector;

    virtual void run(double dt);
};
//
//void makeNewPerson(string name, double x, double y, string info_key);

#endif //WEC_PERSON
