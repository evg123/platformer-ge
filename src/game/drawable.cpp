//
//  Created by Vande Griek, Eric on 2/23/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "drawable.h"

/**
 Create a new drawable and assign it an id
 */
Drawable::Drawable() {
    static int draw_id = 1;
    id = draw_id++;
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
void Drawable::update(long delta, std::map<int, Drawable*> &objects) {
    updatePhysics(delta, objects);
    updateState(delta);
}

void Drawable::updatePhysics(long delta, std::map<int, Drawable*> &objects) {
    applyAcceleration(delta);
    float x_off, y_off;
    std::tie(x_off, y_off) = calcVelocityOffset(delta);
    doMove(x_off, y_off, objects);
}

void Drawable::updateState(long delta) {
    // nothing in base class
}

/**
 Update this drawable using the values from a state object
 */
void Drawable::updateWithObjectState(ObjectStateMsg &state) {
    /*
    if (smooth) {
        // smooth out updates based on how far off they are
        int xpos = smoothValue(rect.xPos(), state.xpos, MIN_SMOOTH_POS, MAX_SMOOTH_POS);
        int ypos = smoothValue(rect.yPos(), state.ypos, MIN_SMOOTH_POS, MAX_SMOOTH_POS);
        setPosition(xpos, ypos);
    }
    */
    setPosition(state.xpos, state.ypos);
    x_vel = state.xvel;
    y_vel = state.yvel;
    intangible = state.intangible;
    marked_for_removal = state.marked_for_removal;
}

float Drawable::smoothValue(float current, float update, float min_off, float max_off) {
    float diff = current - update;
    if (std::abs(diff) < min_off) {
        // below the threshold, just keep current value
        return current;
    }
    if (std::abs(diff) > max_off) {
        // above max threshold, jump to the updated value
        return update;
    }
    // move 10% towards the update
    return current + diff * 0.1f;
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
    x_accel = x_acceleration;
    y_accel = y_acceleration;
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
void Drawable::doMove(float x_offset, float y_offset, std::map<int, Drawable*> &objects) {
    //TODO adjust collision rect here?
    SDL_Rect new_rect = {
        int(rect.getCollider().x + x_offset),
        int(rect.getCollider().y + y_offset),
        rect.getCollider().w,
        rect.getCollider().h
    };

    std::priority_queue<CollisionRecord> collisions;
    for (auto &other_record : objects) {
        Drawable *other = other_record.second;
        if (other->intangible) {
            // this object can't be interacted with
            continue;
        }
        if (SDL_HasIntersection(&new_rect, &other->rect.getCollider())) {
            if (x_offset > 0) {
                // find the distance to the other object on this axis
                float x_diff = other->rect.left() - rect.right();
                if (x_diff >= 0) {
                    // save a record of this collision
                    float collision_time = x_diff / x_offset;
                    CollisionRecord record = {Axis::X, collision_time, x_diff, other};
                    collisions.push(record);
                } // else: a negative value means we aren't going to intersect in this axis
            } else if (x_offset < 0) {
                float x_diff = other->rect.right() - rect.left();
                if (x_diff <= 0) {
                    // save a record of this collision
                    float collision_time = x_diff / x_offset;
                    CollisionRecord record = {Axis::X, collision_time, x_diff, other};
                    collisions.push(record);
                }
            }
            
            // y axis
            if (y_offset > 0) {
                float y_diff = other->rect.top() - rect.bottom();
                if (y_diff >= 0) {
                    // save a record of this collision
                    float collision_time = y_diff / y_offset;
                    CollisionRecord record = {Axis::Y, collision_time, y_diff, other};
                    collisions.push(record);
                }
            } else if (y_offset < 0) {
                float y_diff = other->rect.bottom() - rect.top();
                if (y_diff <= 0) {
                    // save a record of this collision
                    float collision_time = y_diff / y_offset;
                    CollisionRecord record = {Axis::Y, collision_time, y_diff, other};
                    collisions.push(record);
                }
            }
        }
    }

    // the other drawable that was collided with in each axis
    Drawable *collision_other_x = NULL;
    Drawable *collision_other_y = NULL;
    
    bool have_x = false;
    bool have_y = false;
    while (!collisions.empty()) {
        CollisionRecord record = collisions.top();
        collisions.pop();
        if (record.axis == Axis::X) {
            if (!have_x && SDL_HasIntersection(&new_rect, &record.other->rect.getCollider())) {
                new_rect.x = rect.getCollider().x + record.diff;
                collision_other_x = record.other;
                have_x = true;
            }
        } else { // y
            if (!have_y && SDL_HasIntersection(&new_rect, &record.other->rect.getCollider())) {
                new_rect.y = rect.getCollider().y + record.diff;
                collision_other_y = record.other;
                have_y = true;
            }
        }
        if (have_x && have_y) {
            // the earliest collisions have been handled for both x and y
            // we can stop now
            break;
        }
    }

    // update position to as far as we could go
    setPosition(new_rect.x, new_rect.y);
    
    if (collision_other_x != NULL) {
        // register the collision
        processCollision(*collision_other_x, x_offset, 0);
    }
    if (collision_other_y != NULL) {
        // register the collision
        processCollision(*collision_other_y, 0, y_offset);
    }
}

/**
 Take action based on a collision with other.
 Only one of x_off or y_off will be filled in.
 */
void Drawable::processCollision(Drawable &other, float x_off, float y_off) {
    // stop our momentum
    if (x_off != 0) {
        x_vel = 0;
    }
    if (y_off != 0) {
        y_vel = 0;
    }

    // run callbacks
    if (collision_callback) {
        collision_callback(other);
    }
    if (other.collision_callback) {
        other.collision_callback(*this);
    }

    if (y_off > 0) {
        // we jumped on other
        if (other.hit_back_when_hopped_on) {
            // other damages us (spikes, etc)
            hitBy(other);
            other.hitOther(*this);
        } else {
            // we damage the other
            hitOther(other);
            other.hitBy(*this);
        }
    } else if (y_off < 0) {
        // we jumped into other's feet
        if (hit_back_when_hopped_on) {
            // we damage the other
            hitOther(other);
            other.hitBy(*this);
        } else {
            // they jumped on us
            hitBy(other);
            other.hitOther(*this);
        }
    } else if (x_off != 0) {
        // we ran into each other
        ranInto(other);
        other.ranInto(*this);
    }
}

/**
 Calculate how far in both axis this drawable should move in delta milliseconds
 */
std::tuple<float, float> Drawable::calcVelocityOffset(long delta) {
    // move according to current velocity
    float x_off = x_vel * delta;
    float y_off = y_vel * delta;
    
    return std::make_tuple(x_off, y_off);
}

/**
 adjust velocity based on acceleration
 */
void Drawable::applyAcceleration(long delta) {
    x_vel += delta * x_accel;
    y_vel += delta * y_accel;
}

void Drawable::fillObjectState(ObjectStateMsg &state) {
    state.id = id;
    state.type = tile_num;
    state.xpos = rect.xPos();
    state.ypos = rect.yPos();
    state.xvel = x_vel;
    state.yvel = y_vel;
    state.intangible = intangible;
    state.marked_for_removal = marked_for_removal;
}
