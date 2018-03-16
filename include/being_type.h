//
//  BeingType.h
//  type class for Beings
//
//  Created by Vande Griek, Eric on 3/1/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef being_type_h
#define being_type_h

#ifdef __linux__
#include "../lib/linux/SDL2.framework/Headers/SDL.h"
#else // Mac
#include "../lib/osx/SDL2.framework/Headers/SDL.h"
#endif

#include <map>
#include <string>
#include "sprite.h"

/**
 types of automated actions that beings can exhibit
 */
enum class ActionType {
    NONE,
    CHARGE,
    JUMP_AROUND,
};

class BeingType {
private:
public:
    // static functions to return the instance of each type
    static BeingType& player();
    static BeingType& redEnemy();
    static BeingType& blueEnemy();

    // game properties
    int hp;
    int damage;
    int score_on_destruction;
    bool bump_immune;

    // phsical
    int width;
    int height;
    std::string sprite_sheet;
    FrameConfig frame_config;

    // movement
    float top_speed;
    float jump_duration;
    int max_air_jumps;
    
    // callbacks
    ActionType action_type;
};

extern BeingType PLAYER_TYPE;

#endif /* player_h */
