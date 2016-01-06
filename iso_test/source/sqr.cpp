#include "main.h"

using namespace std;
using namespace sf;

extern RenderWindow window;
extern string view_direction;

SquareSprite::SquareSprite(){}
SquareSprite::SquareSprite(string c_type_id, double x, double y, double z){

	type_id = c_type_id;

	direction = view_direction;
	sprite.setTexture(*getTexture( type_id + direction ),false);
	sprite.setTextureRect(getTextureRect( type_id ));
	sprite.setOrigin(getTextureRect(type_id).width/2.0,getTextureRect(type_id).height-(sprite.getTextureRect().width/4.0));
	position = Vector3f(x,y,z);
	sprite.setPosition(transformTo2DView(position, direction));
	a = sprite.getTextureRect().width/4.0;
	side_length = sprite.getTextureRect().width/2.0;
	height = getIsoHeight(type_id) - (2*a);
	frame_index = 0;
	fps = 25;
	elapsed_time = 0;
	first_frame = 0;
	last_frame = 0;
}

int SquareSprite::faceIntersecting(Vector2f cursor){

	double x = cursor.x - sprite.getPosition().x;
	double y = cursor.y - sprite.getPosition().y;

	if(abs(x) < 0.5*getTextureRect(type_id).width && y + (0.5*abs(x)) < a && y + (0.5*abs(x)) > a-height){
		if( x > 0 ){//hitting the right face
			if(direction == "_ne"){ // S face
				return 3;
			}
			else if(direction == "_se"){ // W face
				return 4;
			}
			else if(direction == "_sw"){ // N face
				return 1;
			}
			else if(direction == "_nw"){ // E face
				return 2;
			}
		}
		if( x < 0 ){//hitting the left face
			if(direction == "_ne"){ // W face
				return 4;
			}
			else if(direction == "_se"){ // N face
				return 1;
			}
			else if(direction == "_sw"){ // E face
				return 2;
			}
			else if(direction == "_nw"){ // S face
				return 3;
			}
		}
	}
	else if(abs(x) < 0.5*getTextureRect(type_id).width && y + (0.5*abs(x)) < a-height && y - (0.5*abs(x)) > -getIsoHeight(type_id) + a){
		return 5; //top
	}

	return 0;

} 
bool SquareSprite::cheapIntersecting(Vector2f cursor){

	if(sprite.getGlobalBounds().contains(cursor.x,cursor.y)){
		return true;
	}
	return false;
}
bool SquareSprite::mapIntersecting(Vector2f cursor){

	Vector2f cursor_2d = transformToIsoView(cursor, direction);

	if(cursor_2d.x < position.x + (side_length/2.0) && cursor_2d.x > position.x + (side_length/2.0) && cursor_2d.y < position.y + (side_length/2.0) && cursor_2d.y > position.y + (side_length/2.0)){
		//is inside the defined region
		return true;
	}
	return false;
}

double SquareSprite::getScreenY(){

	return sprite.getPosition().y + position.z;
}

void SquareSprite::play(int c_first_frame, int c_last_frame){

	if(c_first_frame == first_frame && c_last_frame == last_frame){ return; }

	first_frame = c_first_frame;
	last_frame = c_last_frame;

	elapsed_time = 0;
	frame_index = first_frame;
}

void SquareSprite::update(double dt){

	if(direction != view_direction){

		//update rotation
		direction = view_direction;

		sprite.setTexture(*getTexture(type_id + direction),false);
		sprite.setTextureRect(getTextureRect(type_id));
		sprite.setOrigin(getTextureRect(type_id).width/2.0,getTextureRect(type_id).height-a);
		sprite.setPosition(transformTo2DView(position, direction));
	}

	sprite.setColor(Color(255,255,255,255));
	
	if(first_frame != last_frame){
		//update animation by altering the location of the texture rect
		double tiles_per_row = getTexture(type_id+direction)->getSize().x/getTextureRect(type_id).width;
		int rows = floor(getTexture(type_id+direction)->getSize().y/getTextureRect(type_id).height);

		int rows_down = floor(frame_index / tiles_per_row);
		if(rows_down >= rows){ 

			frame_index = 0; 
			rows_down = 0; 
		}
		int tiles_over = floor(frame_index - (rows_down*tiles_per_row));
		
		IntRect texture_rect = getTextureRect(type_id);
		texture_rect.left = tiles_over*getTextureRect(type_id).width;
		texture_rect.top = rows_down*getTextureRect(type_id).height;
		sprite.setTextureRect(texture_rect);

		elapsed_time += dt;
		if(elapsed_time >= (1.0/fps)){

			elapsed_time = 0;
			frame_index++; 
			if(frame_index > last_frame){
				frame_index = first_frame;
			}
		}
	}
	
}

void SquareSprite::draw(){

	window.draw(sprite);
}