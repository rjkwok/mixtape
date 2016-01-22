#ifndef REB_COLL
#define REB_COLL

#include "main.h"

using namespace std;
using namespace sf;

double distanceFromGround(FloatRect rect, Terrain &terrain);
bool isIntersectingTerrain(Sprite a, Terrain &terrain);
bool isIntersectingTerrain(Ship &a, Terrain &terrain);
bool isIntersecting(Sprite a, Sprite b, double tolerance = 0);
bool isIntersecting(Sprite a, FloatRect rect_b, double tolerance = 0);

void untangleSprite(Sprite &sprite, FloatRect rect);
void untangleSprites(Sprite &a, Sprite &b);
void keepSpriteOutOfTerrain(Sprite &sprite, Terrain &terrain);
void keepShipOutOfTerrain(Ship &ship, Terrain &terrain, double dt);

#endif