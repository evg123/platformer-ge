//
//  being.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 3/5/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "being.h"

Being::Being() {
    y_acc = GRAVITY;
}

void Being::resetJumps() {
    jumps = max_jumps;
}

void Being::jump() {
    if (canJump()) {
        sprite.jump();
        y_vel = 1.0 * jump_power;
        --jumps;
    }
}

bool Being::canJump() {
    return jumps > 0;
}

void Being::update(int delta, std::vector<Drawable*> &objects) {
    Drawable::update(delta, objects);
    sprite.update();
}

void Being::render() {
    int screen_off_x, screen_off_y;
    std::tie(screen_off_x, screen_off_y) = Graphics::instance().getScreenOffsets();
    SDL_Rect rend_rect = {rect.x - screen_off_x, rect.y - screen_off_y, rect.w, rect.h};
    SDL_Renderer *renderer = Graphics::instance().getRenderer();
    SDL_RendererFlip flip_mode = facing == Facing::RIGHT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, texture, &sprite.getFrameRect(), &rend_rect, 0, NULL, flip_mode);
}

void Being::doMove(int x_offset, int y_offset, std::vector<Drawable*> &objects) {
    // reset our on_ground state.
    // it will be re-applied if we are on the ground by the superclass doMove.
    // this updates on_ground for jumps as well as walking off cliffs.
    on_ground = false;

    // call the superclass doMove
    Drawable::doMove(x_offset, y_offset, objects);
}

void Being::processCollision(Drawable &other, int x_off, int y_off) {
    // do the default
    Drawable::processCollision(other, x_off, y_off);

    if (y_off > 0) {
        // we have collided while moving down,
        // so we have landed on something
        on_ground = true;
        sprite.land();
        resetJumps();
    }
}

/**
 adjust velocity based on acceleration
 */
void Being::applyAcceleration(int delta) {
    Drawable::applyAcceleration(delta);
    
    // limit max fall velocity
    if (y_vel > TERMINAL_VELOCITY) {
        y_vel = TERMINAL_VELOCITY;
    }
}

void Being::moveRight() {
    x_vel += 1.0 * speed;
    facing = Facing::RIGHT;
    sprite.move();
}

void Being::stopRight() {
    x_vel = std::max(0.0, x_vel - 1.0 * speed);
    sprite.stop();
}

void Being::moveLeft() {
    x_vel -= 1.0 * speed;
    facing = Facing::LEFT;
    sprite.move();
}

void Being::stopLeft() {
    x_vel = std::min(0.0, x_vel + 1.0 * speed);
    sprite.stop();
}
