//
//  tile.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 3/1/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include <stdio.h>
#include "tile.h"

Tile::Tile(int tile_num) {
    // choose texture based on tile_num
    std::string tile_texture = TEXTURE_PREFIX + std::to_string(tile_num) + TEXTURE_SUFFIX;
    texture = ResourceManager::instance().getImageTexture(tile_texture);

    rect.w = TILE_SIDE;
    rect.h = TILE_SIDE;

    // do things based on tile type
    if (tile_num == TileNum::DAMAGE) {
        damage = DAMAGE_TILE_DAMAGE;
        hit_back_when_hopped_on = true;
        bouncy = true;
    }
}

/**
 Update the object based on how much time has passed.
 delta is in ms.
 */
void Tile::update(int delta, std::vector<Drawable*> &objects) {
    // tiles don't do anything
}

void Tile::render() {
    int screen_off_x, screen_off_y;
    std::tie(screen_off_x, screen_off_y) = Graphics::instance().getScreenOffsets();
    SDL_Rect rend_rect = {rect.x - screen_off_x, rect.y - screen_off_y, rect.w, rect.h};
    SDL_Renderer *renderer = Graphics::instance().getRenderer();
    SDL_RenderCopy(renderer, texture, NULL, &rend_rect);
}
