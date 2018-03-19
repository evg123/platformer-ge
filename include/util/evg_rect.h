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
 A slightly enhanced SDL_Rect with some helpful collision detection functions
 */
class EvgRect : public SDL_Rect {
private:
public:
    EvgRect();

    /** Get the left edge of the rect */
    int left() { return x; };
    /** Get the right edge of the rect */
    int right() { return x + w; };
    /** Get the top edge of the rect */
    int top() { return y; };
    /** Get the bottom edge of the rect */
    int bottom() { return y + h; };

    void setPosition(int x_pos, int y_pos);
    void move(int x_offset, int y_offset);
};

#endif /* evg_rect_h */
