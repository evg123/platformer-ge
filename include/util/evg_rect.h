//
//  evg_rect.h
//  enhanced sdl rect
//
//  Created by Vande Griek, Eric on 2/6/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef evg_rect_h
#define evg_rect_h

#include <memory>
#include "SDL2.framework/Headers/SDL.h"

/**
 Holds a collision rect.
 Also has padding for drawing the rect larger than its collision rect
 */
class EvgRect {
private:
    SDL_Rect collider;
    int pad_top;
    int pad_right;
    int pad_bot;
    int pad_left;
public:
    EvgRect();
    const SDL_Rect& getCollider() { return collider; }
    void fillRenderRect(SDL_Rect &render_rect, int screen_off_x, int screen_off_y);

    /** X position of collider */
    int xPos() { return collider.x; }
    /** Y position of collider */
    int yPos() { return collider.y; }
    /** Width of collider */
    int width() { return collider.w; }
    /** Height of collider */
    int height() { return collider.h; }
    /** Get the top edge of the rect */
    int top() { return collider.y; };
    /** Get the right edge of the rect */
    int right() { return collider.x + collider.w; };
    /** Get the bottom edge of the rect */
    int bottom() { return collider.y + collider.h; };
    /** Get the left edge of the rect */
    int left() { return collider.x; };

    void setPosition(int x_pos, int y_pos);
    void move(int x_offset, int y_offset);
    void setColliderSize(int width, int height);
    void setRenderPadding(int top, int right, int bot, int left);
};

#endif /* evg_rect_h */
