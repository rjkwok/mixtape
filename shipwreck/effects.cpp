#include "main.h"

using namespace std;
using namespace sf;


extern World world;
extern RenderWindow window;
extern map<string, Font> fonts;

ArcShape::ArcShape(double arc_angle, double radius){

    double Q = arc_angle * (M_PI/180);
    double arc_length = Q*radius;

    int arc_points = 2+ceil(arc_length/10);

    shape.setPointCount(3+arc_points);

    shape.setPoint(0, Vector2f(0, 0));
    for(int i = 0; i < arc_points; i++){
        shape.setPoint(i+1, Vector2f(radius*sin(i*Q/arc_points),-radius*cos(i*Q/arc_points)));
    }

    shape.setOutlineThickness(0);
}

void ArcShape::setPosition(Vector2f position){

    shape.setPosition(position);
}
void ArcShape::setRotation(double rotation){

    shape.setRotation(rotation);
}
void ArcShape::setFillColor(Color color){

    shape.setFillColor(color);
}

void ArcShape::draw(){

    window.draw(shape);
}


Caption::Caption(){}
Caption::Caption(string contents, string font, Vector2f coords, int char_size, Color color, string bias){

    text.setString(contents);
    text.setFont(fonts[font]);
    text.setCharacterSize(char_size);

    FloatRect rect = text.getGlobalBounds();

    if(bias == "left"){
        text.setOrigin(0,rect.height/2);
    }
    else if(bias == "middle"){
        text.setOrigin(rect.width/2,rect.height/2);
    }
    else if(bias == "right"){
        text.setOrigin(rect.width,rect.height/2);
    }

    text.setColor(color);
    text.setScale(0.5,0.5);
    text.setPosition(coords);
}
void Caption::draw(){

    window.draw(text);
}

void castLight(string sector_id, Vector2f point, double diffuse, double intensity)
{
    set<string> nearby_ents = getSector(sector_id)->collision_grid.getNearby(point, (intensity/diffuse)*2);

    Sprite bloom;
    bloom.setTexture(getProperties("Light Bloom")->sheet, true);
    bloom.setOrigin(getProperties("Light Bloom")->get("width")/2,getProperties("Light Bloom")->get("height")/2);
    bloom.setColor(Color(255,255,255,intensity));
    bloom.setPosition(point);
    getSector(sector_id)->lights.push_back(bloom);

    set<string> connectors;
    for(set<string>::iterator parser = nearby_ents.begin(); parser!=nearby_ents.end(); parser++)
    {
        double ent_diffuse = diffuse*0.65;
        shared_ptr<Entity> candidate = getSector(sector_id)->getEnt(*parser);
        Vector2f dist = candidate->getPosition()-point;
        double h = hypot(dist.x,dist.y) - candidate->collision_radius;
        if(h < 0){ h = 0; }

        double light_amount = intensity-(ent_diffuse*h);
        if(light_amount<0){light_amount=0;}
        candidate->light(light_amount);

        connectors.insert(candidate->connectors.begin(), candidate->connectors.end());

        if(getSector(sector_id)->machines.count(*parser) != 0){
            shared_ptr<Machine> machine = getSector(sector_id)->machines[*parser].lock();

            for(map<string, shared_ptr<FlowPort> >::iterator port_parser = machine->ports.begin(); port_parser != machine->ports.end(); port_parser++){
                port_parser->second->light(light_amount);
            }
        }
    }
    for(set<string>::iterator parser = connectors.begin(); parser != connectors.end(); parser++){

        double ent_diffuse = diffuse*0.7;
        if(getSector(sector_id)->connectors.count(*parser) != 0){
            shared_ptr<Connector> candidate = getSector(sector_id)->connectors[*parser];
            if(candidate->has_sprite){
                Vector2f dist = candidate->sprite.getPosition()-point;
                double h = hypot(dist.x,dist.y) - candidate->collision_radius;
                if(h < 0){ h = 0; }
                double light_amount = intensity-(ent_diffuse*h);
                if(light_amount<0){light_amount=0;}
                candidate->light(light_amount);
            }

        }

    }
    nearby_ents = getSector(sector_id)->floor_grid.getNearby(point, (intensity/diffuse)*2);
    for(set<string>::iterator parser = nearby_ents.begin(); parser!=nearby_ents.end(); parser++)
    {
        shared_ptr<Floor> candidate = getSector(sector_id)->floors[*parser];

        vector<double>::iterator tile_light = candidate->tile_light_levels.begin();
        for(vector<Sprite>::iterator tile_parser = candidate->tiles.begin(); tile_parser != candidate->tiles.end(); tile_parser++){

            Vector2f dist = tile_parser->getPosition()-point;
            double h = hypot(dist.x,dist.y) - candidate->tile_radius;
            if(h < 0){ h = 0; }

            double light_amount = intensity-35-(diffuse*0.4*h);
            if(light_amount<0){light_amount=0;}
            *tile_light += light_amount;
            if(getSector(sector_id)->ambient_light + *tile_light > 255){ *tile_light = 255 - getSector(sector_id)->ambient_light; }

            tile_light++;
        }
    }

    for(map<string,shared_ptr<Poster> >::iterator parser = getSector(sector_id)->posters.begin(); parser != getSector(sector_id)->posters.end(); parser++){

        shared_ptr<Poster> candidate = parser->second;
        Vector2f dist = candidate->sprite.getPosition()-point;
        double h = hypot(dist.x,dist.y);

        double light_amount = intensity-(diffuse*h);
        if(light_amount<0){light_amount=0;}
        candidate->light(light_amount);
    }
    for(map<string,shared_ptr<Rope> >::iterator parser = getSector(sector_id)->ropes.begin(); parser != getSector(sector_id)->ropes.end(); parser++){
        for(int i = 0; i < parser->second->nodes.size(); i++){

            shared_ptr<RopeNode> candidate = parser->second->nodes[i];
            Vector2f dist = candidate->sprite.getPosition()-point;
            double h = hypot(dist.x,dist.y);

            double light_amount = intensity-35-(diffuse*0.4*h);
            if(light_amount<0){light_amount=0;}
            candidate->light(light_amount);
        }
    }
    for(map<string,shared_ptr<Interface> >::iterator parser = getSector(sector_id)->interfaces.begin(); parser!=getSector(sector_id)->interfaces.end(); parser++){

        double ent_diffuse = diffuse*0.65;
        for(int i = 0; i < parser->second->sprite.size(); i++){

            Vector2f dist = Vector2f(parser->second->sprite[i].getPosition().x,parser->second->sprite[i].getPosition().y)-point;
            double h = hypot(dist.x,dist.y);
            double light_amount = (intensity)-((ent_diffuse)*h);
            if(light_amount<0){light_amount=0;}
            if(parser->second->light_level.size() <= i){
                parser->second->light_level.push_back(light_amount);
            }
            else{
                parser->second->light_level[i] += light_amount;
            }

            if(getSector(sector_id)->ambient_light + 20 + parser->second->light_level[i] > 255){parser->second->light_level[i] = 255 - (20+getSector(sector_id)->ambient_light);}
        }




    }
}

RectangleShape createBoundingRectangle(Sprite sprite, Color color){

    RectangleShape new_rectangle(Vector2f(sprite.getTextureRect().width, sprite.getTextureRect().height));
    new_rectangle.setOrigin(sprite.getTextureRect().width/2, sprite.getTextureRect().height/2);
    new_rectangle.setRotation(sprite.getRotation());
    new_rectangle.setPosition(sprite.getPosition());
    new_rectangle.setFillColor(Color(0,0,0,0));
    new_rectangle.setOutlineColor(color);
    new_rectangle.setOutlineThickness(2.5);

    return new_rectangle;
}

CircleShape createBoundingCircle(Sprite sprite, Color color, double margin){

    double radius = (hypot(sprite.getLocalBounds().width, sprite.getLocalBounds().height)/2) + margin;
    CircleShape new_circle(radius);
    new_circle.setOrigin(radius, radius);
    new_circle.setPosition(sprite.getPosition());
    new_circle.setFillColor(Color(0,0,0,0));
    new_circle.setOutlineColor(color);
    new_circle.setOutlineThickness(2.5);

    return new_circle;
}

RectangleShape createRectangle(Vector2f new_position, Vector2f new_size, int outline_width, Color fill_color, Color outline_color){

    RectangleShape new_rectangle(new_size);
    new_rectangle.setOrigin(new_size.x/2.0, new_size.y/2.0);
    new_rectangle.setOutlineThickness(outline_width);
    new_rectangle.setPosition(new_position);
    new_rectangle.setFillColor(fill_color);
    new_rectangle.setOutlineColor(outline_color);


    return new_rectangle;
}

RectangleShape createLine(Vector2f start_position, Vector2f axis, double length, Color color){

    RectangleShape new_rectangle;
    new_rectangle.setSize(Vector2f(3, length));
    new_rectangle.setOrigin(1.5,0);
    new_rectangle.setPosition(start_position);
    new_rectangle.setOutlineThickness(0);
    new_rectangle.setFillColor(color);
    new_rectangle.setRotation(getRotationFromAxis(axis));

    return new_rectangle;
}

void createEffect(string sector_id, Vector2f start_pos, string type)
{
    if(sin(world.date.second*60) - 1 > -0.05){
        string newid = createUniqueId();
        Vector2f velocity = Vector2f(0,0);

        getSector(sector_id)->effects[newid] = make_shared<Effect>(sector_id, newid, start_pos, velocity, type);
    }
}

void createEffect(string sector_id, Vector2f start_pos, Vector2f velocity, string type)
{

    string newid = createUniqueId();
    getSector(sector_id)->effects[newid] = make_shared<Effect>(sector_id, newid, start_pos, velocity, type);

}

Effect::Effect(){}

Effect::Effect(string new_sector_id, string cid, Vector2f start_pos, string type)
{
    sector_id = new_sector_id;
    properties = getProperties(type);
    id = cid;
    sprite.setTexture(properties->sheet,false);
    IntRect dimset = IntRect(0,0,properties->get("width"),properties->get("height"));
    max_frame = (sprite.getTextureRect().width/dimset.width)-1;
    sprite.setTextureRect(dimset);
    sprite.setOrigin(dimset.height/2,dimset.width/2);
    sprite.setPosition(start_pos.x,start_pos.y);

    frame_window = dimset;

    center = start_pos;
}

Effect::Effect(string new_sector_id, string cid, Vector2f start_pos, Vector2f vel, string type)
{
    sector_id = new_sector_id;
    properties = getProperties(type);
    id = cid;
    sprite.setTexture(properties->sheet,false);
    IntRect dimset = IntRect(0,0,properties->get("width"),properties->get("height"));
    max_frame = (sprite.getTextureRect().width/dimset.width)-1;
    sprite.setTextureRect(dimset);
    sprite.setOrigin(dimset.height/2,dimset.width/2);
    sprite.setPosition(start_pos.x,start_pos.y);

    frame_window = dimset;

    center = start_pos;
    velocity = vel;
}

void Effect::run(double dt)
{
    //animate

    elapsed += dt;
    frame = floor(elapsed*FPS);

    center = center + (velocity*dt);
    sprite.setPosition(center.x,center.y);

    if(frame>max_frame)
    {
        getSector(sector_id)->trash(id);
    }
    else
    {
        frame_window.left = frame*frame_window.width;
        sprite.setTextureRect(frame_window);
    }
}

void Effect::draw()
{
    window.draw(sprite);
}
