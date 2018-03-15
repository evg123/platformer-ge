//
//  being.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 3/5/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "being.h"

Being::Being() {}

bool Being::isOnGround() {
    //TODO could calculate this once on update
    unsigned int diff = SDL_GetTicks() - last_grounded;
    return diff <= JUMP_TOLERANCE_MS;
}

void Being::resetJumps() {
    air_jumps = max_air_jumps;
}

void Being::jump() {
    if (canJump()) {
        jump_start_ts = SDL_GetTicks();
        if (!isOnGround()) {
            --air_jumps;
        }
    }
}

bool Being::canJump() {
    return isOnGround() || air_jumps > 0;
}

void Being::update(int delta, std::vector<Drawable*> &objects) {
    Drawable::update(delta, objects);
    updateSprite();
}

void Being::render() {
    int screen_off_x, screen_off_y;
    std::tie(screen_off_x, screen_off_y) = Graphics::instance().getScreenOffsets();
    SDL_Rect rend_rect = {rect.x - screen_off_x, rect.y - screen_off_y, rect.w, rect.h};
    SDL_Renderer *renderer = Graphics::instance().getRenderer();
    SDL_RendererFlip flip_mode = facing == Facing::RIGHT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, texture, &sprite.getFrameRect(), &rend_rect, 0, NULL, flip_mode);
}

void Being::doMove(float x_offset, float y_offset, std::vector<Drawable*> &objects) {
    //TODO override no longer needed
    // call the superclass doMove
    Drawable::doMove(x_offset, y_offset, objects);
}

void Being::processCollision(Drawable &other, float x_off, float y_off) {
    // do the default
    Drawable::processCollision(other, x_off, y_off);

    if (y_off > 0) {
        // we have collided while moving down,
        // so we have landed on something
        last_grounded = SDL_GetTicks();
        jump_start_ts = 0; // zero means not jumping
        resetJumps();
    }
}

/**
 adjust velocity based on acceleration
 */
void Being::applyAcceleration(int delta) {
    // do default first
    Drawable::applyAcceleration(delta);
    
    // apply gravity
    y_vel += GRAVITY * delta;
    
    // limit max fall velocity
    if (y_vel > TERMINAL_VELOCITY) {
        y_vel = TERMINAL_VELOCITY;
    }

    // vertical movement / jump
    if (jump_start_ts != 0 && SDL_GetTicks() - jump_start_ts <= jump_duration) {
        y_vel = -jump_vel;
    }

    // horizonal movement
    if (target_x_vel > 0 && x_vel < top_speed) {
        // moving right
        //TODO maybe apply additional accel here if we are at negative velocity
        x_vel += movement_accel * delta;
        if (x_vel > top_speed) {
            x_vel = top_speed;
        }
    } else if (target_x_vel < 0 && x_vel > -top_speed) {
        // moving left
        x_vel -= movement_accel * delta;
        if (x_vel < -top_speed) {
            x_vel = -top_speed;
        }
    } else if (target_x_vel == 0.0f && isOnGround()) {
        // stopping
        // slow to zero unless we are in the air
        if (x_vel > 0) {
            //SDL_Log("slow");
            x_vel = std::max(0.0f, x_vel - FRICTION * delta);
        } else if (x_vel < 0) {
            x_vel = std::min(0.0f, x_vel + FRICTION * delta);
        }
    }
}

void Being::moveRight() {
    target_x_vel += top_speed;
    facing = Facing::RIGHT;
}

void Being::stopRight() {
    target_x_vel = std::max(0.0f, target_x_vel - top_speed);
}

void Being::moveLeft() {
    target_x_vel -= top_speed;
    facing = Facing::LEFT;
}

void Being::stopLeft() {
    target_x_vel = std::min(0.0f, target_x_vel + top_speed);
}

void Being::updateSprite() {
    //SDL_Log("%d", isOnGround());
    if (!isOnGround()) {
        sprite.setJumping();
    } else if (x_vel == 0.0f) {
        sprite.setIdle();
    } else if (target_x_vel != 0.0f) {
        sprite.setWalking();
    } else {
        sprite.setBraking();
    }
    
    sprite.update();
}
