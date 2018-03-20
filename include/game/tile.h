//
//  tile.h
//  platformer
//
//  Created by Vande Griek, Eric on 3/1/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef tile_h
#define tile_h

#include <vector>
#include "SDL2.framework/Headers/SDL.h"
#include "drawable.h"
#include "graphics.h"

constexpr int TILE_SIDE = 32;

constexpr auto TEXTURE_PREFIX = "tiles/tile_";
constexpr auto TEXTURE_SUFFIX = ".png";

constexpr int DAMAGE_TILE_DAMAGE = 1;

// these should be in a config file so they can be shared with the level creator
enum TileNum {
    EMPTY = 0,
    DIRT = 1,
    STEEL = 2,
    DAMAGE = 3,
    GOAL = 4,
    PLAYER = 5,
    RED_ENEMY = 6,
    BLUE_ENEMY = 7,
};

class Tile : public Drawable {
private:
    int tile_num;
public:
    Tile(int tile_num);
    void update(int delta, std::vector<Drawable*> &objects) override;
    void render() override;
};

#endif /* tile_h */
