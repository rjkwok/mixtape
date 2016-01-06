#include "main.h"

using namespace sf;
using namespace std;using namespace sf;


extern World world;

CollisionGrid::CollisionGrid(){}

CollisionGrid::CollisionGrid(string new_sector_id, double c_s_length, Vector2f top_left, Vector2f bottom_right)
{
    sector_id = new_sector_id;

    cell_side_length = c_s_length;
    Vector2f pos_cells = bottom_right/cell_side_length;
    Vector2f neg_cells = top_left/(-1*cell_side_length);
    pos_cells.x = ceil(pos_cells.x);
    pos_cells.y = ceil(pos_cells.y);
    neg_cells.x = ceil(neg_cells.x);
    neg_cells.y = ceil(neg_cells.y);

    max_cell_x = pos_cells.x;
    max_cell_y = pos_cells.y;
    min_cell_x = -neg_cells.x;
    min_cell_y = -neg_cells.y;

    set<string> blank_set;

    for(int x = min_cell_x; x <= max_cell_x; x++)
    {
        for(int y = min_cell_y; y <= max_cell_y; y++)
        {
            grid[x][y] = blank_set;
            //new cell
        }
    }
}

void CollisionGrid::reset()
{
    //erase all values

    for(unordered_map<int,unordered_map<int,set<string> > >::iterator parser = grid.begin(); parser!=grid.end(); parser++)
    {
        for(unordered_map<int,set<string> >::iterator parser2 = parser->second.begin(); parser2!=parser->second.end(); parser2++)
        {
            if(!parser2->second.empty()){
                parser2->second.clear();
            }

        }
    }

    limbo.clear();
}

void CollisionGrid::removeObject(string ob_id){

    for(unordered_map<int,unordered_map<int,set<string> > >::iterator parser = grid.begin(); parser!=grid.end(); parser++)
    {
        for(unordered_map<int,set<string> >::iterator parser2 = parser->second.begin(); parser2!=parser->second.end(); parser2++)
        {
            parser2->second.erase(ob_id);
        }
    }
}

void CollisionGrid::insertObject(string ob_id)
{
    shared_ptr<Entity> ob = getSector(sector_id)->getEnt( ob_id);

   // if(ob->prop && ob->already_placed){
   //     grid[ob->grid_placement.x][ob->grid_placement.y].insert(ob_id);
    //    return;
  //  }
    //
    double ob_cell_x = closestLowestInt(ob->getPosition().x/cell_side_length);
    double ob_cell_y = closestLowestInt(ob->getPosition().y/cell_side_length);

    double border_distance = closestLowestInt(ceil(ob->collision_radius/cell_side_length));
    Vector2f topleft(ob_cell_x - border_distance,ob_cell_y - border_distance);
    Vector2f bottomright(ob_cell_x + border_distance,ob_cell_y + border_distance);

    bool in_cell = false;
    for(int x = topleft.x; x <= bottomright.x; x++){
        for(int y = topleft.y; y <= bottomright.y; y++){
            if(grid.count(x)!=0 and grid[x].count(y)!=0)
            {
                grid[x][y].insert(ob_id);
                in_cell = true;

             //   if(ob->prop){
             //       ob->already_placed = true;
             //       ob->grid_placement = Vector2f(x,y);
             //   }
            }
        }
    }

    if(not in_cell) //add to limbo if outside radius
    {
        limbo.insert(ob_id);
    }
}

bool CollisionGrid::lineOfSight(string ent_id, Vector2f point){

    double increment = 10;

    if(shared_ptr<Entity> ent = getSector(sector_id)->getEnt(ent_id)){

        Vector2f track_point = ent->getPosition();
        Vector2f dist = point - track_point;
        double h = hypot(dist.x,dist.y);
        if(h==0){return true;}

        Vector2f unit_vector = dist/h;

        double last_h = h;

        while(hypot(point.x-track_point.x,point.y-track_point.y)<=last_h and hypot(point.x-track_point.x,point.y-track_point.y)> increment+5){

            set<string> possible_blockage;

            int x = closestLowestInt(track_point.x/cell_side_length);
            int y = closestLowestInt(track_point.y/cell_side_length);
            if(grid.count(x)!=0 and grid[x].count(y)!=0)
            {
                possible_blockage = grid[x][y];
            }

            for(set<string>::iterator parser = possible_blockage.begin(); parser!=possible_blockage.end(); parser++){
                if(shared_ptr<Entity> blocking_ent = getSector(sector_id)->getEnt(ent_id)){
                    if(blocking_ent->contains(track_point)){
                        return false;
                    }
                }
            }

            last_h = hypot(point.x-track_point.x,point.y-track_point.y);
            track_point = track_point + (unit_vector*increment);
        }

        return true;
    }

    return false;
}

set<string> CollisionGrid::getNearby(Vector2f point, double nearby_radius){

    set<string> possible_collisions;

    double point_cell_x = closestLowestInt(point.x/cell_side_length);
    double point_cell_y = closestLowestInt(point.y/cell_side_length);
    if(grid.count(point_cell_x)!=0 and grid[point_cell_x].count(point_cell_y)!=0)
    {
        set<string> nearby = grid[point_cell_x][point_cell_y];
        for(set<string>::iterator parser = nearby.begin(); parser != nearby.end(); parser++){
            if(getSector(sector_id)->getEnt(*parser)){
                possible_collisions.insert(*parser);
            }
        }
    }
    if(nearby_radius>0){

        double border_distance = closestLowestInt(ceil(nearby_radius/cell_side_length));
        Vector2f topleft(point_cell_x - border_distance,point_cell_y - border_distance);
        Vector2f bottomright(point_cell_x + border_distance,point_cell_y + border_distance);

        for(int x = topleft.x - 1; x <= bottomright.x + 1; x++){
            for(int y = topleft.y - 1; y <= bottomright.y + 1; y++){
                if(grid.count(x)!=0 and grid.find(x)->second.count(y)!=0)
                {
                    set<string> nearby = grid.find(x)->second.find(y)->second;
                    for(set<string>::iterator parser = nearby.begin(); parser != nearby.end(); parser++){
                        if(getSector(sector_id)->getEnt(*parser)){
                            possible_collisions.insert(*parser);
                        }
                    }
                }
            }
        }

    }

    for(set<string>::iterator parser = limbo.begin(); parser != limbo.end(); parser++){
        if(getSector(sector_id)->getEnt(*parser)){
            possible_collisions.insert(*parser);
        }
    }

    return possible_collisions;
}

set<string> CollisionGrid::getNearby(shared_ptr<Entity> ent){

    return getNearby(ent->getPosition(), ent->collision_radius);
}

bool CollisionGrid::checkRadialCollision(double radius, Vector2f point, string omit){


    set<string> possible_collisions;
    vector<Vector2f> checkpoints;
    checkpoints.push_back(point);
    checkpoints.push_back(point + Vector2f(radius,0));
    checkpoints.push_back(point + Vector2f(-radius,0));
    checkpoints.push_back(point + Vector2f(0,radius));
    checkpoints.push_back(point + Vector2f(0,-radius));

    for(vector<Vector2f>::iterator parser = checkpoints.begin(); parser!=checkpoints.end(); parser++){
        set<string> nearby = getNearby(*parser);
        possible_collisions.insert(nearby.begin(), nearby.end());
    }

    for(set<string>::iterator parser = possible_collisions.begin(); parser!=possible_collisions.end(); parser++){
        shared_ptr<Entity> ent = getSector(sector_id)->getEnt( *parser);
        if(ent->id==omit){continue;}

        if(ent->intersects(point,radius)){

            return true;
        }

    }

    return false;
}


void CollisionGrid::findCollisions(double dt)
{

    for(unordered_map<int,unordered_map<int,set<string> > >::iterator grid_parser_x = grid.begin(); grid_parser_x!=grid.end(); grid_parser_x++)
    {

        for(unordered_map<int,set<string> >::iterator grid_parser_y = grid_parser_x->second.begin(); grid_parser_y!=grid_parser_x->second.end(); grid_parser_y++)
        {
            //each iteration is another cell

            for(set<string>::iterator parser = grid_parser_y->second.begin(); parser!=grid_parser_y->second.end(); parser++)
            {
                //for each item in the cell
                shared_ptr<Entity> current_ent = getSector(sector_id)->getEnt(*parser);
                if(current_ent->slaved){continue;}
                if(current_ent->ghost){continue;}
                for(set<string>::iterator parser2 = parser; parser2!=grid_parser_y->second.end(); parser2++)
                {
                    //check against all others
                    if(parser==parser2){continue;} //prevents entities from being checked against themselves

                    shared_ptr<Entity> check_ent = getSector(sector_id)->getEnt(*parser2);
                    if(check_ent->slaved){continue;}
                    if(check_ent->ghost){continue;}
                    if(current_ent->bonded and check_ent->bonded and current_ent->bond==check_ent->bond){continue;} //ignore any collision if the two are bonded
                    if(current_ent->prop && check_ent->prop){continue;}

                    Vector2f ent_dist = current_ent->getPosition() - check_ent->getPosition(); //perform a quick radial check to see if a collision is remotely possible
                    if(((ent_dist.x*ent_dist.x) + (ent_dist.y*ent_dist.y)) > (current_ent->collision_radius + check_ent->collision_radius)*(current_ent->collision_radius + check_ent->collision_radius)){ continue;}

                    if(current_ent->intersects(*parser2)) //determine exactly if the entities are overlapping
                    {
                        correctPositions(current_ent,check_ent, dt);

                        if(current_ent->temperature > 398.15 || check_ent->temperature > 398.15){
                            makeNewConnector(sector_id, createUniqueId(), "WELD", *parser, *parser2);

                            Vector2f dist = check_ent->getPosition()-current_ent->getPosition();
                            double h = hypot(dist.x,dist.y);
                            dist = dist/h;

                            Vector2f pos = current_ent->getPosition() + (dist*current_ent->collision_radius);
                            createEffect(sector_id, pos, Vector2f(0,0), "Smoke");
                        }



                        redistributeMomentum(current_ent,check_ent, dt);
                    }
                }
            }
        }
    }


    //limbo
    for(set<string>::iterator parser = limbo.begin(); parser!=limbo.end(); parser++)
    {
        //for each item in the cell
        shared_ptr<Entity> current_ent = getSector(sector_id)->getEnt(*parser);
        if(current_ent->slaved){continue;}
        for(set<string>::iterator parser2 = parser; parser2!=limbo.end(); parser2++)
        {
            //check against all others
            if(parser==parser2){continue;}
            shared_ptr<Entity> check_ent = getSector(sector_id)->getEnt(*parser2);
            if(check_ent->slaved){continue;}
            if(current_ent->bonded and check_ent->bonded and current_ent->bond==check_ent->bond){continue;}

            Vector2f ent_dist = current_ent->getPosition() - check_ent->getPosition(); //perform a quick radial check to see if a collision is remotely possible
            if(((ent_dist.x*ent_dist.x) + (ent_dist.y*ent_dist.y)) > (current_ent->collision_radius + check_ent->collision_radius)*(current_ent->collision_radius + check_ent->collision_radius)){ continue;}


            if(current_ent->intersects(*parser2))
            {

                correctPositions(current_ent,check_ent, dt);

                if(current_ent->temperature > 398.15 || check_ent->temperature > 398.15){
                    makeNewConnector(sector_id, createUniqueId(), "WELD", *parser, *parser2);

                    Vector2f dist = check_ent->getPosition()-current_ent->getPosition();
                    double h = hypot(dist.x,dist.y);
                    dist = dist/h;

                    Vector2f pos = current_ent->getPosition() + (dist*current_ent->collision_radius);
                    createEffect(sector_id, pos, Vector2f(0,0), "Smoke");
                }



                redistributeMomentum(current_ent,check_ent, dt);

            }
        }
    }
}

void CollisionGrid::processCollisions(double dt)
{
    findCollisions(dt);

}

void CollisionGrid::insertField(string field_id){

    shared_ptr<Field> field = getSector(sector_id)->fields[field_id];

    field->grid_cells.clear();

    if(field->bounds_type == 0){
        double ob_cell_x = closestLowestInt(field->bounds_r.getPosition().x/cell_side_length);
        double ob_cell_y = closestLowestInt(field->bounds_r.getPosition().y/cell_side_length);

        double border_distance = closestLowestInt(ceil(hypot(field->bounds_r.getOrigin().x,field->bounds_r.getOrigin().y)/cell_side_length));
        Vector2f topleft(ob_cell_x - border_distance,ob_cell_y - border_distance);
        Vector2f bottomright(ob_cell_x + border_distance,ob_cell_y + border_distance);

        bool in_cell = false;
        for(int x = topleft.x; x <= bottomright.x; x++){
            for(int y = topleft.y; y <= bottomright.y; y++){
                if(grid.count(x)!=0 and grid[x].count(y)!=0)
                {
                    field->grid_cells.push_back(Vector2f(x,y));
                }
            }
        }
    }
    else if(field->bounds_type == 1){
        double ob_cell_x = closestLowestInt(field->bounds_c.getPosition().x/cell_side_length);
        double ob_cell_y = closestLowestInt(field->bounds_c.getPosition().y/cell_side_length);

        double border_distance = closestLowestInt(ceil(field->bounds_c.getRadius()/cell_side_length));
        Vector2f topleft(ob_cell_x - border_distance,ob_cell_y - border_distance);
        Vector2f bottomright(ob_cell_x + border_distance,ob_cell_y + border_distance);

        bool in_cell = false;
        for(int x = topleft.x; x <= bottomright.x; x++){
            for(int y = topleft.y; y <= bottomright.y; y++){
                if(grid.count(x)!=0 and grid[x].count(y)!=0)
                {
                    field->grid_cells.push_back(Vector2f(x,y));
                }
            }
        }
    }

}

void CollisionGrid::processFields(){

    for(map<string,shared_ptr<Field> >::iterator parser = getSector(sector_id)->fields.begin(); parser != getSector(sector_id)->fields.end(); parser++){
        for(vector<Vector2f>::iterator cell_coords = parser->second->grid_cells.begin(); cell_coords != parser->second->grid_cells.end(); cell_coords++){
            set<string> possible_collisions = grid[cell_coords->x][cell_coords->y];
            for(set<string>::iterator ent_id = possible_collisions.begin(); ent_id != possible_collisions.end(); ent_id++){
                if(getSector(sector_id)->getEnt(*ent_id)){
                    if(parser->second->bounds_type == 0){
                        if(spriteIntersectingShape(getSector(sector_id)->getEnt(*ent_id)->sprite,parser->second->bounds_r)){
                            parser->second->activateFor(*ent_id);
                        }
                    }
                    else if(parser->second->bounds_type == 1){
                        if(spriteIntersectingShape(getSector(sector_id)->getEnt(*ent_id)->sprite,parser->second->bounds_c)){
                            parser->second->activateFor(*ent_id);
                        }
                    }
                }

            }
        }
    }
}

double getGap(shared_ptr<Entity> A,shared_ptr<Entity> B, Vector2f axis)
{
    //returns the magnitude of the gap between two entites (A and B) when projected on a specific axis.
    //returns the magnitude of the gap between two entites (A and B) when projected on a specific axis.

    Vector2f maxA = A->getMaxPoint(axis); //get the corner of A that projects farthest "up" the axis P
    Vector2f minB = B->getMinPoint(axis); //get the corner of B that projects farthest "down" the axis P
    Vector2f maxB = B->getMaxPoint(axis);
    Vector2f minA = A->getMinPoint(axis);

    double pCB = B->getPosition().x*axis.x + B->getPosition().y*axis.y;
    double pCA = A->getPosition().x*axis.x + A->getPosition().y*axis.y;

    double pC = pCB-pCA;


    double pA = maxA.x*axis.x + maxA.y*axis.y; //calculate the X and Y components of a diagonal line drawn between the center of A and its maxpoint on P

    double pB = minB.x*axis.x + minB.y*axis.y; //calculate the X and Y components of a diagonal line drawn between the center of B and its minpoint on P

    double pmA = abs(pA-pCA); //calculate the magnitude of the projection of A's center-to-corner diagonal on P
    double pmB = abs(pB-pCB); //calculate the magnitude of the projection of B's center-to-corner diagonal on P

    return ((abs(pC) - pmA) - pmB); //calculate the difference between how far away A is from B (pC) and how far away the two need to be (pA + pB)}

}

bool pointInSprite(Sprite a, Vector2f point){

     FloatRect rect_a = a.getLocalBounds();

     Vector2f a_tr = Vector2f(rect_a.width/2,-rect_a.height/2);
     Vector2f a_tl = Vector2f(-rect_a.width/2,-rect_a.height/2);
     Vector2f a_br = Vector2f(rect_a.width/2,rect_a.height/2);
     Vector2f a_bl = Vector2f(-rect_a.width/2,rect_a.height/2);

     Vector2f a_ctr = Vector2f(a.getPosition().x, a.getPosition().y);

     double rot_a = a.getRotation();

     double Q_a = rot_a * (M_PI/180);

     a_tr = a_ctr + Vector2f(a_tr.x*cos(Q_a)-a_tr.y*sin(Q_a),a_tr.x*sin(Q_a)+a_tr.y*cos(Q_a));
     a_tl = a_ctr + Vector2f(a_tl.x*cos(Q_a)-a_tl.y*sin(Q_a),a_tl.x*sin(Q_a)+a_tl.y*cos(Q_a));
     a_br = a_ctr + Vector2f(a_br.x*cos(Q_a)-a_br.y*sin(Q_a),a_br.x*sin(Q_a)+a_br.y*cos(Q_a));
     a_bl = a_ctr + Vector2f(a_bl.x*cos(Q_a)-a_bl.y*sin(Q_a),a_bl.x*sin(Q_a)+a_bl.y*cos(Q_a));

     vector<Vector2f> axises;
     Vector2f axis_1 = a_tr - a_tl;
     axises.push_back(axis_1/hypot(axis_1.x,axis_1.y));
     Vector2f axis_2 = a_br - a_tr;
     axises.push_back(axis_2/hypot(axis_2.x,axis_2.y));

    bool intersecting = true;
    for(vector<Vector2f>::iterator parser = axises.begin(); parser!=axises.end(); parser++){

        double min_a = parser->x*a_tr.x+parser->y*a_tr.y;
        double max_a = parser->x*a_tr.x+parser->y*a_tr.y;
        if(parser->x*a_tl.x+parser->y*a_tl.y<min_a){min_a = parser->x*a_tl.x+parser->y*a_tl.y;}
        if(parser->x*a_tl.x+parser->y*a_tl.y>max_a){max_a = parser->x*a_tl.x+parser->y*a_tl.y;}
        if(parser->x*a_bl.x+parser->y*a_bl.y<min_a){min_a = parser->x*a_bl.x+parser->y*a_bl.y;}
        if(parser->x*a_bl.x+parser->y*a_bl.y>max_a){max_a = parser->x*a_bl.x+parser->y*a_bl.y;}
        if(parser->x*a_br.x+parser->y*a_br.y<min_a){min_a = parser->x*a_br.x+parser->y*a_br.y;}
        if(parser->x*a_br.x+parser->y*a_br.y>max_a){max_a = parser->x*a_br.x+parser->y*a_br.y;}

        double ctr_a = parser->x*a_ctr.x+parser->y*a_ctr.y;
        double projected_point = parser->x*point.x+parser->y*point.y;

        if((min_a <= projected_point && max_a >= projected_point) || (min_a >= projected_point && max_a <= projected_point)){
            continue;
        }
        else{
            intersecting = false;
            break;
        }
    }

    return intersecting;
}

bool spritesIntersecting(Sprite a, Sprite b, double tolerance){

     FloatRect rect_a = a.getLocalBounds();
     FloatRect rect_b = b.getLocalBounds();

     Vector2f a_tr = Vector2f(rect_a.width/2,-rect_a.height/2);
     Vector2f a_tl = Vector2f(-rect_a.width/2,-rect_a.height/2);
     Vector2f a_br = Vector2f(rect_a.width/2,rect_a.height/2);
     Vector2f a_bl = Vector2f(-rect_a.width/2,rect_a.height/2);

     Vector2f a_ctr = Vector2f(a.getPosition().x, a.getPosition().y);

     Vector2f b_tr = Vector2f(rect_b.width/2,-rect_b.height/2);
     Vector2f b_tl = Vector2f(-rect_b.width/2,-rect_b.height/2);
     Vector2f b_br = Vector2f(rect_b.width/2,rect_b.height/2);
     Vector2f b_bl = Vector2f(-rect_b.width/2,rect_b.height/2);

     Vector2f b_ctr = Vector2f(b.getPosition().x, b.getPosition().y);

     double rot_a = a.getRotation();
     double rot_b = b.getRotation();

     double Q_a = rot_a * (M_PI/180);
     double Q_b = rot_b * (M_PI/180);

     a_tr = a_ctr + Vector2f(a_tr.x*cos(Q_a)-a_tr.y*sin(Q_a),a_tr.x*sin(Q_a)+a_tr.y*cos(Q_a));
     a_tl = a_ctr + Vector2f(a_tl.x*cos(Q_a)-a_tl.y*sin(Q_a),a_tl.x*sin(Q_a)+a_tl.y*cos(Q_a));
     a_br = a_ctr + Vector2f(a_br.x*cos(Q_a)-a_br.y*sin(Q_a),a_br.x*sin(Q_a)+a_br.y*cos(Q_a));
     a_bl = a_ctr + Vector2f(a_bl.x*cos(Q_a)-a_bl.y*sin(Q_a),a_bl.x*sin(Q_a)+a_bl.y*cos(Q_a));

     b_tr = b_ctr + Vector2f(b_tr.x*cos(Q_b)-b_tr.y*sin(Q_b),b_tr.x*sin(Q_b)+b_tr.y*cos(Q_b));
     b_tl = b_ctr + Vector2f(b_tl.x*cos(Q_b)-b_tl.y*sin(Q_b),b_tl.x*sin(Q_b)+b_tl.y*cos(Q_b));
     b_br = b_ctr + Vector2f(b_br.x*cos(Q_b)-b_br.y*sin(Q_b),b_br.x*sin(Q_b)+b_br.y*cos(Q_b));
     b_bl = b_ctr + Vector2f(b_bl.x*cos(Q_b)-b_bl.y*sin(Q_b),b_bl.x*sin(Q_b)+b_bl.y*cos(Q_b));

     //everything above here just calculated the exact position of the four corners of each block

     vector<Vector2f> axises;
     Vector2f axis_1 = a_tr - a_tl;
     axises.push_back(axis_1/hypot(axis_1.x,axis_1.y));
     Vector2f axis_2 = a_br - a_tr;
     axises.push_back(axis_2/hypot(axis_2.x,axis_2.y));
     Vector2f axis_3 = b_tr - b_tl;
     axises.push_back(axis_3/hypot(axis_3.x,axis_3.y));
     Vector2f axis_4 = b_br - b_tr;
     axises.push_back(axis_4/hypot(axis_4.x,axis_4.y));

    //the "axises" vector is just a list of axises we want to project onto to check collision

     for(vector<Vector2f>::iterator parser = axises.begin(); parser!=axises.end(); parser++){

        double min_a = parser->x*a_tr.x+parser->y*a_tr.y;
        double max_a = parser->x*a_tr.x+parser->y*a_tr.y;
        if(parser->x*a_tl.x+parser->y*a_tl.y<min_a){min_a = parser->x*a_tl.x+parser->y*a_tl.y;}
        if(parser->x*a_tl.x+parser->y*a_tl.y>max_a){max_a = parser->x*a_tl.x+parser->y*a_tl.y;}
        if(parser->x*a_bl.x+parser->y*a_bl.y<min_a){min_a = parser->x*a_bl.x+parser->y*a_bl.y;}
        if(parser->x*a_bl.x+parser->y*a_bl.y>max_a){max_a = parser->x*a_bl.x+parser->y*a_bl.y;}
        if(parser->x*a_br.x+parser->y*a_br.y<min_a){min_a = parser->x*a_br.x+parser->y*a_br.y;}
        if(parser->x*a_br.x+parser->y*a_br.y>max_a){max_a = parser->x*a_br.x+parser->y*a_br.y;}

        double min_b = parser->x*b_tr.x+parser->y*b_tr.y;
        double max_b = parser->x*b_tr.x+parser->y*b_tr.y;
        if(parser->x*b_tl.x+parser->y*b_tl.y<min_b){min_b = parser->x*b_tl.x+parser->y*b_tl.y;}
        if(parser->x*b_tl.x+parser->y*b_tl.y>max_b){max_b = parser->x*b_tl.x+parser->y*b_tl.y;}
        if(parser->x*b_bl.x+parser->y*b_bl.y<min_b){min_b = parser->x*b_bl.x+parser->y*b_bl.y;}
        if(parser->x*b_bl.x+parser->y*b_bl.y>max_b){max_b = parser->x*b_bl.x+parser->y*b_bl.y;}
        if(parser->x*b_br.x+parser->y*b_br.y<min_b){min_b = parser->x*b_br.x+parser->y*b_br.y;}
        if(parser->x*b_br.x+parser->y*b_br.y>max_b){max_b = parser->x*b_br.x+parser->y*b_br.y;}

        double ctr_a = parser->x*a_ctr.x+parser->y*a_ctr.y;
        double ctr_b = parser->x*b_ctr.x+parser->y*b_ctr.y;

        if(abs(max_a-min_b)<abs(max_b-min_a)){
            double dist = ctr_a - ctr_b;
            double diag_a = max_a - ctr_a;
            double diag_b = min_b - ctr_b;

            if(abs(diag_a) + abs(diag_b) < abs(dist) + tolerance){
                return false;
            }
        }
        else{
            double dist = ctr_a - ctr_b;
            double diag_a = min_a - ctr_a;
            double diag_b = max_b - ctr_b;
            if(abs(diag_a) + abs(diag_b) < abs(dist) + tolerance){
                return false;
            }
        }

     }

     return true;
}


bool spriteIntersectingShape(Sprite a, RectangleShape b, double tolerance){

     FloatRect rect_a = a.getLocalBounds();
     FloatRect rect_b = b.getLocalBounds();

     Vector2f a_tr = Vector2f(rect_a.width/2,-rect_a.height/2);
     Vector2f a_tl = Vector2f(-rect_a.width/2,-rect_a.height/2);
     Vector2f a_br = Vector2f(rect_a.width/2,rect_a.height/2);
     Vector2f a_bl = Vector2f(-rect_a.width/2,rect_a.height/2);

     Vector2f a_ctr = Vector2f(a.getPosition().x, a.getPosition().y);

     Vector2f b_tr = Vector2f(rect_b.width/2,-rect_b.height/2);
     Vector2f b_tl = Vector2f(-rect_b.width/2,-rect_b.height/2);
     Vector2f b_br = Vector2f(rect_b.width/2,rect_b.height/2);
     Vector2f b_bl = Vector2f(-rect_b.width/2,rect_b.height/2);

     Vector2f b_ctr = Vector2f(b.getPosition().x, b.getPosition().y);

     double rot_a = a.getRotation();
     double rot_b = b.getRotation();

     double Q_a = rot_a * (M_PI/180);
     double Q_b = rot_b * (M_PI/180);

     a_tr = a_ctr + Vector2f(a_tr.x*cos(Q_a)-a_tr.y*sin(Q_a),a_tr.x*sin(Q_a)+a_tr.y*cos(Q_a));
     a_tl = a_ctr + Vector2f(a_tl.x*cos(Q_a)-a_tl.y*sin(Q_a),a_tl.x*sin(Q_a)+a_tl.y*cos(Q_a));
     a_br = a_ctr + Vector2f(a_br.x*cos(Q_a)-a_br.y*sin(Q_a),a_br.x*sin(Q_a)+a_br.y*cos(Q_a));
     a_bl = a_ctr + Vector2f(a_bl.x*cos(Q_a)-a_bl.y*sin(Q_a),a_bl.x*sin(Q_a)+a_bl.y*cos(Q_a));

     b_tr = b_ctr + Vector2f(b_tr.x*cos(Q_b)-b_tr.y*sin(Q_b),b_tr.x*sin(Q_b)+b_tr.y*cos(Q_b));
     b_tl = b_ctr + Vector2f(b_tl.x*cos(Q_b)-b_tl.y*sin(Q_b),b_tl.x*sin(Q_b)+b_tl.y*cos(Q_b));
     b_br = b_ctr + Vector2f(b_br.x*cos(Q_b)-b_br.y*sin(Q_b),b_br.x*sin(Q_b)+b_br.y*cos(Q_b));
     b_bl = b_ctr + Vector2f(b_bl.x*cos(Q_b)-b_bl.y*sin(Q_b),b_bl.x*sin(Q_b)+b_bl.y*cos(Q_b));

     //everything above here just calculated the exact position of the four corners of each block

     vector<Vector2f> axises;
     Vector2f axis_1 = a_tr - a_tl;
     axises.push_back(axis_1/hypot(axis_1.x,axis_1.y));
     Vector2f axis_2 = a_br - a_tr;
     axises.push_back(axis_2/hypot(axis_2.x,axis_2.y));
     Vector2f axis_3 = b_tr - b_tl;
     axises.push_back(axis_3/hypot(axis_3.x,axis_3.y));
     Vector2f axis_4 = b_br - b_tr;
     axises.push_back(axis_4/hypot(axis_4.x,axis_4.y));

    //the "axises" vector is just a list of axises we want to project onto to check collision

     for(vector<Vector2f>::iterator parser = axises.begin(); parser!=axises.end(); parser++){

        double min_a = parser->x*a_tr.x+parser->y*a_tr.y;
        double max_a = parser->x*a_tr.x+parser->y*a_tr.y;
        if(parser->x*a_tl.x+parser->y*a_tl.y<min_a){min_a = parser->x*a_tl.x+parser->y*a_tl.y;}
        if(parser->x*a_tl.x+parser->y*a_tl.y>max_a){max_a = parser->x*a_tl.x+parser->y*a_tl.y;}
        if(parser->x*a_bl.x+parser->y*a_bl.y<min_a){min_a = parser->x*a_bl.x+parser->y*a_bl.y;}
        if(parser->x*a_bl.x+parser->y*a_bl.y>max_a){max_a = parser->x*a_bl.x+parser->y*a_bl.y;}
        if(parser->x*a_br.x+parser->y*a_br.y<min_a){min_a = parser->x*a_br.x+parser->y*a_br.y;}
        if(parser->x*a_br.x+parser->y*a_br.y>max_a){max_a = parser->x*a_br.x+parser->y*a_br.y;}

        double min_b = parser->x*b_tr.x+parser->y*b_tr.y;
        double max_b = parser->x*b_tr.x+parser->y*b_tr.y;
        if(parser->x*b_tl.x+parser->y*b_tl.y<min_b){min_b = parser->x*b_tl.x+parser->y*b_tl.y;}
        if(parser->x*b_tl.x+parser->y*b_tl.y>max_b){max_b = parser->x*b_tl.x+parser->y*b_tl.y;}
        if(parser->x*b_bl.x+parser->y*b_bl.y<min_b){min_b = parser->x*b_bl.x+parser->y*b_bl.y;}
        if(parser->x*b_bl.x+parser->y*b_bl.y>max_b){max_b = parser->x*b_bl.x+parser->y*b_bl.y;}
        if(parser->x*b_br.x+parser->y*b_br.y<min_b){min_b = parser->x*b_br.x+parser->y*b_br.y;}
        if(parser->x*b_br.x+parser->y*b_br.y>max_b){max_b = parser->x*b_br.x+parser->y*b_br.y;}

        double ctr_a = parser->x*a_ctr.x+parser->y*a_ctr.y;
        double ctr_b = parser->x*b_ctr.x+parser->y*b_ctr.y;

        if(abs(max_a-min_b)<abs(max_b-min_a)){
            double dist = ctr_a - ctr_b;
            double diag_a = max_a - ctr_a;
            double diag_b = min_b - ctr_b;

            if(abs(diag_a) + abs(diag_b) < abs(dist) + tolerance){
                return false;
            }
        }
        else{
            double dist = ctr_a - ctr_b;
            double diag_a = min_a - ctr_a;
            double diag_b = max_b - ctr_b;
            if(abs(diag_a) + abs(diag_b) < abs(dist) + tolerance){
                return false;
            }
        }

     }

     return true;
}


bool spriteIntersectingShape(Sprite a, CircleShape b, double tolerance){

     FloatRect rect_a = a.getLocalBounds();

     Vector2f a_tr = Vector2f(rect_a.width/2,-rect_a.height/2);
     Vector2f a_tl = Vector2f(-rect_a.width/2,-rect_a.height/2);
     Vector2f a_br = Vector2f(rect_a.width/2,rect_a.height/2);
     Vector2f a_bl = Vector2f(-rect_a.width/2,rect_a.height/2);

     Vector2f a_ctr = Vector2f(a.getPosition().x, a.getPosition().y);
     Vector2f b_ctr = Vector2f(b.getPosition().x, b.getPosition().y);

     double rot_a = a.getRotation();
     double Q_a = rot_a * (M_PI/180);

     a_tr = a_ctr + Vector2f(a_tr.x*cos(Q_a)-a_tr.y*sin(Q_a),a_tr.x*sin(Q_a)+a_tr.y*cos(Q_a));
     a_tl = a_ctr + Vector2f(a_tl.x*cos(Q_a)-a_tl.y*sin(Q_a),a_tl.x*sin(Q_a)+a_tl.y*cos(Q_a));
     a_br = a_ctr + Vector2f(a_br.x*cos(Q_a)-a_br.y*sin(Q_a),a_br.x*sin(Q_a)+a_br.y*cos(Q_a));
     a_bl = a_ctr + Vector2f(a_bl.x*cos(Q_a)-a_bl.y*sin(Q_a),a_bl.x*sin(Q_a)+a_bl.y*cos(Q_a));


     //everything above here just calculated the exact position of the four corners of each block

     vector<Vector2f> axises;
     Vector2f axis_1 = a_tr - a_tl;
     axises.push_back(axis_1/hypot(axis_1.x,axis_1.y));
     Vector2f axis_2 = a_br - a_tr;
     axises.push_back(axis_2/hypot(axis_2.x,axis_2.y));

    //the "axises" vector is just a list of axises we want to project onto to check collision

     for(vector<Vector2f>::iterator parser = axises.begin(); parser!=axises.end(); parser++){

        double min_a = parser->x*a_tr.x+parser->y*a_tr.y;
        double max_a = parser->x*a_tr.x+parser->y*a_tr.y;
        if(parser->x*a_tl.x+parser->y*a_tl.y<min_a){min_a = parser->x*a_tl.x+parser->y*a_tl.y;}
        if(parser->x*a_tl.x+parser->y*a_tl.y>max_a){max_a = parser->x*a_tl.x+parser->y*a_tl.y;}
        if(parser->x*a_bl.x+parser->y*a_bl.y<min_a){min_a = parser->x*a_bl.x+parser->y*a_bl.y;}
        if(parser->x*a_bl.x+parser->y*a_bl.y>max_a){max_a = parser->x*a_bl.x+parser->y*a_bl.y;}
        if(parser->x*a_br.x+parser->y*a_br.y<min_a){min_a = parser->x*a_br.x+parser->y*a_br.y;}
        if(parser->x*a_br.x+parser->y*a_br.y>max_a){max_a = parser->x*a_br.x+parser->y*a_br.y;}

        double min_b = (parser->x*b_ctr.x+parser->y*b_ctr.y) + b.getRadius();
        double max_b = (parser->x*b_ctr.x+parser->y*b_ctr.y) + b.getRadius();
        if((parser->x*b_ctr.x+parser->y*b_ctr.y) - b.getRadius()<min_b){min_b = (parser->x*b_ctr.x+parser->y*b_ctr.y) - b.getRadius();}
        if((parser->x*b_ctr.x+parser->y*b_ctr.y) - b.getRadius()>max_b){max_b = (parser->x*b_ctr.x+parser->y*b_ctr.y) - b.getRadius();}

        double ctr_a = parser->x*a_ctr.x+parser->y*a_ctr.y;
        double ctr_b = parser->x*b_ctr.x+parser->y*b_ctr.y;

        if(abs(max_a-min_b)<abs(max_b-min_a)){
            double dist = ctr_a - ctr_b;
            double diag_a = max_a - ctr_a;
            double diag_b = min_b - ctr_b;

            if(abs(diag_a) + abs(diag_b) < abs(dist) + tolerance){
                return false;
            }
        }
        else{
            double dist = ctr_a - ctr_b;
            double diag_a = min_a - ctr_a;
            double diag_b = max_b - ctr_b;
            if(abs(diag_a) + abs(diag_b) < abs(dist) + tolerance){
                return false;
            }
        }

     }

     return true;
}

Vector2f correctPositions(shared_ptr<Entity> A, shared_ptr<Entity> B, double dt)
{

    if(A->dead or B->dead){return Vector2f(0,0);}

    Vector2f disp = A->getPosition()-B->getPosition();
    if(A->collision_radius+B->collision_radius+5 <= hypot(disp.x,disp.y)){return Vector2f(0,0);}

    vector<Vector2f> axises;

    axises.push_back(A->getMyParallelAxis());
    axises.push_back(A->getMyPerpendicularAxis());
    axises.push_back(B->getMyParallelAxis());
    axises.push_back(B->getMyPerpendicularAxis());

    double minimum = getGap(A, B, axises[0]);

    bool only_correct_A = false;
    bool only_correct_B = false;
    if(A->floor_bonds.count(B->bond)!=0 || B->prop || (B->bonded && !getSector(B->sector_id)->bonds[B->bond]->moveable)){
        only_correct_A = true;
    }
    else if(B->floor_bonds.count(A->bond)!=0 || A->prop|| (A->bonded && !getSector(A->sector_id)->bonds[A->bond]->moveable)){
        only_correct_B = true;
    }


    Vector2f minaxis = axises[0];

    for(vector<Vector2f>::iterator i = axises.begin(); i != axises.end(); i++)
    {
        double newgap = getGap(A, B, *i);
        if(abs(newgap)<abs(minimum)){minimum=abs(newgap)    ; minaxis = *i;}
    }

    Vector2f col_point = Vector2f(0,0);
    Vector2f closest_A = A->getClosestPoint(B->getPosition());
    Vector2f closest_B = B->getClosestPoint(A->getPosition());
    double distA = hypot(closest_A.x-B->getPosition().x,closest_A.y-B->getPosition().y);
    double distB = hypot(closest_B.x-A->getPosition().x,closest_B.y-A->getPosition().y);
    if(distA<=distB){col_point = closest_A;}
    else{col_point = closest_B;}


    if((A->getPosition().x*minaxis.x)+(A->getPosition().y*minaxis.y)>=(B->getPosition().x*minaxis.x)+(B->getPosition().y*minaxis.y))
    {

        //if A is above B on the axis
        Vector2f point_velocity_A = A->getVelocity()+A->getAngularVelocity(dt,col_point);
        Vector2f point_velocity_B = B->getVelocity()+B->getAngularVelocity(dt,col_point);

        double velocityA = (-1*minaxis.x*point_velocity_A.x + -1*minaxis.y*point_velocity_A.y)*dt;
        double velocityB = (minaxis.x*point_velocity_B.x + minaxis.y*point_velocity_B.y)*dt;

        velocityA=A->getMass();
        velocityB=B->getMass();

        if(velocityA<0){velocityA=0;}
        if(velocityB<0){velocityB=0;}

        double total_closing_velocity = velocityA + velocityB;
        if(total_closing_velocity!=0)
        {
            double percentA = velocityB/total_closing_velocity;
            double percentB = velocityA/total_closing_velocity;

            if(only_correct_A){percentB = 0;}
            else if(only_correct_B){percentA = 0;}

            A->displace(minaxis.x*abs(minimum*percentA),minaxis.y*abs(minimum*percentA));
            B->displace(-minaxis.x*abs(minimum*percentB),-minaxis.y*abs(minimum*percentB));

        }

    }
    else
    {
        //if B is above A on the axis
        Vector2f point_velocity_A = A->getVelocity()+A->getAngularVelocity(dt,col_point);
        Vector2f point_velocity_B = B->getVelocity()+B->getAngularVelocity(dt,col_point);

        double velocityA = (minaxis.x*point_velocity_A.x + minaxis.y*point_velocity_A.y)*dt;
        double velocityB = (-1*minaxis.x*point_velocity_B.x + -1*minaxis.y*point_velocity_B.y)*dt;

        velocityA=A->getMass();
        velocityB=B->getMass();

        if(velocityA<0){velocityA=0;}
        if(velocityB<0){velocityB=0;}

        double total_closing_velocity = velocityA + velocityB;
        if(total_closing_velocity!=0)
        {
            double percentA = velocityB/total_closing_velocity;
            double percentB = velocityA/total_closing_velocity;

            if(only_correct_A){percentB = 0;}
            else if(only_correct_B){percentA = 0;}

            A->displace(-minaxis.x*abs(minimum*percentA),-minaxis.y*abs(minimum*percentA));
            B->displace(minaxis.x*abs(minimum*percentB),minaxis.y*abs(minimum*percentB));

        }



    }

    return minaxis;
}



void redistributeMomentum(shared_ptr<Entity> A, shared_ptr<Entity> B, double dt)
{

        if(A->dead or B->dead){return;}

        bool only_correct_A = false;
        bool only_correct_B = false;
        if( B->prop || (B->bonded && !getSector(B->sector_id)->bonds[B->bond]->moveable)){
            only_correct_A = true;
        }
        else if( A->prop || (A->bonded && !getSector(A->sector_id)->bonds[A->bond]->moveable)){
            only_correct_B = true;
        }

        //determine collision point
        Vector2f col_point = Vector2f(0,0);
        Vector2f closest_A = A->getClosestPoint(B->getPosition());
        Vector2f closest_B = B->getClosestPoint(A->getPosition());
        double distA = hypot(closest_A.x-B->getPosition().x,closest_A.y-B->getPosition().y);
        double distB = hypot(closest_B.x-A->getPosition().x,closest_B.y-A->getPosition().y);
        if(distA<=distB){col_point = closest_A;}
        else{col_point = closest_B;}


       Vector2f velocity_A = A->getVelocity()+A->getAngularVelocity(dt,col_point);
       Vector2f velocity_B = B->getVelocity()+B->getAngularVelocity(dt,col_point);

       Vector2f sumMomentum = ((velocity_A*A->getMass())+(velocity_B*B->getMass()));

       double sumMass = A->getMass() + B->getMass();

        double uVx = sumMomentum.x/sumMass; //unified x velocity
        double uVy = sumMomentum.y/sumMass;//unified y velocity

        if(only_correct_A){
            A->applyImpulse((uVx-velocity_A.x)*A->getMass(),(uVy-velocity_A.y)*A->getMass(),col_point,true,false);
        }
        else if(only_correct_B){
            B->applyImpulse((uVx-velocity_B.x)*B->getMass(),(uVy-velocity_B.y)*B->getMass(),col_point,true,false);
        }
        else{
            A->applyImpulse((uVx-velocity_A.x)*A->getMass(),(uVy-velocity_A.y)*A->getMass(),col_point,true,false);
            B->applyImpulse((uVx-velocity_B.x)*B->getMass(),(uVy-velocity_B.y)*B->getMass(),col_point,true,false);

        }




}

bool weakLessThan(weak_ptr<Entity> e1, weak_ptr<Entity> e2)
{
    if(e1.lock()<e2.lock()){return true;}
    else{return false;}
}

//bool doesCollide(Entity *A, Entity *B)
//{
//    if(A->bonded and B->bonded and A->bond==B->bond){return false;}
//
//    vector<Vector2f> axises;
//
//    A->collectNormals(axises); //collect the normals of every side of each object to use as axises for collision checks
//    B->collectNormals(axises);
//
//    for(vector<Vector2f>::iterator i = axises.begin(); i != axises.end(); i++)
//    {
//         //*i is unit vector of axis to project onto
//
//        if(getGap(A, B, *i)>0) //if pC is greater than pA + pB then A and B are not intersecting
//        {
//           return false;
//        }
//    }
//
//    //otherwise they are
//
//    return true;
//
//}
//
//void getCollided(string id, set<string> &reactlist)
//{
//    reactlist.insert(id);
//
//    for(set<string>::iterator i = getSector(server.sector_id)->ents[id]->collided.begin(); i != getSector(server.sector_id)->ents[id]->collided.end(); i++)
//    {
//        bool check = false;
//        set<string>::iterator itCheck = reactlist.find(*i);
//        if(itCheck!=reactlist.end())
//        {
//           continue;
//        }
//        getCollided(*i, reactlist);
//    }
//}
//
//void colReact(string id1, string id2, double dt)
//{
//
//
//    set<string> reactlist;
//    set<string> bondlist;
//
//    getCollided(id1, reactlist);
//    getCollided(id2, reactlist);
////
////    //colreact
//        double sumMass = 0;
//        double sumMomentumx = 0;
//        double sumMomentumy = 0;
////
//        for(set<string>::iterator i = reactlist.begin(); i != reactlist.end(); i++)
//        {
//            if(getSector(server.sector_id)->ents[*i]->bonded)
//            {
//                bondlist.insert(getSector(server.sector_id)->ents[*i]->bond);
//                continue;
//            }
//            double px = getSector(server.sector_id)->ents[*i]->backupvelocity.x*getSector(server.sector_id)->ents[*i]->getMass();
//            double py = getSector(server.sector_id)->ents[*i]->backupvelocity.y*getSector(server.sector_id)->ents[*i]->getMass();
//
//            sumMass += getSector(server.sector_id)->ents[*i]->getMass();
//            sumMomentumx += px;
//            sumMomentumy += py;
//        }
//
//        for(set<string>::iterator i = bondlist.begin(); i != bondlist.end(); i++)
//        {
//            double px = getSector(server.sector_id)->bonds[*i]->backupvelocity.x*getSector(server.sector_id)->bonds[*i]->mass;
//            double py = getSector(server.sector_id)->bonds[*i]->backupvelocity.y*getSector(server.sector_id)->bonds[*i]->mass;
//
//            sumMass += getSector(server.sector_id)->bonds[*i]->mass;
//            sumMomentumx += px;
//            sumMomentumy += py;
//        }
//
//        double uVx = 0;
//        double uVy = 0;
//
//
//        uVx = sumMomentumx/sumMass; //unified x velocity
//        uVy = sumMomentumy/sumMass;//unified y velocity
////
//
//         for(set<string>::iterator i = reactlist.begin(); i != reactlist.end(); i++)
//        {
//
//            //ents[*i]->setPosition(ents[*i]->backupcoords.x,ents[*i]->backupcoords.y);
//            //ents[*i]->setRotation(ents[*i]->backup_rotation);
//            for(set<string>::iterator k = i; k != reactlist.end(); k++)
//            {
//
//                if(k==i)
//                {
//                    continue;
//                }
//                correctPositions(getSector(server.sector_id)->ents[*i],getSector(server.sector_id)->ents[*k],dt);
//            }
//            getSector(server.sector_id)->ents[*i]->setVelocity(uVx,uVy);
//        }
//
//        for(set<string>::iterator i = bondlist.begin(); i != bondlist.end(); i++)
//        {
//            getSector(server.sector_id)->bonds[*i]->velocity = Vector2f(uVx, uVy);
//        }
//
//        for(set<string>::iterator i = reactlist.begin(); i != reactlist.end(); i++) //add itself to every above
//        {                                                                           //add every above to itself
//            for(set<string>::iterator k = i; k != reactlist.end(); k++)
//            {
//                if(k==i)
//                {
//                    continue;
//                }
//
//                getSector(server.sector_id)->ents[*i]->collided.insert(*k);
//                getSector(server.sector_id)->ents[*k]->collided.insert(*i);
//            }
//        }
//
//}
