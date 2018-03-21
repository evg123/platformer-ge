//
//  being.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 3/5/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "being.h"

Being::Being() {}

void Being::init(BeingType type) {
    marked_for_removal = false;
    x_vel = 0;
    y_vel = 0;
    x_accel = 0;
    y_accel = 0;
    
    // unpack type class
    this->type = type;
    rect.setColliderSize(type.width, type.height);
    rect.setRenderPadding(type.pad_top, type.pad_right, type.pad_bot, type.pad_left);
    texture = ResourceManager::instance().getImageTexture(type.sprite_sheet);
    sprite.init(type.frame_config);
    hp = type.hp;
    damage = type.damage;
    bump_immune = type.bump_immune;
    bouncy = type.bouncy;
    hit_back_when_hopped_on = type.hit_back_when_hopped_on;
    score_on_destruction = type.score_on_destruction;
    top_speed = type.top_speed;
    jump_duration = type.jump_duration;
    max_air_jumps = type.max_air_jumps;
}

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
        Audio::instance().playSound(type.jump_sound);
    }
}

bool Being::canJump() {
    return isOnGround() || air_jumps > 0;
}

bool Being::dead() {
    return hp <= 0;
}

void Being::update(int delta, std::vector<Drawable*> &objects) {
    if (!dead()) {
        performAction(delta);
        Drawable::update(delta, objects);
    }

    if (dead()) {
        int now = SDL_GetTicks();
        if (destroy_at_ts == 0) {
            destroy_at_ts = now + BEING_DEATH_DELAY_MS;
        } else if (now >= destroy_at_ts) {
            destroy();
        }
    }

    updateSprite();
}

void Being::render() {
    int screen_off_x, screen_off_y;
    std::tie(screen_off_x, screen_off_y) = Graphics::instance().getScreenOffsets();
    SDL_Rect rend_rect;
    rect.fillRenderRect(rend_rect, screen_off_x, screen_off_y);
    SDL_Renderer *renderer = Graphics::instance().getRenderer();
    SDL_RendererFlip flip_mode = facing == Facing::RIGHT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, texture, &sprite.getFrameRect(), &rend_rect, 0, NULL, flip_mode);
}

/**
 Update the being based on its defined action type
 */
void Being::performAction(int delta) {
    int now = SDL_GetTicks();
    int action_len = now - action_start_ts;
    if (type.action_type == ActionType::CHARGE) {
        // run in a direction for 1 second
        if (action_len > 1000) {
            int dir = rand() % 3;
            switch(dir) {
                case 0:
                    // stand still
                    target_x_vel = 0;
                    break;
                case 1:
                    // run right
                    target_x_vel = top_speed;
                    break;
                case 2:
                    // run right
                    target_x_vel = -top_speed;
                    break;
            }
            action_start_ts = now;
        }
    } else if (type.action_type == ActionType::JUMP_AROUND) {
        // jump in a direction
        if (isOnGround() && action_len > 500) {
            int dir = rand() % 3;
            switch(dir) {
                case 0:
                    // stand still
                    target_x_vel = 0;
                    break;
                case 1:
                    // run right
                    target_x_vel = top_speed;
                    jump();
                    break;
                case 2:
                    // run right
                    target_x_vel = -top_speed;
                    jump();
                    break;
            }
            action_start_ts = now;
        }
    }
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
        Audio::instance().playSound(type.landed_sound);
    }
}

void Being::hitOther(Drawable &other) {
    if (other.isBouncy()) {
        y_vel = -jump_vel;
        Audio::instance().playSound(type.jump_sound);
    }
}

void Being::hitBy(Drawable &other) {
    takeDamage(other.getDamage());
}

void Being::ranInto(Drawable &other) {
    if (!bump_immune) {
        takeDamage(other.getDamage());
    }
}

void Being::takeDamage(int damage) {
    hp -= damage;
    Audio::instance().playSound(type.damaged_sound);
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
        // apply additional accel if we are at negative velocity
        float accel = movement_accel;
        if (x_vel < 0) {
            accel += CORRECTION_ACCEL;
        }
        x_vel += accel * delta;
        if (x_vel > top_speed) {
            x_vel = top_speed;
        }
        if (isOnGround()) {
            int played_ago = SDL_GetTicks() - Audio::instance().getLastPlayed(type.walk_sound);
            if (played_ago > WALK_SOUND_INTERVAL_MS) {
                Audio::instance().playSound(type.walk_sound);
            }
        }
    } else if (target_x_vel < 0 && x_vel > -top_speed) {
        // moving left
        // apply additional accel if we are at negative velocity
        float accel = movement_accel;
        if (x_vel > 0) {
            accel += CORRECTION_ACCEL;
        }
        x_vel -= accel * delta;
        if (x_vel < -top_speed) {
            x_vel = -top_speed;
        }
        if (isOnGround()) {
            int played_ago = SDL_GetTicks() - Audio::instance().getLastPlayed(type.walk_sound);
            if (played_ago > WALK_SOUND_INTERVAL_MS) {
                Audio::instance().playSound(type.walk_sound);
            }
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
    if (dead()) {
        sprite.setDead();
    } else if (!isOnGround()) {
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
