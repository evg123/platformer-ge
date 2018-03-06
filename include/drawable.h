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
#include "evg_rect.h"
#include "resource_manager.h"

/**
 Abstract base class for things drawn to the screen.
 Handles rendering, movement, and collision.
 */
class Drawable {
protected:
    EvgRect rect;
    SDL_Texture *texture = NULL;
    
    float x_vel = 0, y_vel = 0; // velocity
    float x_acc = 0, y_acc = 0; // acceleration
    float speed = 0.3; // movement velocity modifier
    
    bool marked_for_removal = false; // will be cleaned up and removed from game if true
    int score_on_destruction = 0; // points earned/lost for the destruction of this object

    std::tuple<int, int> calcVelocityOffset(int delta);
    virtual void applyAcceleration(int delta);
public:
    Drawable();
    virtual ~Drawable();
    void init(int x_pos, int y_pos, int width, int height);

    // update based on delta in ms since last update
    virtual void update(int delta, std::vector<Drawable*> &objects);
    virtual void render() = 0;
    virtual void doMove(int x_offset, int y_offset, std::vector<Drawable*> &objects);
    virtual void processCollision(Drawable &other, int x_off, int y_off);

    /** Get the bounding rect of this object */
    virtual EvgRect& getRect() { return rect; }
    
    /** Set the speed modifier of this object */
    virtual void setSpeed(float speed) { this->speed = speed; };

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

#endif /* drawable_h */
