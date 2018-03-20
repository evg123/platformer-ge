//
//  drawable.h
//  abstract base class for objects drawn on screen
//
//  Created by Vande Griek, Eric on 2/23/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef drawable_h
#define drawable_h

#include <vector>
#include <tuple>
#include <algorithm>
#include <queue>
#include <functional>
#include "SDL2.framework/Headers/SDL.h"
#include "evg_rect.h"
#include "resource_manager.h"

enum class Axis {
    X,
    Y,
};

/**
 Abstract base class for things drawn to the screen.
 Handles rendering, movement, and collision.
 */
class Drawable {
protected:
    EvgRect rect;
    SDL_Texture *texture = NULL;

    float x_vel = 0, y_vel = 0; // velocity
    float x_accel = 0, y_accel = 0; // acceleration

    bool marked_for_removal = false; // will be cleaned up and removed from game if true
    int score_on_destruction = 0; // points earned/lost for the destruction of this object
    int damage = 0; // damage done on collision
    bool bouncy = false; // beings will bounce off of this
    bool hit_back_when_hopped_on = false;

    // fired on collision
    std::function<void(Drawable&)> collision_callback;

    std::tuple<float, float> calcVelocityOffset(int delta);
    virtual void applyAcceleration(int delta);
    virtual void doMove(float x_offset, float y_offset, std::vector<Drawable*> &objects);
    virtual void processCollision(Drawable &other, float x_off, float y_off);
public:
    Drawable();
    virtual ~Drawable();
    // update based on delta in ms since last update
    virtual void update(int delta, std::vector<Drawable*> &objects);
    virtual void render() = 0;
    /** Get the bounding rect of this object */
    virtual EvgRect& getRect() { return rect; }
    /** Get the number of points earned for destroying this object */
    int getScoreOnDestruction() { return score_on_destruction; }
    int getDamage() { return damage; }
    bool isBouncy() { return bouncy; }
    void setColor(int color_r, int color_g, int color_b, int alpha);
    void setVelocity(float x_vel, float y_vel);
    void setAcceleration(float x_acceleration, float y_acceleration);
    virtual void setPosition(int x_pos, int y_pos);
    void setDamage(int damage) { this->damage = damage; }
    void setCollisionCallback(std::function<void(Drawable&)> callback) { collision_callback = callback; }
    /** true if this object needs to be cleaned up */
    bool needsRemoval() { return marked_for_removal; }
    void destroy();
    virtual void hitOther(Drawable &other) {};
    virtual void hitBy(Drawable &other) {};
    virtual void ranInto(Drawable &other) {};
};

/**
 Struct used during collision detection
 Defines < operator based on time, so it can be stored in priority queue
 */
struct CollisionRecord {
    Axis axis;
    float time;
    float diff;
    Drawable *other;

    bool operator<(const CollisionRecord &rhs) const { return time > rhs.time; }
};

#endif /* drawable_h */
