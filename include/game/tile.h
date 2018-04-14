//
//  tile.h
//  Tile that makes up the structure of a level
//
//  Created by Vande Griek, Eric on 3/1/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef tile_h
#define tile_h

#include <map>
#include "SDL.h"
#include "drawable.h"
#include "graphics.h"

constexpr int TILE_SIDE = 32;

constexpr auto TEXTURE_PREFIX = "tiles/tile_";
constexpr auto TEXTURE_SUFFIX = ".png";

constexpr int DAMAGE_TILE_DAMAGE = 1;

/**
 Class representing one tile in the grid of tiles that makes up a level
 */
class Tile : public Drawable {
private:
public:
    Tile(int tile_num);
    void update(long delta, std::map<int, Drawable*> &objects) override;
    void render() override;
};

#endif /* tile_h */
