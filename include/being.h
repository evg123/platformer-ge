//
//  being.h
//  platformer
//
//  Created by Vande Griek, Eric on 3/5/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef being_h
#define being_h

#include "drawable.h"

// in game scale: 32 pixels = 1 meter
// gravity: 32 px/m * 9.8 m/s/s / 1000 ms/s
constexpr float GRAVITY = (32 * 9.8) / 1000.0f / 1000.0f;
constexpr float TERMINAL_VELOCITY = (32 * 50) / 1000.0f;

class Being : public Drawable {
protected:
    int max_jumps = 1;
    int jumps = max_jumps;
    float jump_power = 0;
    bool on_ground = false;
    
    void resetJumps();
    void doMove(int x_offset, int y_offset, std::vector<Drawable*> &objects) override;
    void processCollision(Drawable &other, int x_off, int y_off) override;
    void applyAcceleration(int delta) override;
public:
    Being();
    void jump();
    bool canJump();
    bool isOnGround() { return on_ground; }
};

#endif /* being_h */
