#include "main.h"

using namespace std;
using namespace sf;


extern World world;

Person::Person(string new_sector_id, string e_id, double x, double y, string type)
{
    constructEntity(new_sector_id, e_id, x, y, type);

    setSlots(9);

}

string makeNodeKey(Vector2f point){

    return asString(floor(point.x)) + "_" + asString(floor(point.y));
}

Node::Node(){}
Node::Node(Vector2f new_coords){

    coords = new_coords;
    status = OPEN;
    id = makeNodeKey(new_coords);
}

void Node::getPath(vector<Vector2f> &nodes){

    nodes.push_back(coords);

    if(id!=parent.lock()->id){
         parent.lock()->getPath(nodes);
    }

}


Person::Person(){}

vector<Vector2f> getNeighbors(Vector2f point){

    double step = 10;
    double angle_step = 14;

    vector<Vector2f> displacements;
    displacements.push_back(Vector2f(0,-step));
    displacements.push_back(Vector2f(0,step));
    displacements.push_back(Vector2f(step,0));
    displacements.push_back(Vector2f(-step,0));
    displacements.push_back(Vector2f(angle_step,-angle_step));
    displacements.push_back(Vector2f(-angle_step,-angle_step));
    displacements.push_back(Vector2f(angle_step,angle_step));
    displacements.push_back(Vector2f(-angle_step,angle_step));

    vector<Vector2f> return_list;
    for(vector<Vector2f>::iterator parser = displacements.begin(); parser!=displacements.end(); parser++){
        return_list.push_back(point + *parser);
    }

    return return_list;
}

double hypot(Vector2f a,Vector2f b){

    Vector2f disp = b-a;
    if(disp.x==0 and disp.y==0){
        return 0;
    }
    return hypot(disp.x,disp.y);
}


void Person::calculatePath(Vector2f pathing_target){

    map<string,shared_ptr<Node> > nodes;
    set<string> open_nodes;

    Vector2f start_point = getPosition();
    nodes[makeNodeKey(start_point)] = make_shared<Node>(start_point);
    nodes[makeNodeKey(start_point)]->parent = weak_ptr<Node>(nodes[makeNodeKey(start_point)]);
    nodes[makeNodeKey(start_point)]->g = 0;

    open_nodes.insert(makeNodeKey(start_point));

    string current_id = makeNodeKey(start_point);
    string current_parent_id;

    bool no_solution = false;

    while((abs(nodes[current_id]->coords.x-pathing_target.x)>collision_radius or abs(nodes[current_id]->coords.y-pathing_target.y)>collision_radius) and open_nodes.size()>0){

        current_parent_id = nodes[current_id]->parent.lock()->id;
        //get lowest node
        double lowest_value;
        string node_id;
        for(set<string>::iterator parser = open_nodes.begin(); parser != open_nodes.end(); parser++){
            if(parser!=open_nodes.begin()){
                if(nodes[*parser]->g+hypot(nodes[*parser]->coords,pathing_target)>=lowest_value){
                    continue;
                }

            }

            lowest_value = nodes[*parser]->g+hypot(nodes[*parser]->coords,pathing_target);
            node_id = *parser;
        }
        //

        //close node
        nodes[node_id]->status = CLOSED;
        open_nodes.erase(node_id);
        //

        //add neighbors to open
        vector<Vector2f> neighbors = getNeighbors(nodes[node_id]->coords);
        for(vector<Vector2f>::iterator parser = neighbors.begin(); parser!=neighbors.end(); parser++){
            string new_node_id = makeNodeKey(*parser);
            if(nodes.count(new_node_id)==0){
                //make new
                nodes[new_node_id] = make_shared<Node>(*parser);
                nodes[new_node_id]->g = INFINITY;
                nodes[new_node_id]->status = OPEN;
                //insert if not blocked

                if(not getSector(sector_id)->collision_grid.checkRadialCollision(collision_radius,*parser,id)){
                    open_nodes.insert(new_node_id);
                }
                else{
                    nodes[new_node_id]->status = BLOCKED;
                }

            }

            if(nodes[new_node_id]->status==OPEN){

                if(hypot(nodes[new_node_id]->coords, nodes[node_id]->coords) +nodes[node_id]->g < nodes[new_node_id]->g){
                    nodes[new_node_id]->parent = weak_ptr<Node>(nodes[node_id]);
                    nodes[new_node_id]->g = hypot(nodes[new_node_id]->coords, nodes[node_id]->coords) +nodes[node_id]->g;
                }
            }

        }

        current_id = node_id;

    }
    if (abs(nodes[current_id]->coords.x-pathing_target.x)>collision_radius or abs(nodes[current_id]->coords.y-pathing_target.y)>collision_radius){
        double closest_value;
        for(map<string,shared_ptr<Node> >::iterator parser = nodes.begin(); parser!=nodes.end(); parser++){
            if(parser->second->status==BLOCKED){continue;}

            if(parser!=nodes.begin()){
                if(hypot(pathing_target,parser->second->coords)>=closest_value){
                    continue;
                }
            }
            closest_value = hypot(pathing_target,parser->second->coords);
            current_id = parser->first;
        }

    }

    path.clear();
    nodes[current_id]->getPath(path);

}

void Person::proceedAlongPath(double dt){

    pathing_vector = Vector2f(0,0);
    if(path.size()==0){return;}

//    for(int i = 0; i < path.size()-1; i++){
//
//        drawDot(path[i],Color(55,255,255,200));
//    }

    if(abs(getPosition().x-path[path.size()-1].x)<10 and abs(getPosition().y-path[path.size()-1].y)<10){

        path.pop_back();
        if(path.size()==0){return;}
    }

    Vector2f dist = path[path.size()-1] - getPosition();
    double h = hypot(dist.x,dist.y);
    pathing_vector = dist/h;

    sprite.setRotation(getRotationFromAxis(pathing_vector));

    Vector2f p_difference = (pathing_vector*300*getMass()) - getMomentum();

    if((getSector(sector_id)->planetside || floors.size()!=0) and (not bonded or floor_bonds.count(bond)==0))
    {
        applyImpulse(p_difference.x*dt,p_difference.y*dt,getPosition(),false,true);
        for(set<string>::iterator parser = floors.begin(); parser!=floors.end(); parser++)
        {
            shared_ptr<Entity> current_floor = getSector(sector_id)->getEnt(*parser);
           // current_floor->applyImpulse(-p_difference.x*dt/floors.size(),-p_difference.y*dt/floors.size(),getPosition(),false);
        }
    }

}

void Person::run(double dt)
{
    proceedAlongPath(dt);
    runPhysicsAndGraphics(dt);
}

