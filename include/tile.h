//
//  tile.h
//  platformer
//
//  Created by Vande Griek, Eric on 3/1/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef tile_h
#define tile_h

#ifdef __linux__
#include "../lib/linux/SDL2.framework/Headers/SDL.h"
#else // Mac
#include "../lib/osx/SDL2.framework/Headers/SDL.h"
#endif

#include <vector>
#include "drawable.h"
#include "graphics.h"

constexpr int TILE_SIDE = 32;

constexpr auto TEXTURE_PREFIX = "tiles/tile_";
constexpr auto TEXTURE_SUFFIX = ".png";

class Tile : public Drawable {
private:
    int tile_num;
public:
    Tile(int tile_num);
    void update(int delta, std::vector<Drawable*> &objects);
    void render();
};

#endif /* tile_h */
