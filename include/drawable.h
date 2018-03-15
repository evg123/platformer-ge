//
//  drawable.h
//  abstract base class for objects drawn on screen
//
//  Created by Vande Griek, Eric on 2/23/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef drawable_h
#define drawable_h

#ifdef __linux__
#include "../lib/linux/SDL2.framework/Headers/SDL.h"
#else // Mac
#include "../lib/osx/SDL2.framework/Headers/SDL.h"
#endif

#include <vector>
#include <tuple>
#include <algorithm>
#include <queue>
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

    std::tuple<float, float> calcVelocityOffset(int delta);
    virtual void applyAcceleration(int delta);
public:
    Drawable();
    virtual ~Drawable();
    void init(int x_pos, int y_pos, int width, int height);

    // update based on delta in ms since last update
    virtual void update(int delta, std::vector<Drawable*> &objects);
    virtual void render() = 0;
    virtual void doMove(float x_offset, float y_offset, std::vector<Drawable*> &objects);
    virtual void processCollision(Drawable &other, float x_off, float y_off);

    /** Get the bounding rect of this object */
    virtual EvgRect& getRect() { return rect; }

    /** Get the number of points earned for destroying this object */
    int getScoreOnDestruction() { return score_on_destruction; }

    void setColor(int color_r, int color_g, int color_b, int alpha);
    void setVelocity(float x_vel, float y_vel);
    void setAcceleration(float x_acceleration, float y_acceleration);
    virtual void setPosition(int x_pos, int y_pos);

    /** true if this object needs to be cleaned up */
    bool needsRemoval() { return marked_for_removal; }
    void destroy();
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
