#include "main.h"

using namespace std;
using namespace sf;


extern World world;
extern RenderWindow window;

Rope::Rope(){}

Rope::Rope(string new_sector_id, string new_id, string type)
{
    properties = getProperties(type);
    id = new_id;

    sector_id = new_sector_id;
}

Rope::Rope(string new_sector_id, string new_id, Vector2f head, Vector2f tail, string type)
{
    properties = getProperties(type);
    id = new_id;

    sector_id = new_sector_id;

    Vector2f dist = head-tail;
    double h_dist = hypot(dist.x,dist.y);
    Vector2f unit_vector = dist/h_dist;

    double node_amount = (h_dist/properties->get("node_gap"));
    if(node_amount <= 0){node_amount = 1;}

    for(double i = 0; i < node_amount; i++)
    {
        nodes.push_back(make_shared<RopeNode>(sector_id, Vector2f(tail.x+(i*unit_vector.x*properties->get("node_gap")),tail.y+(i*unit_vector.y*properties->get("node_gap"))), id, i, type));
    }
}

void Rope::addExplicitNode(Vector2f position, Vector2f velocity, string attached, Vector2f attachment_coords){

    int i = nodes.size();
    nodes.push_back(make_shared<RopeNode>(sector_id, position, id, i, properties->name));
    nodes[i]->p = velocity*properties->get("mass");
    nodes[i]->attached_id = attached;
    nodes[i]->attachment_coords = attachment_coords;
}

void Rope::run(double dt)
{
    if(dt>0.01){dt=0.01;}
    bool apply_to_ob = true;
    if(nodes.size() >= 2){
        string attached_id_1 = nodes[0]->attached_id;
        string attached_id_2 = nodes[nodes.size()-1]->attached_id;
        
        if(getSector(sector_id)->getEnt(attached_id_1)->bonded && getSector(sector_id)->getEnt(attached_id_2)->bonded && getSector(sector_id)->getEnt(attached_id_1)->bond == getSector(sector_id)->getEnt(attached_id_2)->bond){
            apply_to_ob = false;
        }
    }
    for(int i = 0; i < nodes.size(); i++)
    {
        //check distance from next node to right
        shared_ptr<RopeNode> current_node;
        shared_ptr<RopeNode> partner_1;
        shared_ptr<RopeNode> partner_2;
        if(i==0)//first
        {
            current_node = nodes[i];
            partner_2 = nodes[i+1];

            Vector2f gap_2 = current_node->coords - partner_2->coords;
            double h_2 = hypot(gap_2.x,gap_2.y);

            if(h_2 > 0.01){
                Vector2f seperation_axis_2 = gap_2/h_2;
                double potential_energy_2  = (properties->get("node_gap")-h_2) * properties->get("mass") * properties->get("k")/2.0;

                //if(potential_energy_2>0){potential_energy_2*=2;}

                current_node->applyImpulse(dt*potential_energy_2*seperation_axis_2.x,dt*potential_energy_2*seperation_axis_2.y);
                //partner_2->applyImpulse(dt*potential_energy_2*-seperation_axis_2.x,dt*potential_energy_2*-seperation_axis_2.y);
            }
            
        }
        else if(i==nodes.size()-1)//last
        {
            current_node = nodes[i];
            partner_1 = nodes[i-1];

            Vector2f gap_1 = current_node->coords - partner_1->coords;
            double h_1 = hypot(gap_1.x,gap_1.y);

            if(h_1 > 0.01){
                Vector2f seperation_axis_1 = gap_1/h_1;
                double potential_energy_1 = (properties->get("node_gap")-h_1) * properties->get("mass") * properties->get("k") /2.0;//displacement (negative if the nodes should pull together, positive if push away) * stiffness(rate of release) of P.E.

              //  if(potential_energy_1>0){potential_energy_1*=2;}

                current_node->applyImpulse(dt*potential_energy_1*seperation_axis_1.x,dt*potential_energy_1*seperation_axis_1.y);
                //partner_1->applyImpulse(dt*potential_energy_1*-seperation_axis_1.x,dt*potential_energy_1*-seperation_axis_1.y);
            }
            
        }
        else
        {
            current_node = nodes[i];
            partner_1 = nodes[i-1];
            partner_2 = nodes[i+1];

            Vector2f gap_1 = current_node->coords - partner_1->coords;
            double h_1 = hypot(gap_1.x,gap_1.y);

            if(h_1 > 0.01){
               Vector2f seperation_axis_1 = gap_1/h_1;
               double potential_energy_1 = (properties->get("node_gap")-h_1) * properties->get("mass") * properties->get("k")/2.0;//displacement (negative if the nodes should pull together, positive if push away) * stiffness(rate of release) of P.E.
               current_node->applyImpulse(dt*potential_energy_1*seperation_axis_1.x,dt*potential_energy_1*seperation_axis_1.y);
            }
            
            Vector2f gap_2 = current_node->coords - partner_2->coords;
            double h_2 = hypot(gap_2.x,gap_2.y);

            if(h_2 > 0.01){
                Vector2f seperation_axis_2 = gap_2/h_2;
                double potential_energy_2  = (properties->get("node_gap")-h_2) * properties->get("mass") * properties->get("k")/2.0;
 
               current_node->applyImpulse(dt*potential_energy_2*seperation_axis_2.x,dt*potential_energy_2*seperation_axis_2.y);
            }
            

          //  if(potential_energy_1>0){potential_energy_1*=2;}
         //   if(potential_energy_2>0){potential_energy_2*=2;}

            
            
        }
    }

    for(vector<shared_ptr<RopeNode> >::iterator i = nodes.begin(); i != nodes.end(); i++)
    {
        shared_ptr<RopeNode> node = *i;
        node->run(dt, apply_to_ob);
    }
}

void Rope::draw()
{
    for(vector<shared_ptr<RopeNode> >::iterator i = nodes.begin(); i != nodes.end(); i++)
    {
        shared_ptr<RopeNode>  inode = *i;
        inode->sprite.setPosition(inode->coords.x,inode->coords.y);
        window.draw(inode->sprite);
    }
}

int Rope::getNearestNode(Vector2f point)
{
    int nearest = 0;
    double dist = hypot(point.x-nodes[0]->coords.x,point.y-nodes[0]->coords.y);
    double nearestdist = dist;

    for(int i = 1; i < nodes.size(); i++)
    {
        dist = hypot(point.x-nodes[i]->coords.x,point.y-nodes[i]->coords.y);
        if(dist<nearestdist)
        {
            nearest = i;
        }
    }

    return nearest;
}

void Rope::attach(string ent_id, Vector2f relative){

    shared_ptr<Entity> ent = getSector(sector_id)->getEnt(ent_id);
    if(not ent){return;}

    int index = getNearestNode(ent->getPosition());
    nodes[index]->attached_id = ent_id;
    nodes[index]->attachment_coords = relative;
}

void Rope::attach(int index, string ent_id, Vector2f relative){

    shared_ptr<Entity> ent = getSector(sector_id)->getEnt(ent_id);
    if(not ent){return;}

    nodes[index]->attached_id = ent_id;
    nodes[index]->attachment_coords = relative;
}

RopeNode::RopeNode(){}

RopeNode::RopeNode(string new_sector_id, Vector2f new_coords, string new_rope_id, int new_index, string type)
{
    properties = getProperties(type);
    rope_id = new_rope_id;
    index = new_index;

    sprite.setTexture(properties->sheet, true);
    FloatRect bounds_rect = sprite.getLocalBounds();
    sprite.setOrigin(bounds_rect.width/2,bounds_rect.height/2);

    sector_id = new_sector_id;
    coords = new_coords;

    sprite.setPosition(coords.x,coords.y);
}

void RopeNode::applyImpulse(double x, double y)
{
    p = p + Vector2f(x,y);
}

Vector2f RopeNode::getPosition()
{
    return coords;
}

Vector2f RopeNode::getMomentum()
{
    return p;
}

Vector2f RopeNode::getVelocity()
{
    return p/properties->get("mass");
}
void RopeNode::light(double amount){

    light_level += amount;
    if(getSector(sector_id)->ambient_light + light_level > 255){ light_level =  255-getSector(sector_id)->ambient_light; }
}

void RopeNode::run(double dt, bool apply_to_ob)
{
    if(dt>0.01){dt = 0.01;}
    if(attached_id != "")
    {
        shared_ptr<Entity> ob = getSector(sector_id)->getEnt(attached_id);
        if(ob){
            double Q = ob->sprite.getRotation() * (M_PI/180);
            Vector2f target = ob->getPosition() + Vector2f(attachment_coords.x*cos(Q)-attachment_coords.y*sin(Q),attachment_coords.x*sin(Q)+attachment_coords.y*cos(Q));
            Vector2f gap = target - coords;
            
            double h = hypot(gap.x,gap.y);

            if(h > 0.01){
                Vector2f seperation_axis = gap/h;

                double potential_energy = (h) * properties->get("k") * properties->get("mass")/2.0;//displacement (negative if the nodes should pull together, positive if push away) * stiffness(rate of release) of P.E.

                applyImpulse(dt*potential_energy*seperation_axis.x,dt*potential_energy*seperation_axis.y);

                if(apply_to_ob){
                    
                    ob->applyImpulse(-dt*potential_energy*seperation_axis.x,-dt*potential_energy*seperation_axis.y, target,false,false);
                }
                
            }
            
        }
        else{
            attached_id = "";
        }
    }

    double friction = 0.001; //rope friction will eat away at momentum from connected entities...
    Vector2f p_to_subtract = p*friction*properties->get("k")*dt;
    if(abs(p_to_subtract.x) > abs(p.x)){
        p.x = 0;
    }
    else{
        p.x = p.x - p_to_subtract.x;
    }
    if(abs(p_to_subtract.y) > abs(p.y)){
        p.y = 0;
    }
    else{
        p.y = p.y - p_to_subtract.y;
    }
    Vector2f velocity = getVelocity();

    //if(hypot(velocity.x,velocity.y)>1)
    //{
        coords = coords + (velocity*dt);
        //p = p - (velocity*properties->get("mass")*friction*dt);
    //}

    sprite.setColor(Color(getSector(sector_id)->ambient_light+light_level,getSector(sector_id)->ambient_light+light_level,getSector(sector_id)->ambient_light+light_level));
    light_level = 0;


    sprite.setPosition(coords.x,coords.y);
}
