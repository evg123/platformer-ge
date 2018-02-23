//
//  Created by Vande Griek, Eric on 2/23/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "../include/drawable.h"

/**
 Create a new drawable with specified location and size
 */
Drawable::Drawable(int x_pos, int y_pos, int width, int height)
    : rect(x_pos, y_pos, width, height) {}

/**
 Deallocate
 */
Drawable::~Drawable() {}

/**
 Mark the drawable for removal at the next sweep
 */
void Drawable::destroy() {
    marked_for_removal = true;
}

/**
 Update the object based on how much time has passed.
 delta is in ms.
 */
bool Drawable::update(int delta, std::vector<Drawable*> &objects) {
    int x_off, y_off;
    std::tie(x_off, y_off) = calcVelocityOffset(delta);
    move(x_off, y_off);
    return x_off || y_off;
}

/**
 Update velocity
 */
void Drawable::setVelocity(float x_velocity, float y_velocity) {
    x_vel = x_velocity;
    y_vel = y_velocity;
}

/**
 Update color
 */
void Drawable::setColor(int color_r, int color_g, int color_b, int alpha) {
    this->color_r = color_r;
    this->color_g = color_g;
    this->color_b = color_b;
    this->alpha = alpha;
}

/**
 Set the position to an absolute value
 */
void Drawable::setPosition(int x_pos, int y_pos) {
    rect.setPosition(x_pos, y_pos);
}

/**
 Update the position by offsetting previous position
 */
void Drawable::move(int x_offset, int y_offset) {
    rect.move(x_offset, y_offset);
}

/**
 Calculate how far in both axis this drawable should move in delta milliseconds
 */
std::tuple<int, int> Drawable::calcVelocityOffset(int delta) {
    // move according to current velocity
    int x_off = x_vel * delta * speed;
    int y_off = y_vel * delta * speed;
    
    return std::make_tuple(x_off, y_off);
}
