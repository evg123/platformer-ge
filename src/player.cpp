//
//  player.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 3/1/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include <stdio.h>
#include "../include/player.h"

Player::Player() {
    texture = ResourceManager::instance().getImageTexture(PLAYER_SPRITE_SHEET);
    sprite.init(PLAYER_FRAME_CONFIG);
    rect.w = PLAYER_WIDTH;
    rect.h = PLAYER_HEIGHT;
    
    movement_accel = PLAYER_MOVE_ACCEL;
    top_speed = PLAYER_TOP_SPEED;
    
    max_air_jumps = PLAYER_AIR_JUMP_COUNT;
    jump_vel = PLAYER_JUMP_VELOCITY;
    jump_duration = PLAYER_JUMP_TIME_MS;
}

/**
 Set the position to an absolute value
 */
void Player::setPosition(int x_pos, int y_pos) {
    Drawable::setPosition(x_pos, y_pos);
    Graphics::instance().focusScreenOffsets(rect);
}

