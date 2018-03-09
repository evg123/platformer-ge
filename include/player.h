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
#include <algorithm>
#include "being.h"
#include "graphics.h"

constexpr int PLAYER_WIDTH = 32;
constexpr int PLAYER_HEIGHT = 32;

constexpr auto PLAYER_SPRITE_SHEET = "sprites/player.png";
constexpr FrameConfig PLAYER_FRAME_CONFIG =
{
    0, 3, // idle
    5, 7, // walking
    8, 8, // jump
    9, 9, // braking
};

constexpr float PLAYER_MOVE_ACCEL = 500 / 1000.0f / 1000.0f;
constexpr float PLAYER_TOP_SPEED = 300 / 1000.0f;

constexpr int PLAYER_AIR_JUMP_COUNT = 1;
constexpr float PLAYER_JUMP_VELOCITY = 250 / 1000.0f;
constexpr float PLAYER_JUMP_TIME_MS = 300;

class Player : public Being {
private:
public:
    Player();
    void setPosition(int xpos, int ypos) override;
};

#endif /* player_h */
