//
//  Created by Vande Griek, Eric on 3/5/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "being.h"

/**
 Set up the being using the passed in type
 */
void Being::init(BeingType type) {
    marked_for_removal = false;
    air_jumps = 0;
    jump_vel = JUMP_VELOCITY;
    jump_start_ts = 0;
    last_grounded = 0;
    movement_accel = MOVE_ACCEL;
    target_x_vel = 0;
    action_start_ts = 0;
    destroy_at_ts = 0;
    x_vel = 0;
    y_vel = 0;
    x_accel = 0;
    y_accel = 0;
    
    // unpack type class
    this->type = type;
    tile_num = type.tile_num;
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

/**
 Mark for destruction and play death sound
 */
void Being::destroy() {
    Drawable::destroy();
    hp = 0;
    Audio::instance().playSound(type.damaged_sound);
}

/**
 Return true if the being is currently considered on the ground
 */
bool Being::isOnGround() {
    //TODO could calculate this once on update
    unsigned long diff = getTime() - last_grounded;
    return diff <= JUMP_TOLERANCE_MS || jump_start_ts == 0;
}

/**
 Reset the being's jumps, such as when they land
 */
void Being::resetJumps() {
    air_jumps = max_air_jumps;
}

/**
 Initiate a jump
 */
void Being::jump() {
    if (canJump()) {
        if (!isOnGround()) {
            --air_jumps;
        }
        jump_start_ts = getTime();
        last_grounded = 0;
        Audio::instance().playSound(type.jump_sound);
    }
}

/**
 True if the being is allowed to jump currently
 */
bool Being::canJump() {
    return isOnGround() || air_jumps > 0;
}

/**
 True if the being has died
 */
bool Being::dead() {
    return hp <= 0;
}

/**
 Update the pysics of this being
 Handled movement and collisions
 */
void Being::updatePhysics(long delta, std::map<int, Drawable*> &objects) {
    if (!dead()) {
        performAction(delta);
        Drawable::updatePhysics(delta, objects);
    }
}

/**
 Update the game state of this being
 */
void Being::updateState(long delta) {
    if (dead()) {
        long now = getTime();
        if (destroy_at_ts == 0) {
            destroy_at_ts = now + BEING_DEATH_DELAY_MS;
        } else if (now >= destroy_at_ts && !needsRemoval()) {
            destroy();
        }
    }

    updateSprite();
}

/**
 Update this drawable using the values from a state object
 Used by HopmanClient
 */
void Being::updateWithObjectState(ObjectStateMsg &state) {
    Drawable::updateWithObjectState(state);
    hp = state.hp;
    air_jumps = state.air_jumps;
    jump_start_ts = state.jump_start_ts;
    if (state.jump_start_ts == 0) {
        last_grounded = getTime();
    }
}

/**
 Update the being based on input received from the client
 Used by HopmanServer
 */
void Being::updateWithInput(ClientInputMsg &input) {
    target_x_vel = input.target_x_vel;
    if (input.target_x_vel > 0) {
        moveRight();
    } else if (input.target_x_vel < 0) {
        moveLeft();
    }
    if (input.jumped) {
        jump();
    }
}

/**
 Draw the being to the screen
 */
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
void Being::performAction(long delta) {
    long now = getTime();
    long action_len = now - action_start_ts;
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

/**
 Try to move the being up to x_offset and y_offset
 */
void Being::doMove(float x_offset, float y_offset, std::map<int, Drawable*> &objects) {
    //TODO override no longer needed
    // call the superclass doMove
    Drawable::doMove(x_offset, y_offset, objects);
}

/**
 Update the being based on it colliding with other.
 One of x_off or y_off will be set to indicate the direction the being collided with other
 */
void Being::processCollision(Drawable &other, float x_off, float y_off) {
    // do the default
    Drawable::processCollision(other, x_off, y_off);

    if (y_off > 0) {
        // we have collided while moving down,
        // so we have landed on something
        if (!isOnGround()) {
            Audio::instance().playSound(type.landed_sound);
        }
        last_grounded = getTime();
        jump_start_ts = 0; // zero means not jumping
        resetJumps();
    }
}

/**
 Called when this being hits a drawable
 */
void Being::hitOther(Drawable &other) {
    if (other.isBouncy()) {
        y_vel = -jump_vel;
        Audio::instance().playSound(type.jump_sound);
    }
}

/**
 Called when this being is hit by other
 */
void Being::hitBy(Drawable &other) {
    if (getTileNum() != other.getTileNum()) {
        // don't hurt beings of the same type
        takeDamage(other.getDamage());
    }
}

/**
 Called when this being bumps into other
 */
void Being::ranInto(Drawable &other) {
    if (!bump_immune && getTileNum() != other.getTileNum()) {
        takeDamage(other.getDamage());
    }
}

/**
 Apply damage to this being
 */
void Being::takeDamage(int damage) {
    if (damage > 0) {
        hp -= damage;
        Audio::instance().playSound(type.damaged_sound);
    }
}

/**
 adjust velocity based on acceleration
 */
void Being::applyAcceleration(long delta) {
    // do default first
    Drawable::applyAcceleration(delta);
    
    // apply gravity
    y_vel += GRAVITY * delta;
    
    // limit max fall velocity
    if (y_vel > TERMINAL_VELOCITY) {
        y_vel = TERMINAL_VELOCITY;
    }

    // vertical movement / jump
    if (jump_start_ts != 0 && getTime() - jump_start_ts <= jump_duration) {
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
    } else if (target_x_vel == 0.0f && isOnGround()) {
        // stopping
        // slow to zero unless we are in the air
        if (x_vel > 0) {
            x_vel = std::max(0.0f, x_vel - FRICTION * delta);
        } else if (x_vel < 0) {
            x_vel = std::min(0.0f, x_vel + FRICTION * delta);
        }
    }

    // play sounds
    if (target_x_vel != 0 and isOnGround()) {
        long played_ago = getTime() - Audio::instance().getLastPlayed(type.walk_sound);
        if (played_ago > WALK_SOUND_INTERVAL_MS) {
            Audio::instance().playSound(type.walk_sound);
        }
    }
}

/**
 Set this being to try to move to the right.
 Goes until stopRight is called.
 */
void Being::moveRight() {
    target_x_vel = std::min(top_speed, target_x_vel + top_speed);
    facing = Facing::RIGHT;
}

/**
 Begin to stop moving to the right
 */
void Being::stopRight() {
    target_x_vel = std::max(0.0f, target_x_vel - top_speed);
}

/**
 Set this being to try to move to the left.
 Goes until stopLeft is called.
 */
void Being::moveLeft() {
    target_x_vel = std::max(-top_speed, target_x_vel - top_speed);
    facing = Facing::LEFT;
}

/**
 Begin to stop moving to the left
 */
void Being::stopLeft() {
    target_x_vel = std::min(0.0f, target_x_vel + top_speed);
}

/**
 Update the state of this being's sprite based on what the being is doing
 */
void Being::updateSprite() {
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

/**
 Fill in the given state message with values from this being
 */
void Being::fillObjectState(ObjectStateMsg &state) {
    Drawable::fillObjectState(state);
    state.hp = hp;
    state.air_jumps = air_jumps;
    state.jump_start_ts = jump_start_ts;
}
