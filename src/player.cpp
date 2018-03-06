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
    rect.w = PLAYER_WIDTH;
    rect.h = PLAYER_HEIGHT;
    max_jumps = PLAYER_JUMP_COUNT;
    jump_power = PLAYER_JUMP_POWER;
}

/**
 Set the position to an absolute value
 */
void Player::setPosition(int x_pos, int y_pos) {
    Drawable::setPosition(x_pos, y_pos);
    Graphics::instance().focusScreenOffsets(rect);
}

void Player::render() {
    int screen_off_x, screen_off_y;
    std::tie(screen_off_x, screen_off_y) = Graphics::instance().getScreenOffsets();
    SDL_Rect rend_rect = {rect.x - screen_off_x, rect.y - screen_off_y, rect.w, rect.h};
    SDL_Renderer *renderer = Graphics::instance().getRenderer();
    SDL_RenderCopy(renderer, texture, NULL, &rend_rect);
}

void Player::moveRight() {
    x_vel += 1.0 * speed;
}

void Player::stopRight() {
    x_vel -= 1.0 * speed;
}

void Player::moveLeft() {
    x_vel -= 1.0 * speed;
}

void Player::stopLeft() {
    x_vel += 1.0 * speed;
}

