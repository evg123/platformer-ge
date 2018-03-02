//
//  player.h
//  platformer
//
//  Created by Vande Griek, Eric on 3/1/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef player_h
#define player_h

#ifdef __linux__
#include "../lib/linux/SDL2.framework/Headers/SDL.h"
#else // Mac
#include "../lib/osx/SDL2.framework/Headers/SDL.h"
#endif

#include <vector>
#include "drawable.h"
#include "graphics.h"

constexpr int PLAYER_WIDTH = 20;
constexpr int PLAYER_HEIGHT = 20;

constexpr auto PLAYER_SPRITE_SHEET = "sprites/player.png";

constexpr float GRAVITY = 0.001;
constexpr float PLAYER_JUMP_POWER = 0.5;

class Player : public Drawable {
private:
    float jump_power = PLAYER_JUMP_POWER;
public:
    Player();
    void setPosition(int xpos, int ypos) override;
    void render() override;
    
    void moveRight();
    void stopRight();
    void moveLeft();
    void stopLeft();
    void jump();

    bool canJump();
};

#endif /* player_h */
