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
#include "being.h"
#include "graphics.h"

constexpr int PLAYER_WIDTH = 20;
constexpr int PLAYER_HEIGHT = 20;

constexpr auto PLAYER_SPRITE_SHEET = "sprites/player.png";

constexpr int PLAYER_JUMP_COUNT = 2;
constexpr float PLAYER_JUMP_POWER = 0.3;

class Player : public Being {
private:
public:
    Player();
    void setPosition(int xpos, int ypos) override;
    void render() override;
    
    void moveRight();
    void stopRight();
    void moveLeft();
    void stopLeft();
};

#endif /* player_h */
