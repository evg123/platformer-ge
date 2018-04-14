//
//  Created by Vande Griek, Eric on 3/1/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include <stdio.h>
#include "tile.h"

/**
 Create a new tile that corresponds to tile_num
 */
Tile::Tile(int tile_num) {
    this->tile_num = tile_num;
    // choose texture based on tile_num
    std::string tile_texture = TEXTURE_PREFIX + std::to_string(tile_num) + TEXTURE_SUFFIX;
    texture = ResourceManager::instance().getImageTexture(tile_texture);

    rect.setColliderSize(TILE_SIDE, TILE_SIDE);

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
void Tile::update(long delta, std::map<int, Drawable*> &objects) {
    // tiles don't do anything
}

/**
 Draw the tile onto the screen
 */
void Tile::render() {
    int screen_off_x, screen_off_y;
    std::tie(screen_off_x, screen_off_y) = Graphics::instance().getScreenOffsets();
    SDL_Rect rend_rect;
    rect.fillRenderRect(rend_rect, screen_off_x, screen_off_y);
    SDL_Renderer *renderer = Graphics::instance().getRenderer();
    SDL_RenderCopy(renderer, texture, NULL, &rend_rect);
}
