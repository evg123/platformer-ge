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
#include "being_type.h"
#include "sprite.h"

constexpr float GRAVITY = 500 / 1000.0f / 1000.0f;
constexpr float TERMINAL_VELOCITY = 1500 / 1000.0f;

constexpr float FRICTION = 800 / 1000.0f / 1000.0f;

constexpr int JUMP_TOLERANCE_MS = 200; // can jump has touched ground in the last X milliseconds

constexpr float MOVE_ACCEL = 500 / 1000.0f / 1000.0f;
constexpr float CORRECTION_ACCEL = 200 / 1000.0f / 1000.0f;
constexpr float JUMP_VELOCITY = 250 / 1000.0f;

constexpr float BEING_DEATH_DELAY_MS = 1500;

// enum for direction being is facing
enum class Facing {
    RIGHT,
    LEFT,
};

class Being : public Drawable {
protected:
    BeingType type;
    Sprite sprite;
    Facing facing = Facing::RIGHT;

    // filled in by BeingType
    int hp;
    float top_speed;
    float jump_duration;
    int max_air_jumps;
    bool bump_immune; // are we hurt by walking into other beings?

    int air_jumps = 0;
    float jump_vel = JUMP_VELOCITY;
    int jump_start_ts = 0; // 0 means not jumping
    unsigned int last_grounded = 0; // ts of the last time we landed on something
    float movement_accel = MOVE_ACCEL;
    float target_x_vel = 0;
    int action_start_ts = 0;
    int destroy_at_ts = 0;

    void resetJumps();
    void doMove(float x_offset, float y_offset, std::vector<Drawable*> &objects) override;
    void update(int delta, std::vector<Drawable*> &objects) override;
    void performAction(int delta);
    void updateSprite();
    void render() override;
    void processCollision(Drawable &other, float x_off, float y_off) override;
    void applyAcceleration(int delta) override;
public:
    Being();
    void init(BeingType type);
    bool dead();
    void jump();
    bool canJump();
    bool isOnGround();
    void moveRight();
    void stopRight();
    void moveLeft();
    void stopLeft();

    void hitOther(Drawable &other) override;
    void hitBy(Drawable &other) override;
    void ranInto(Drawable &other) override;
};

#endif /* being_h */
