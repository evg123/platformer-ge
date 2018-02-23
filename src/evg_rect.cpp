//
//  Created by Vande Griek, Eric on 2/6/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include <iostream>
#include "../include/evg_rect.h"

/**
 Create a new EvgRect
 */
EvgRect::EvgRect(int x_pos, int y_pos, int width, int height) {
        w = width;
        h = height;
}

/**
 Set position to absolute values.
 Remember the previous position.
 */
void EvgRect::setPosition(int x_pos, int y_pos) {
    x = x_pos;
    y = y_pos;
}

/**
 Move based on offsets from the current position.
 Remember the previous position.
 */
void EvgRect::move(int x_offset, int y_offset) {
    x += x_offset;
    y += y_offset;
}
