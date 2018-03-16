//
//  being.h
//  Being class represents handles movement and display of player and enemies
//
//  Created by Vande Griek, Eric on 3/5/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef being_h
#define being_h

#include "drawable.h"
#include "sprite.h"

constexpr float GRAVITY = 500 / 1000.0f / 1000.0f;
constexpr float TERMINAL_VELOCITY = 1500 / 1000.0f;

constexpr float FRICTION = 800 / 1000.0f / 1000.0f;

constexpr int JUMP_TOLERANCE_MS = 200; // can jump has touched ground in the last X milliseconds

// enum for direction being is facing
enum class Facing {
    RIGHT,
    LEFT,
};

class Being : public Drawable {
protected:
    Sprite sprite;
    Facing facing = Facing::RIGHT;

    int hp = 1;
    int max_air_jumps = 1;
    int air_jumps = max_air_jumps;
    float jump_vel = 0;
    float jump_duration = 0;
    float jump_start_ts = 0; // 0 means not jumping
    unsigned int last_grounded = 0; // ts of the last time we landed on something

    float movement_accel = 0;
    float top_speed = 0;
    float target_x_vel = 0;

    void resetJumps();
    void doMove(float x_offset, float y_offset, std::vector<Drawable*> &objects) override;
    void update(int delta, std::vector<Drawable*> &objects) override;
    void updateSprite();
    void render() override;
    void processCollision(Drawable &other, float x_off, float y_off) override;
    void applyAcceleration(int delta) override;
public:
    Being();
    void init();
    int getHp() { return hp; }
    void jump();
    bool canJump();
    bool isOnGround();
    void moveRight();
    void stopRight();
    void moveLeft();
    void stopLeft();
};

#endif /* being_h */
