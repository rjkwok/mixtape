#include "main.h"

using namespace std;
using namespace sf;

extern set<int> collidable_terrain_types;

double distanceFromGround(FloatRect rect, Terrain &terrain){

	double bottom = rect.top + rect.height;

	//subdivide the texture rect into enough sample points to get an accurate check against this grid resolution
    int checkpoints_across = ceil(rect.width/terrain.tile_size);
    double smallest_height_gap = 999999999999;
    for(int index = 0; index < checkpoints_across; index++){

    	int terrain_index_x = floor((rect.left + (index*(rect.width/checkpoints_across)) - terrain.grid_ref.x)/terrain.tile_size);
        int terrain_index_y = floor((bottom-terrain.grid_ref.y)/-terrain.tile_size);

        int faked_terrain_index_x = terrain_index_x;
        if(terrain.looping && faked_terrain_index_x >= terrain.max_x){ faked_terrain_index_x -= terrain.max_x; }
        if(terrain.looping && faked_terrain_index_x < 0){ faked_terrain_index_x += terrain.max_x; }

        while(terrain_index_y > terrain.max_y || collidable_terrain_types.count(terrain.grid[faked_terrain_index_x][terrain_index_y]) == 0){
        	//if there's no collidable block in this grid tile
        	terrain_index_y--;
        	if(terrain_index_y < 2){ break; }
        }

       	double height_gap = abs(bottom - terrain.grid_ref.y) - ((terrain_index_y+1)*terrain.tile_size);
       	if(index == 0 || height_gap < smallest_height_gap){
       		smallest_height_gap = height_gap;
       	}
    }

    return smallest_height_gap;
}

bool isIntersectingTerrain(Sprite a, Terrain &terrain){

    FloatRect rect = a.getLocalBounds();
    vector<Vector2f> checkpoints;

    //subdivide the texture rect into enough sample points to get an accurate check against this grid resolution
    double terrain_grid_size = terrain.tile_size;
    int checkpoints_across = ceil(rect.width/terrain.tile_size);
    int checkpoints_down = ceil(rect.height/terrain.tile_size);

    double cell_size_across = rect.width/checkpoints_across;
    double cell_size_down = rect.height/checkpoints_down;

    checkpoints.push_back(Vector2f(-rect.width/2,-rect.height/2));
    for(int x = 0; x <= checkpoints_across; x++){
        for(int y = 0; y <= checkpoints_down; y++){
            if(x == 0 && y == 0){ continue;}

            checkpoints.push_back(checkpoints[0] + Vector2f(x*cell_size_across,y*cell_size_down));
        }
    }
    //

    //apply sprite's rotation and global position, then find relative position to grid_ref, then get a representative terrain square if intersecting immovable terrain
    FloatRect tile_rect;
    tile_rect.width = terrain.tile_size;
    tile_rect.height = terrain.tile_size;

    for(int index = 0; index < checkpoints.size(); index++){
    
        checkpoints[index] = a.getPosition() + rotateAboutOrigin(checkpoints[index], a.getRotation()) - terrain.grid_ref;

        int terrain_index_x = floor(checkpoints[index].x/terrain.tile_size);
        int terrain_index_y = floor(checkpoints[index].y/-terrain.tile_size);
        
        if(terrain_index_y > terrain.max_y){ continue; } // won't be a collision possible if this point is not in the terrain layer

        int faked_terrain_index_x = terrain_index_x;
        if(terrain.looping && faked_terrain_index_x >= terrain.max_x){ faked_terrain_index_x -= terrain.max_x; }
        if(terrain.looping && faked_terrain_index_x < 0){ faked_terrain_index_x += terrain.max_x; }

        if(collidable_terrain_types.count(terrain.grid[faked_terrain_index_x][terrain_index_y]) !=0){ return true; } //if this terrain block is collidable then the sprite is colliding
    }
    return false;
}
bool isIntersectingTerrain(Ship &a, Terrain &terrain){

    for(map<string, Sprite>::iterator i = a.sprite.begin(); i != a.sprite.end(); i++){
        if(isIntersectingTerrain(i->second, terrain)){
            return true;
        }
    }
    return false;
}
bool isIntersecting(Sprite a, Sprite b, double tolerance){

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

bool isIntersecting(Sprite a, FloatRect rect_b, double tolerance){

    FloatRect rect_a = a.getLocalBounds();
     

     Vector2f a_tr = Vector2f(rect_a.width/2,-rect_a.height/2);
     Vector2f a_tl = Vector2f(-rect_a.width/2,-rect_a.height/2);
     Vector2f a_br = Vector2f(rect_a.width/2,rect_a.height/2);
     Vector2f a_bl = Vector2f(-rect_a.width/2,rect_a.height/2);

     Vector2f a_ctr = Vector2f(a.getPosition().x, a.getPosition().y);

     Vector2f b_tr = Vector2f(rect_b.width/2,-rect_b.height/2);
     Vector2f b_tl = Vector2f(-rect_b.width/2,-rect_b.height/2);
     Vector2f b_br = Vector2f(rect_b.width/2,rect_b.height/2);
     Vector2f b_bl = Vector2f(-rect_b.width/2,rect_b.height/2);

     Vector2f b_ctr = Vector2f(rect_b.left + (rect_b.width/2.0), rect_b.top + (rect_b.height/2.0));

     double rot_a = a.getRotation();
     double rot_b = 0;

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

void untangleSprites(Sprite &a, Sprite &b){

	//determine how much the sprites are overlapping on each of the 4 axises defined by their rotations
	//move the sprites apart along the axis with the least

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

    Vector2f axis_1 = normalize(a_bl - a_tl);
    Vector2f axis_2 = normalize(a_tr - a_tl);
    Vector2f axis_3 = normalize(b_bl - b_tl);
    Vector2f axis_4 = normalize(b_tr - b_tl);

    double overlap_1 = 0;
	double overlap_2 = 0;
	double overlap_3 = 0;
	double overlap_4 = 0;
	bool a_before_b_1 = dot(axis_1, a_ctr) < dot(axis_1, b_ctr);
	bool a_before_b_2 = dot(axis_2, a_ctr) < dot(axis_2, b_ctr);
	bool a_before_b_3 = dot(axis_3, a_ctr) < dot(axis_3, b_ctr);
	bool a_before_b_4 = dot(axis_4, a_ctr) < dot(axis_4, b_ctr);

	a_tr = a_ctr + a_tr;
	a_tl = a_ctr + a_tl;
	a_br = a_ctr + a_br;
	a_bl = a_ctr + a_bl;

	b_tr = b_ctr + b_tr;
	b_tl = b_ctr + b_tl;
	b_br = b_ctr + b_br;
	b_bl = b_ctr + b_bl;

    if(a_before_b_1){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_1, a_tr);
    	if(dot(axis_1, a_tl) > max_a){ max_a = dot(axis_1, a_tl); }
    	if(dot(axis_1, a_bl) > max_a){ max_a = dot(axis_1, a_bl); }
    	if(dot(axis_1, a_br) > max_a){ max_a = dot(axis_1, a_br); }

    	double min_b = dot(axis_1, b_tr);
    	if(dot(axis_1, b_tl) < min_b){ min_b = dot(axis_1, b_tl); }
    	if(dot(axis_1, b_bl) < min_b){ min_b = dot(axis_1, b_bl); }
    	if(dot(axis_1, b_br) < min_b){ min_b = dot(axis_1, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_1 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_1, b_tr);
    	if(dot(axis_1, b_tl) > max_b){ max_b = dot(axis_1, b_tl); }
    	if(dot(axis_1, b_bl) > max_b){ max_b = dot(axis_1, b_bl); }
    	if(dot(axis_1, b_br) > max_b){ max_b = dot(axis_1, b_br); }

    	double min_a = dot(axis_1, a_tr);
    	if(dot(axis_1, a_tl) < min_a){ min_a = dot(axis_1, a_tl); }
    	if(dot(axis_1, a_bl) < min_a){ min_a = dot(axis_1, a_bl); }
    	if(dot(axis_1, a_br) < min_a){ min_a = dot(axis_1, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_1 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }
    if(a_before_b_2){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_2, a_tr);
    	if(dot(axis_2, a_tl) > max_a){ max_a = dot(axis_2, a_tl); }
    	if(dot(axis_2, a_bl) > max_a){ max_a = dot(axis_2, a_bl); }
    	if(dot(axis_2, a_br) > max_a){ max_a = dot(axis_2, a_br); }

    	double min_b = dot(axis_2, b_tr);
    	if(dot(axis_2, b_tl) < min_b){ min_b = dot(axis_2, b_tl); }
    	if(dot(axis_2, b_bl) < min_b){ min_b = dot(axis_2, b_bl); }
    	if(dot(axis_2, b_br) < min_b){ min_b = dot(axis_2, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_2 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_2, b_tr);
    	if(dot(axis_2, b_tl) > max_b){ max_b = dot(axis_2, b_tl); }
    	if(dot(axis_2, b_bl) > max_b){ max_b = dot(axis_2, b_bl); }
    	if(dot(axis_2, b_br) > max_b){ max_b = dot(axis_2, b_br); }

    	double min_a = dot(axis_2, a_tr);
    	if(dot(axis_2, a_tl) < min_a){ min_a = dot(axis_2, a_tl); }
    	if(dot(axis_2, a_bl) < min_a){ min_a = dot(axis_2, a_bl); }
    	if(dot(axis_2, a_br) < min_a){ min_a = dot(axis_2, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_2 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }
    if(a_before_b_3){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_3, a_tr);
    	if(dot(axis_3, a_tl) > max_a){ max_a = dot(axis_3, a_tl); }
    	if(dot(axis_3, a_bl) > max_a){ max_a = dot(axis_3, a_bl); }
    	if(dot(axis_3, a_br) > max_a){ max_a = dot(axis_3, a_br); }

    	double min_b = dot(axis_3, b_tr);
    	if(dot(axis_3, b_tl) < min_b){ min_b = dot(axis_3, b_tl); }
    	if(dot(axis_3, b_bl) < min_b){ min_b = dot(axis_3, b_bl); }
    	if(dot(axis_3, b_br) < min_b){ min_b = dot(axis_3, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_3 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_3, b_tr);
    	if(dot(axis_3, b_tl) > max_b){ max_b = dot(axis_3, b_tl); }
    	if(dot(axis_3, b_bl) > max_b){ max_b = dot(axis_3, b_bl); }
    	if(dot(axis_3, b_br) > max_b){ max_b = dot(axis_3, b_br); }

    	double min_a = dot(axis_3, a_tr);
    	if(dot(axis_3, a_tl) < min_a){ min_a = dot(axis_3, a_tl); }
    	if(dot(axis_3, a_bl) < min_a){ min_a = dot(axis_3, a_bl); }
    	if(dot(axis_3, a_br) < min_a){ min_a = dot(axis_3, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_3 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }
    if(a_before_b_4){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_4, a_tr);
    	if(dot(axis_4, a_tl) > max_a){ max_a = dot(axis_4, a_tl); }
    	if(dot(axis_4, a_bl) > max_a){ max_a = dot(axis_4, a_bl); }
    	if(dot(axis_4, a_br) > max_a){ max_a = dot(axis_4, a_br); }

    	double min_b = dot(axis_4, b_tr);
    	if(dot(axis_4, b_tl) < min_b){ min_b = dot(axis_4, b_tl); }
    	if(dot(axis_4, b_bl) < min_b){ min_b = dot(axis_4, b_bl); }
    	if(dot(axis_4, b_br) < min_b){ min_b = dot(axis_4, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_4 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_4, b_tr);
    	if(dot(axis_4, b_tl) > max_b){ max_b = dot(axis_4, b_tl); }
    	if(dot(axis_4, b_bl) > max_b){ max_b = dot(axis_4, b_bl); }
    	if(dot(axis_4, b_br) > max_b){ max_b = dot(axis_4, b_br); }

    	double min_a = dot(axis_4, a_tr);
    	if(dot(axis_4, a_tl) < min_a){ min_a = dot(axis_4, a_tl); }
    	if(dot(axis_4, a_bl) < min_a){ min_a = dot(axis_4, a_bl); }
    	if(dot(axis_4, a_br) < min_a){ min_a = dot(axis_4, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_4 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }

    double lowest_overlap = returnLower(returnLower(returnLower(overlap_1, overlap_2),overlap_3),overlap_4);
    Vector2f b_displacement;
    Vector2f a_displacement;
    if(lowest_overlap == overlap_1){

    	if(a_before_b_1){

    		b_displacement = axis_1*(overlap_1/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_1*(overlap_1/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }
    else if(lowest_overlap == overlap_2){

    	if(a_before_b_2){

    		b_displacement = axis_2*(overlap_2/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_2*(overlap_2/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }
    else if(lowest_overlap == overlap_3){

    	if(a_before_b_3){

    		b_displacement = axis_3*(overlap_3/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_3*(overlap_3/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }
    else if(lowest_overlap == overlap_4){

    	if(a_before_b_4){

    		b_displacement = axis_4*(overlap_4/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_4*(overlap_4/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }

    a.move(a_displacement.x,a_displacement.y);
    b.move(b_displacement.x,b_displacement.y);
}

void untangleSprite(Sprite &sprite, FloatRect rect){

	//determine how much the sprites are overlapping on each of the 4 axises defined by their rotations
	//move the sprites apart along the axis with the least

    FloatRect rect_a = sprite.getLocalBounds();
    FloatRect rect_b = rect;
	
	Vector2f a_tr = Vector2f(rect_a.width/2,-rect_a.height/2);
    Vector2f a_tl = Vector2f(-rect_a.width/2,-rect_a.height/2);
    Vector2f a_br = Vector2f(rect_a.width/2,rect_a.height/2);
    Vector2f a_bl = Vector2f(-rect_a.width/2,rect_a.height/2);

    Vector2f a_ctr = Vector2f(sprite.getPosition().x, sprite.getPosition().y);

    Vector2f b_tr = Vector2f(rect_b.width/2,-rect_b.height/2);
    Vector2f b_tl = Vector2f(-rect_b.width/2,-rect_b.height/2);
    Vector2f b_br = Vector2f(rect_b.width/2,rect_b.height/2);
    Vector2f b_bl = Vector2f(-rect_b.width/2,rect_b.height/2);

    Vector2f b_ctr = Vector2f(rect.left + (rect.width/2.0), rect.top + (rect.height/2.0));

    Vector2f axis_1 = normalize(a_bl - a_tl);
    Vector2f axis_2 = normalize(a_tr - a_tl);
    Vector2f axis_3 = normalize(b_bl - b_tl);
    Vector2f axis_4 = normalize(b_tr - b_tl);

    double overlap_1 = 0;
	double overlap_2 = 0;
	double overlap_3 = 0;
	double overlap_4 = 0;
	bool a_before_b_1 = dot(axis_1, a_ctr) < dot(axis_1, b_ctr);
	bool a_before_b_2 = dot(axis_2, a_ctr) < dot(axis_2, b_ctr);
	bool a_before_b_3 = dot(axis_3, a_ctr) < dot(axis_3, b_ctr);
	bool a_before_b_4 = dot(axis_4, a_ctr) < dot(axis_4, b_ctr);

	a_tr = a_ctr + rotateAboutOrigin(a_tr,sprite.getRotation());
	a_tl = a_ctr + rotateAboutOrigin(a_tl,sprite.getRotation());
	a_br = a_ctr + rotateAboutOrigin(a_br,sprite.getRotation());
	a_bl = a_ctr + rotateAboutOrigin(a_bl,sprite.getRotation());

	b_tr = b_ctr + b_tr;
	b_tl = b_ctr + b_tl;
	b_br = b_ctr + b_br;
	b_bl = b_ctr + b_bl;

    if(a_before_b_1){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_1, a_tr);
    	if(dot(axis_1, a_tl) > max_a){ max_a = dot(axis_1, a_tl); }
    	if(dot(axis_1, a_bl) > max_a){ max_a = dot(axis_1, a_bl); }
    	if(dot(axis_1, a_br) > max_a){ max_a = dot(axis_1, a_br); }

    	double min_b = dot(axis_1, b_tr);
    	if(dot(axis_1, b_tl) < min_b){ min_b = dot(axis_1, b_tl); }
    	if(dot(axis_1, b_bl) < min_b){ min_b = dot(axis_1, b_bl); }
    	if(dot(axis_1, b_br) < min_b){ min_b = dot(axis_1, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_1 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_1, b_tr);
    	if(dot(axis_1, b_tl) > max_b){ max_b = dot(axis_1, b_tl); }
    	if(dot(axis_1, b_bl) > max_b){ max_b = dot(axis_1, b_bl); }
    	if(dot(axis_1, b_br) > max_b){ max_b = dot(axis_1, b_br); }

    	double min_a = dot(axis_1, a_tr);
    	if(dot(axis_1, a_tl) < min_a){ min_a = dot(axis_1, a_tl); }
    	if(dot(axis_1, a_bl) < min_a){ min_a = dot(axis_1, a_bl); }
    	if(dot(axis_1, a_br) < min_a){ min_a = dot(axis_1, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_1 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }
    if(a_before_b_2){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_2, a_tr);
    	if(dot(axis_2, a_tl) > max_a){ max_a = dot(axis_2, a_tl); }
    	if(dot(axis_2, a_bl) > max_a){ max_a = dot(axis_2, a_bl); }
    	if(dot(axis_2, a_br) > max_a){ max_a = dot(axis_2, a_br); }

    	double min_b = dot(axis_2, b_tr);
    	if(dot(axis_2, b_tl) < min_b){ min_b = dot(axis_2, b_tl); }
    	if(dot(axis_2, b_bl) < min_b){ min_b = dot(axis_2, b_bl); }
    	if(dot(axis_2, b_br) < min_b){ min_b = dot(axis_2, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_2 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_2, b_tr);
    	if(dot(axis_2, b_tl) > max_b){ max_b = dot(axis_2, b_tl); }
    	if(dot(axis_2, b_bl) > max_b){ max_b = dot(axis_2, b_bl); }
    	if(dot(axis_2, b_br) > max_b){ max_b = dot(axis_2, b_br); }

    	double min_a = dot(axis_2, a_tr);
    	if(dot(axis_2, a_tl) < min_a){ min_a = dot(axis_2, a_tl); }
    	if(dot(axis_2, a_bl) < min_a){ min_a = dot(axis_2, a_bl); }
    	if(dot(axis_2, a_br) < min_a){ min_a = dot(axis_2, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_2 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }
    if(a_before_b_3){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_3, a_tr);
    	if(dot(axis_3, a_tl) > max_a){ max_a = dot(axis_3, a_tl); }
    	if(dot(axis_3, a_bl) > max_a){ max_a = dot(axis_3, a_bl); }
    	if(dot(axis_3, a_br) > max_a){ max_a = dot(axis_3, a_br); }

    	double min_b = dot(axis_3, b_tr);
    	if(dot(axis_3, b_tl) < min_b){ min_b = dot(axis_3, b_tl); }
    	if(dot(axis_3, b_bl) < min_b){ min_b = dot(axis_3, b_bl); }
    	if(dot(axis_3, b_br) < min_b){ min_b = dot(axis_3, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_3 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_3, b_tr);
    	if(dot(axis_3, b_tl) > max_b){ max_b = dot(axis_3, b_tl); }
    	if(dot(axis_3, b_bl) > max_b){ max_b = dot(axis_3, b_bl); }
    	if(dot(axis_3, b_br) > max_b){ max_b = dot(axis_3, b_br); }

    	double min_a = dot(axis_3, a_tr);
    	if(dot(axis_3, a_tl) < min_a){ min_a = dot(axis_3, a_tl); }
    	if(dot(axis_3, a_bl) < min_a){ min_a = dot(axis_3, a_bl); }
    	if(dot(axis_3, a_br) < min_a){ min_a = dot(axis_3, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_3 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }
    if(a_before_b_4){
    	//if a is before b when projected on the axis, then get the max corner of a and the min corner of b
    	double max_a = dot(axis_4, a_tr);
    	if(dot(axis_4, a_tl) > max_a){ max_a = dot(axis_4, a_tl); }
    	if(dot(axis_4, a_bl) > max_a){ max_a = dot(axis_4, a_bl); }
    	if(dot(axis_4, a_br) > max_a){ max_a = dot(axis_4, a_br); }

    	double min_b = dot(axis_4, b_tr);
    	if(dot(axis_4, b_tl) < min_b){ min_b = dot(axis_4, b_tl); }
    	if(dot(axis_4, b_bl) < min_b){ min_b = dot(axis_4, b_bl); }
    	if(dot(axis_4, b_br) < min_b){ min_b = dot(axis_4, b_br); }

    	//since they're overlapping, if the overlap is on this axis min b should come before max a
    	overlap_4 = max_a - min_b; //negative value means no overlap
    	//since a is before b, b will need to move positively along this axis while a moves negatively
    }
    else{
    	//if b is before a when projected on the axis, then get the max corner of b and the min corner of a
    	double max_b = dot(axis_4, b_tr);
    	if(dot(axis_4, b_tl) > max_b){ max_b = dot(axis_4, b_tl); }
    	if(dot(axis_4, b_bl) > max_b){ max_b = dot(axis_4, b_bl); }
    	if(dot(axis_4, b_br) > max_b){ max_b = dot(axis_4, b_br); }

    	double min_a = dot(axis_4, a_tr);
    	if(dot(axis_4, a_tl) < min_a){ min_a = dot(axis_4, a_tl); }
    	if(dot(axis_4, a_bl) < min_a){ min_a = dot(axis_4, a_bl); }
    	if(dot(axis_4, a_br) < min_a){ min_a = dot(axis_4, a_br); }

    	//since they're overlapping, if the overlap is on this axis min a should come before max b
    	overlap_4 = max_b - min_a; //negative value means no overlap
    	//since b is before a, a will need to move positively along this axis while b moves negatively
    }

    double lowest_overlap = returnLower(returnLower(returnLower(overlap_1, overlap_2),overlap_3),overlap_4);
    Vector2f b_displacement;
    Vector2f a_displacement;
    if(lowest_overlap == overlap_1){

    	if(a_before_b_1){

    		b_displacement = axis_1*(overlap_1/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_1*(overlap_1/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }
    else if(lowest_overlap == overlap_2){

    	if(a_before_b_2){

    		b_displacement = axis_2*(overlap_2/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_2*(overlap_2/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }
    else if(lowest_overlap == overlap_3){

    	if(a_before_b_3){

    		b_displacement = axis_3*(overlap_3/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_3*(overlap_3/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }
    else if(lowest_overlap == overlap_4){

    	if(a_before_b_4){

    		b_displacement = axis_4*(overlap_4/2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    	else{

    		b_displacement = axis_4*(overlap_4/-2.0);
    		a_displacement = b_displacement*(-1.0);
    	}
    }

    sprite.move(a_displacement.x*2.0,a_displacement.y*2.0);

}

void keepSpriteOutOfTerrain(Sprite &sprite, Terrain &terrain){

	FloatRect rect = sprite.getLocalBounds();
	vector<Vector2f> checkpoints;

	//subdivide the texture rect into enough sample points to get an accurate check against this grid resolution
	double terrain_grid_size = terrain.tile_size;
	int checkpoints_across = ceil(rect.width/terrain.tile_size);
	int checkpoints_down = ceil(rect.height/terrain.tile_size);

	double cell_size_across = rect.width/checkpoints_across;
	double cell_size_down = rect.height/checkpoints_down;

	checkpoints.push_back(Vector2f(-rect.width/2,-rect.height/2));
	for(int x = 0; x <= checkpoints_across; x++){
		for(int y = 0; y <= checkpoints_down; y++){
			if(x == 0 && y == 0){ continue;}

			checkpoints.push_back(checkpoints[0] + Vector2f(x*cell_size_across,y*cell_size_down));
		}
	}
	//

	//apply sprite's rotation and global position, then find relative position to grid_ref, then get a representative terrain square if intersecting immovable terrain
	FloatRect tile_rect;
	tile_rect.width = terrain.tile_size;
	tile_rect.height = terrain.tile_size;

	for(int index = 0; index < checkpoints.size(); index++){

		checkpoints[index] = sprite.getPosition() + rotateAboutOrigin(checkpoints[index], sprite.getRotation()) - terrain.grid_ref;

		int terrain_index_x = floor(checkpoints[index].x/terrain.tile_size);
		int terrain_index_y = floor(checkpoints[index].y/-terrain.tile_size);
		
		if(terrain_index_y > terrain.max_y){ continue; } // won't be a collision possible if this point is not in the terrain layer

        int faked_terrain_index_x = terrain_index_x;
        if(terrain.looping && faked_terrain_index_x >= terrain.max_x){ faked_terrain_index_x -= terrain.max_x; }
        if(terrain.looping && faked_terrain_index_x < 0){ faked_terrain_index_x += terrain.max_x; }

		if(collidable_terrain_types.count(terrain.grid[faked_terrain_index_x][terrain_index_y])==0){ continue; } //if this terrain block is not collidable don't continue

		tile_rect.left = (terrain_index_x*terrain.tile_size) + terrain.grid_ref.x;
		tile_rect.top = (terrain_index_y*-terrain.tile_size) - terrain.tile_size + terrain.grid_ref.y;
		
		untangleSprite(sprite, tile_rect);
	}

	//
}
    
void keepShipOutOfTerrain(Ship &ship, Terrain &terrain, double dt){

    //this function is an ugly composite of keepSpriteOutOfTerrain and untangleSprite. Definitely a candidate for future optimization.

    for(map<string,Sprite>::iterator k = ship.sprite.begin(); k != ship.sprite.end(); k++){ //check each sprite that composes the ship against the terrain

        Sprite &sprite = k->second;
        FloatRect rect = sprite.getLocalBounds();
        vector<Vector2f> checkpoints;

        //subdivide the texture rect into enough sample points to get an accurate check against this grid resolution
        double terrain_grid_size = terrain.tile_size;
        int checkpoints_across = ceil(rect.width/terrain.tile_size);
        int checkpoints_down = ceil(rect.height/terrain.tile_size);

        double cell_size_across = rect.width/checkpoints_across;
        double cell_size_down = rect.height/checkpoints_down;

        checkpoints.push_back(Vector2f(-rect.width/2,-rect.height/2));
        for(int x = 0; x <= checkpoints_across; x++){
            for(int y = 0; y <= checkpoints_down; y++){
                if(x == 0 && y == 0){ continue;}

                checkpoints.push_back(checkpoints[0] + Vector2f(x*cell_size_across,y*cell_size_down));
            }
        }
        //

        //apply sprite's rotation and global position, then find relative position to grid_ref, then get a representative terrain square if intersecting immovable terrain
        FloatRect tile_rect;
        tile_rect.width = terrain.tile_size;
        tile_rect.height = terrain.tile_size;

        for(int index = 0; index < checkpoints.size(); index++){

            checkpoints[index] = sprite.getPosition() + rotateAboutOrigin(checkpoints[index], sprite.getRotation()) - terrain.grid_ref;

            int terrain_index_x = floor(checkpoints[index].x/terrain.tile_size);
            int terrain_index_y = floor(checkpoints[index].y/-terrain.tile_size);
            
            if(terrain_index_y > terrain.max_y){ continue; } // won't be a collision possible if this point is not in the terrain layer

            int faked_terrain_index_x = terrain_index_x;
            if(terrain.looping && faked_terrain_index_x >= terrain.max_x){ faked_terrain_index_x -= terrain.max_x; }
            if(terrain.looping && faked_terrain_index_x < 0){ faked_terrain_index_x += terrain.max_x; }

            if(collidable_terrain_types.count(terrain.grid[faked_terrain_index_x][terrain_index_y])==0){ continue; } //if this terrain block is not collidable don't continue

            tile_rect.left = (terrain_index_x*terrain.tile_size) + terrain.grid_ref.x;
            tile_rect.top = (terrain_index_y*-terrain.tile_size) - terrain.tile_size + terrain.grid_ref.y;

            FloatRect rect_a = sprite.getLocalBounds();
            FloatRect rect_b = tile_rect;
            
            Vector2f a_tr = Vector2f(rect_a.width/2,-rect_a.height/2);
            Vector2f a_tl = Vector2f(-rect_a.width/2,-rect_a.height/2);
            Vector2f a_br = Vector2f(rect_a.width/2,rect_a.height/2);
            Vector2f a_bl = Vector2f(-rect_a.width/2,rect_a.height/2);

            Vector2f a_ctr = Vector2f(sprite.getPosition().x, sprite.getPosition().y);

            Vector2f b_tr = Vector2f(rect_b.width/2,-rect_b.height/2);
            Vector2f b_tl = Vector2f(-rect_b.width/2,-rect_b.height/2);
            Vector2f b_br = Vector2f(rect_b.width/2,rect_b.height/2);
            Vector2f b_bl = Vector2f(-rect_b.width/2,rect_b.height/2);

            Vector2f b_ctr = Vector2f(tile_rect.left + (tile_rect.width/2.0), tile_rect.top + (tile_rect.height/2.0));

            Vector2f axis_1 = normalize(a_bl - a_tl);
            Vector2f axis_2 = normalize(a_tr - a_tl);
            Vector2f axis_3 = normalize(b_bl - b_tl);
            Vector2f axis_4 = normalize(b_tr - b_tl);

            double overlap_1 = 0;
            double overlap_2 = 0;
            double overlap_3 = 0;
            double overlap_4 = 0;
            bool a_before_b_1 = dot(axis_1, a_ctr) < dot(axis_1, b_ctr);
            bool a_before_b_2 = dot(axis_2, a_ctr) < dot(axis_2, b_ctr);
            bool a_before_b_3 = dot(axis_3, a_ctr) < dot(axis_3, b_ctr);
            bool a_before_b_4 = dot(axis_4, a_ctr) < dot(axis_4, b_ctr);

            a_tr = a_ctr + rotateAboutOrigin(a_tr,sprite.getRotation());
            a_tl = a_ctr + rotateAboutOrigin(a_tl,sprite.getRotation());
            a_br = a_ctr + rotateAboutOrigin(a_br,sprite.getRotation());
            a_bl = a_ctr + rotateAboutOrigin(a_bl,sprite.getRotation());

            b_tr = b_ctr + b_tr;
            b_tl = b_ctr + b_tl;
            b_br = b_ctr + b_br;
            b_bl = b_ctr + b_bl;

            if(a_before_b_1){
                //if a is before b when projected on the axis, then get the max corner of a and the min corner of b
                double max_a = dot(axis_1, a_tr);
                if(dot(axis_1, a_tl) > max_a){ max_a = dot(axis_1, a_tl); }
                if(dot(axis_1, a_bl) > max_a){ max_a = dot(axis_1, a_bl); }
                if(dot(axis_1, a_br) > max_a){ max_a = dot(axis_1, a_br); }

                double min_b = dot(axis_1, b_tr);
                if(dot(axis_1, b_tl) < min_b){ min_b = dot(axis_1, b_tl); }
                if(dot(axis_1, b_bl) < min_b){ min_b = dot(axis_1, b_bl); }
                if(dot(axis_1, b_br) < min_b){ min_b = dot(axis_1, b_br); }

                //since they're overlapping, if the overlap is on this axis min b should come before max a
                overlap_1 = max_a - min_b; //negative value means no overlap
                //since a is before b, b will need to move positively along this axis while a moves negatively
            }
            else{
                //if b is before a when projected on the axis, then get the max corner of b and the min corner of a
                double max_b = dot(axis_1, b_tr);
                if(dot(axis_1, b_tl) > max_b){ max_b = dot(axis_1, b_tl); }
                if(dot(axis_1, b_bl) > max_b){ max_b = dot(axis_1, b_bl); }
                if(dot(axis_1, b_br) > max_b){ max_b = dot(axis_1, b_br); }

                double min_a = dot(axis_1, a_tr);
                if(dot(axis_1, a_tl) < min_a){ min_a = dot(axis_1, a_tl); }
                if(dot(axis_1, a_bl) < min_a){ min_a = dot(axis_1, a_bl); }
                if(dot(axis_1, a_br) < min_a){ min_a = dot(axis_1, a_br); }

                //since they're overlapping, if the overlap is on this axis min a should come before max b
                overlap_1 = max_b - min_a; //negative value means no overlap
                //since b is before a, a will need to move positively along this axis while b moves negatively
            }
            if(a_before_b_2){
                //if a is before b when projected on the axis, then get the max corner of a and the min corner of b
                double max_a = dot(axis_2, a_tr);
                if(dot(axis_2, a_tl) > max_a){ max_a = dot(axis_2, a_tl); }
                if(dot(axis_2, a_bl) > max_a){ max_a = dot(axis_2, a_bl); }
                if(dot(axis_2, a_br) > max_a){ max_a = dot(axis_2, a_br); }

                double min_b = dot(axis_2, b_tr);
                if(dot(axis_2, b_tl) < min_b){ min_b = dot(axis_2, b_tl); }
                if(dot(axis_2, b_bl) < min_b){ min_b = dot(axis_2, b_bl); }
                if(dot(axis_2, b_br) < min_b){ min_b = dot(axis_2, b_br); }

                //since they're overlapping, if the overlap is on this axis min b should come before max a
                overlap_2 = max_a - min_b; //negative value means no overlap
                //since a is before b, b will need to move positively along this axis while a moves negatively
            }
            else{
                //if b is before a when projected on the axis, then get the max corner of b and the min corner of a
                double max_b = dot(axis_2, b_tr);
                if(dot(axis_2, b_tl) > max_b){ max_b = dot(axis_2, b_tl); }
                if(dot(axis_2, b_bl) > max_b){ max_b = dot(axis_2, b_bl); }
                if(dot(axis_2, b_br) > max_b){ max_b = dot(axis_2, b_br); }

                double min_a = dot(axis_2, a_tr);
                if(dot(axis_2, a_tl) < min_a){ min_a = dot(axis_2, a_tl); }
                if(dot(axis_2, a_bl) < min_a){ min_a = dot(axis_2, a_bl); }
                if(dot(axis_2, a_br) < min_a){ min_a = dot(axis_2, a_br); }

                //since they're overlapping, if the overlap is on this axis min a should come before max b
                overlap_2 = max_b - min_a; //negative value means no overlap
                //since b is before a, a will need to move positively along this axis while b moves negatively
            }
            if(a_before_b_3){
                //if a is before b when projected on the axis, then get the max corner of a and the min corner of b
                double max_a = dot(axis_3, a_tr);
                if(dot(axis_3, a_tl) > max_a){ max_a = dot(axis_3, a_tl); }
                if(dot(axis_3, a_bl) > max_a){ max_a = dot(axis_3, a_bl); }
                if(dot(axis_3, a_br) > max_a){ max_a = dot(axis_3, a_br); }

                double min_b = dot(axis_3, b_tr);
                if(dot(axis_3, b_tl) < min_b){ min_b = dot(axis_3, b_tl); }
                if(dot(axis_3, b_bl) < min_b){ min_b = dot(axis_3, b_bl); }
                if(dot(axis_3, b_br) < min_b){ min_b = dot(axis_3, b_br); }

                //since they're overlapping, if the overlap is on this axis min b should come before max a
                overlap_3 = max_a - min_b; //negative value means no overlap
                //since a is before b, b will need to move positively along this axis while a moves negatively
            }
            else{
                //if b is before a when projected on the axis, then get the max corner of b and the min corner of a
                double max_b = dot(axis_3, b_tr);
                if(dot(axis_3, b_tl) > max_b){ max_b = dot(axis_3, b_tl); }
                if(dot(axis_3, b_bl) > max_b){ max_b = dot(axis_3, b_bl); }
                if(dot(axis_3, b_br) > max_b){ max_b = dot(axis_3, b_br); }

                double min_a = dot(axis_3, a_tr);
                if(dot(axis_3, a_tl) < min_a){ min_a = dot(axis_3, a_tl); }
                if(dot(axis_3, a_bl) < min_a){ min_a = dot(axis_3, a_bl); }
                if(dot(axis_3, a_br) < min_a){ min_a = dot(axis_3, a_br); }

                //since they're overlapping, if the overlap is on this axis min a should come before max b
                overlap_3 = max_b - min_a; //negative value means no overlap
                //since b is before a, a will need to move positively along this axis while b moves negatively
            }
            if(a_before_b_4){
                //if a is before b when projected on the axis, then get the max corner of a and the min corner of b
                double max_a = dot(axis_4, a_tr);
                if(dot(axis_4, a_tl) > max_a){ max_a = dot(axis_4, a_tl); }
                if(dot(axis_4, a_bl) > max_a){ max_a = dot(axis_4, a_bl); }
                if(dot(axis_4, a_br) > max_a){ max_a = dot(axis_4, a_br); }

                double min_b = dot(axis_4, b_tr);
                if(dot(axis_4, b_tl) < min_b){ min_b = dot(axis_4, b_tl); }
                if(dot(axis_4, b_bl) < min_b){ min_b = dot(axis_4, b_bl); }
                if(dot(axis_4, b_br) < min_b){ min_b = dot(axis_4, b_br); }

                //since they're overlapping, if the overlap is on this axis min b should come before max a
                overlap_4 = max_a - min_b; //negative value means no overlap
                //since a is before b, b will need to move positively along this axis while a moves negatively
            }
            else{
                //if b is before a when projected on the axis, then get the max corner of b and the min corner of a
                double max_b = dot(axis_4, b_tr);
                if(dot(axis_4, b_tl) > max_b){ max_b = dot(axis_4, b_tl); }
                if(dot(axis_4, b_bl) > max_b){ max_b = dot(axis_4, b_bl); }
                if(dot(axis_4, b_br) > max_b){ max_b = dot(axis_4, b_br); }

                double min_a = dot(axis_4, a_tr);
                if(dot(axis_4, a_tl) < min_a){ min_a = dot(axis_4, a_tl); }
                if(dot(axis_4, a_bl) < min_a){ min_a = dot(axis_4, a_bl); }
                if(dot(axis_4, a_br) < min_a){ min_a = dot(axis_4, a_br); }

                //since they're overlapping, if the overlap is on this axis min a should come before max b
                overlap_4 = max_b - min_a; //negative value means no overlap
                //since b is before a, a will need to move positively along this axis while b moves negatively
            }

            double lowest_overlap = returnLower(returnLower(returnLower(overlap_1, overlap_2),overlap_3),overlap_4);

            Vector2f b_displacement;
            Vector2f a_displacement;
            if(lowest_overlap == overlap_1){

                if(a_before_b_1){

                    b_displacement = axis_1*(overlap_1/2.0);
                    a_displacement = b_displacement*(-1.0);
                }
                else{

                    b_displacement = axis_1*(overlap_1/-2.0);
                    a_displacement = b_displacement*(-1.0);
                }
            }
            else if(lowest_overlap == overlap_2){

                if(a_before_b_2){

                    b_displacement = axis_2*(overlap_2/2.0);
                    a_displacement = b_displacement*(-1.0);
                }
                else{

                    b_displacement = axis_2*(overlap_2/-2.0);
                    a_displacement = b_displacement*(-1.0);
                }
            }
            else if(lowest_overlap == overlap_3){

                if(a_before_b_3){

                    b_displacement = axis_3*(overlap_3/2.0);
                    a_displacement = b_displacement*(-1.0);
                }
                else{

                    b_displacement = axis_3*(overlap_3/-2.0);
                    a_displacement = b_displacement*(-1.0);
                }
            }
            else if(lowest_overlap == overlap_4){

                if(a_before_b_4){

                    b_displacement = axis_4*(overlap_4/2.0);
                    a_displacement = b_displacement*(-1.0);
                }
                else{

                    b_displacement = axis_4*(overlap_4/-2.0);
                    a_displacement = b_displacement*(-1.0);
                }
            }

            //sap momentum from the ship on collision with terrain
            ship.velocity.x = ship.velocity.x - (ship.velocity.x*8.0*dt); //get rid of x component of velocity at an instantaneous rate such that 1/8 a second of collision would reduce to 0
            ship.velocity.y = -ship.velocity.y*0.2; //"bounce" ship on the y-axis at 1/5th of the previous velocity
            //

            ship.position = ship.position + Vector2f(a_displacement.x*2.0,a_displacement.y*2.0); //correct ship position
            ship.updateSpritePositions(); //this function will loop over the rest of the sprites on the ship after this iteration completes, so we need to update all sprites now to avoid false-positives (if this displacement solved terrain collisions with other sprites too)
        }
    }
}
