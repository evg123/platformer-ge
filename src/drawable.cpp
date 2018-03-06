//
//  Created by Vande Griek, Eric on 2/23/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "../include/drawable.h"

/**
 Create a new drawable
 */
Drawable::Drawable() {}

/**
 Deallocate
 */
Drawable::~Drawable() {}

void Drawable::init(int x_pos, int y_pos, int width, int height) {
    rect.x = x_pos;
    rect.y = y_pos;
    rect.w = width;
    rect.h = height;
}

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
void Drawable::update(int delta, std::vector<Drawable*> &objects) {
    applyAcceleration(delta);
    int x_off, y_off;
    std::tie(x_off, y_off) = calcVelocityOffset(delta);
    doMove(x_off, y_off, objects);
}

/**
 Update velocity
 */
void Drawable::setVelocity(float x_velocity, float y_velocity) {
    x_vel = x_velocity;
    y_vel = y_velocity;
}

/**
 Update acceleration
 */
void Drawable::setAcceleration(float x_acceleration, float y_acceleration) {
    x_acc = x_acceleration;
    y_acc = y_acceleration;
}

/**
 Set the position to an absolute value
 */
void Drawable::setPosition(int x_pos, int y_pos) {
    rect.setPosition(x_pos, y_pos);
}

/**
 Update the position by offsetting previous position.
 Handle collisions.
 */
void Drawable::doMove(int x_offset, int y_offset, std::vector<Drawable*> &objects) {
    SDL_Rect new_rect = {rect.x + x_offset, rect.y + y_offset, rect.w, rect.h};
    
    Drawable *collision_other_x = NULL;
    Drawable *collision_other_y = NULL;
    int new_x_off = x_offset;
    int new_y_off = y_offset;
    for (auto &other : objects) {
        if (SDL_HasIntersection(&new_rect, &other->rect)) {
            if (x_offset > 0) {
                // find the distance to the other object on this axis
                int x_diff = other->rect.left() - rect.right();
                if (x_diff >= 0) {
                    // limit x_offset to that distance
                    if (x_diff < new_x_off) {
                        new_x_off = x_diff;
                        collision_other_x = other;
                    }
                } // else: a negative value means we aren't going to intersect in this axis
            } else if (x_offset < 0) {
                int x_diff = other->rect.right() - rect.left();
                if (x_diff <= 0) {
                    if (x_diff > new_x_off) {
                        new_x_off = x_diff;
                        collision_other_x = other;
                    }
                }
            }
            
            // y axis
            if (y_offset > 0) {
                int y_diff = other->rect.top() - rect.bottom();
                if (y_diff >= 0) {
                    if (y_diff < new_y_off) {
                        new_y_off = y_diff;
                        collision_other_y = other;
                    }
                }
            } else if (y_offset < 0) {
                int y_diff = other->rect.bottom() - rect.top();
                if (y_diff <= 0) {
                    if (y_diff > new_y_off) {
                        new_y_off = y_diff;
                        collision_other_y = other;
                    }
                }
            }

            
        }
    }
    
    // update position to as far as we could go
    setPosition(rect.x + new_x_off, rect.y + new_y_off);
    
    if (collision_other_x != NULL) {
        // register the collision
        processCollision(*collision_other_x, x_offset, 0);
    }
    if (collision_other_y != NULL) {
        // register the collision
        processCollision(*collision_other_y, 0, y_offset);
    }
}

void Drawable::processCollision(Drawable &other, int x_off, int y_off) {
    // stop our momentum
    if (x_off != 0) {
        x_vel = 0;
    }
    if (y_off != 0) {
        y_vel = 0;
    }
}

/**
 Calculate how far in both axis this drawable should move in delta milliseconds
 */
std::tuple<int, int> Drawable::calcVelocityOffset(int delta) {
    // move according to current velocity
    int x_off = x_vel * delta;
    int y_off = y_vel * delta;
    
    return std::make_tuple(x_off, y_off);
}

/**
 adjust velocity based on acceleration
 */
void Drawable::applyAcceleration(int delta) {
    x_vel += delta * x_acc;
    y_vel += delta * y_acc;
}
