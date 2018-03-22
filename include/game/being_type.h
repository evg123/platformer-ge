//
//  BeingType.h
//  type class for Beings
//
//  Created by Vande Griek, Eric on 3/1/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef being_type_h
#define being_type_h

#include <map>
#include <string>
#include "SDL2.framework/Headers/SDL.h"
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
    bool bouncy;
    bool hit_back_when_hopped_on;

    // phsical
    int width;
    int height;
    std::string sprite_sheet;
    FrameConfig frame_config;
    int pad_top;
    int pad_right;
    int pad_bot;
    int pad_left;

    // movement
    float top_speed;
    float jump_duration;
    int max_air_jumps;
    
    // sounds
    std::string walk_sound;
    std::string jump_sound;
    std::string landed_sound;
    std::string damaged_sound;
    std::string death_sound;
    
    // callbacks
    ActionType action_type;
};

#endif /* player_h */
