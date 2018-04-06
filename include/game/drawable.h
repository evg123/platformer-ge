//
//  drawable.h
//  abstract base class for objects drawn on screen
//
//  Created by Vande Griek, Eric on 2/23/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef drawable_h
#define drawable_h

#include <map>
#include <tuple>
#include <algorithm>
#include <queue>
#include <functional>
#include "SDL.h"
#include "evg_rect.h"
#include "resource_manager.h"
#include "network.h"

enum class Axis {
    X,
    Y,
};

// these should be in a config file so they can be shared with the level creator
/**
 Mapping of tile type to tile number.
 Used by the level creator as well.
 */
enum TileNum {
    EMPTY = 0,
    DIRT = 1,
    STEEL = 2,
    DAMAGE = 3,
    GOAL = 4,
    PLAYER = 5,
    RED_ENEMY = 6,
    BLUE_ENEMY = 7,
};

/**
 Abstract base class for things drawn to the screen.
 Handles rendering, movement, and collision.
 */
class Drawable {
protected:
    int id;
    int tile_num;
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
    virtual void doMove(float x_offset, float y_offset, std::map<int, Drawable*> &objects);
    virtual void processCollision(Drawable &other, float x_off, float y_off);
public:
    Drawable();
    /** Destructor */
    virtual ~Drawable() {};
    /** Return unique id */
    int getId() { return id; }
    /** Set unique id */
    void setId(int new_id) { id = new_id; }
    // update based on delta in ms since last update
    virtual void update(int delta, std::map<int, Drawable*> &objects);
    virtual void updateWithObjectState(ObjectStateMsg &state);
    virtual void render() = 0;
    /** Get the bounding rect of this object */
    virtual EvgRect& getRect() { return rect; }
    /** Get the number of points earned for destroying this object */
    int getScoreOnDestruction() { return score_on_destruction; }
    /** Return the amount of damage this drawable does when it hits something */
    int getDamage() { return damage; }
    /** Return true if things will bounce off of this drawable */
    bool isBouncy() { return bouncy; }
    void setColor(int color_r, int color_g, int color_b, int alpha);
    void setVelocity(float x_vel, float y_vel);
    void setAcceleration(float x_acceleration, float y_acceleration);
    virtual void setPosition(int x_pos, int y_pos);
    /** Set the amount of damage this drawable does when it hits something */
    void setDamage(int damage) { this->damage = damage; }
    /** Set the callback that gets called on collision */
    void setCollisionCallback(std::function<void(Drawable&)> callback) { collision_callback = callback; }
    /** true if this object needs to be cleaned up */
    bool needsRemoval() { return marked_for_removal; }
    virtual void destroy();
    /** Called when this drawable hits another. Default does nothing */
    virtual void hitOther(Drawable &other) {};
    /** Called when this drawable is hit by another. Default does nothing */
    virtual void hitBy(Drawable &other) {};
    /** Called when this drawable bumps into another. Default does nothing */
    virtual void ranInto(Drawable &other) {};
    virtual void fillObjectState(ObjectStateMsg &state);
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

    /** Comparison operator so records can be sorted by the time they occurred */
    bool operator<(const CollisionRecord &rhs) const { return time > rhs.time; }
};

#endif /* drawable_h */
