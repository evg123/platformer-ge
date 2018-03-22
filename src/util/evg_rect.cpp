//
//  Created by Vande Griek, Eric on 2/6/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include <iostream>
#include "evg_rect.h"

/**
 Create a new EvgRect
 */
EvgRect::EvgRect()
: pad_top(0), pad_right(0), pad_bot(0), pad_left(0) {
}

/**
 Set position to absolute values.
 */
void EvgRect::setPosition(int x_pos, int y_pos) {
    collider.x = x_pos;
    collider.y = y_pos;
}

/**
 Move based on offsets from the current position.
 */
void EvgRect::move(int x_offset, int y_offset) {
    setPosition(collider.x + x_offset, collider.y + y_offset);
}

/**
 Set the padding around the collition rect to use when rendering
 */
void EvgRect::setRenderPadding(int top, int right, int bot, int left) {
    pad_top = top;
    pad_right = right;
    pad_bot = bot;
    pad_left = left;
}

/**
 Set the size of the collision rect
 */
void EvgRect::setColliderSize(int width, int height) {
    collider.w = width;
    collider.h = height;
}

/**
 Fill in the passed in rect based on how this EvgRect should be rendered
 */
void EvgRect::fillRenderRect(SDL_Rect &render_rect, int screen_off_x, int screen_off_y) {
    render_rect.x = collider.x - screen_off_x - pad_left;
    render_rect.y = collider.y - screen_off_y - pad_top;
    render_rect.w = collider.w + pad_left + pad_right;
    render_rect.h = collider.h + pad_top + pad_bot;
}
