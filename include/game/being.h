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
#include "audio.h"

constexpr float GRAVITY = 500 / 1000.0f / 1000.0f;
constexpr float TERMINAL_VELOCITY = 1500 / 1000.0f;
constexpr float FRICTION = 800 / 1000.0f / 1000.0f;

constexpr float BEING_DEATH_DELAY_MS = 1500; // keep dead enemies on the screen for this long
constexpr int JUMP_TOLERANCE_MS = 200; // can jump has touched ground in the last X milliseconds
constexpr int WALK_SOUND_INTERVAL_MS = 300; // play the walk sound every x ms while walking

constexpr float MOVE_ACCEL = 500 / 1000.0f / 1000.0f;
constexpr float CORRECTION_ACCEL = 200 / 1000.0f / 1000.0f;
constexpr float JUMP_VELOCITY = 250 / 1000.0f;

/**
 enum for direction being is facing
 */
enum class Facing {
    RIGHT,
    LEFT,
};

/**
 A Being represents a character in the game world.
 Can be a player or an enemy depending on its BeingType.
 */
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

    int air_jumps;
    float jump_vel;
    int jump_start_ts; // 0 means not jumping
    unsigned int last_grounded; // ts of the last time we landed on something
    float movement_accel;
    float target_x_vel;
    int action_start_ts;
    int destroy_at_ts;

    void resetJumps();
    void doMove(float x_offset, float y_offset, std::vector<Drawable*> &objects) override;
    void update(int delta, std::vector<Drawable*> &objects) override;
    void performAction(int delta);
    void updateSprite();
    void render() override;
    void processCollision(Drawable &other, float x_off, float y_off) override;
    void applyAcceleration(int delta) override;
    void takeDamage(int damage);
public:
    void init(BeingType type);
    void destroy() override;
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
