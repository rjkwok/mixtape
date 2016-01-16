#include "main.h"

using namespace std;
using namespace sf;

extern map<string, Texture*> textures;
extern map<int, TileProperties> tile_properties;

TileProperties::TileProperties(){}

Terrain::Terrain(){}
Terrain::Terrain(Vector2f bottom_left_origin, double c_tile_size, int terrain_tiles_across, int terrain_tiles_up){

    grid_ref = bottom_left_origin;
    max_x = terrain_tiles_across;
    max_y = terrain_tiles_up;
    tile_size = c_tile_size;

    tiles = VertexArray(Quads,4);
    tiles.resize(max_x*max_y*4); //resize to buildable grid (needs to be equal to total area in tiles, not pixels, and then multiplied by 4 to have 4 vertexes per tile)
}

void Terrain::updateTiles(){

    //terrain update loop
    for(int index_x = 0; index_x < max_x; index_x++){
        for(int index_y = 0; index_y < max_y; index_y++){
            //iterate over every tile
            int tile_type_index = grid[index_x][index_y]; //used here to determine which part of the loaded texture sheet to show
            int texture_index = tile_properties[tile_type_index].texture_indexes[randInt(tile_properties[tile_type_index].texture_indexes.size())-1];
            int tiles_across = floor(textures["terrain"]->getSize().x/tile_size);
            int u = texture_index % tiles_across;
            int v = floor(texture_index/tiles_across);

            sf::Vertex* tile = &tiles[4*(index_x + (index_y*max_x))]; //get a pointer to a location in the array defined by this tile's sequential position iterated left-to-right and then bottom-to-top
            //take this and the next 3 array elements as the corners of the tile, texture and position them accordingly

            tile[0].position = sf::Vector2f(grid_ref.x+(index_x*tile_size), grid_ref.y-(index_y*tile_size));
            tile[1].position = sf::Vector2f(grid_ref.x+((index_x + 1)*tile_size), grid_ref.y-(index_y*tile_size));
            tile[2].position = sf::Vector2f(grid_ref.x+((index_x + 1)*tile_size), grid_ref.y-((index_y + 1)*tile_size));
            tile[3].position = sf::Vector2f(grid_ref.x+(index_x*tile_size), grid_ref.y-((index_y + 1)*tile_size));

            tile[3].texCoords = sf::Vector2f(u*tile_size, v*tile_size);
            tile[2].texCoords = sf::Vector2f((u + 1)*tile_size, v*tile_size);
            tile[1].texCoords = sf::Vector2f((u + 1)*tile_size, (v + 1)*tile_size);
            tile[0].texCoords = sf::Vector2f(u*tile_size, (v + 1)*tile_size);
        }
    }
    //
}

void Terrain::changeTile(int x, int y, int type_id){

    grid[x][y] = type_id;
    updateTiles();
}

void Terrain::draw(RenderWindow &window){

    window.draw(tiles, textures["terrain"]);
}