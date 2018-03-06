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
        y_vel = 1.0 * jump_power;
        --jumps;
    }
}

bool Being::canJump() {
    return jumps > 0;
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
