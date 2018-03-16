//
//  player.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 3/1/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "../include/being_type.h"

// these should be read from a config file instead of statically defined

BeingType& BeingType::player() {
    static BeingType this_type;
    static bool init_done = false;
    if (!init_done) {
        init_done = true;

        this_type.sprite_sheet = "sprites/player.png";
        this_type.frame_config =
        {
            0, 3, // idle
            5, 7, // walking
            8, 8, // jump
            9, 9, // braking
        };
        this_type.hp = 1;
        this_type.damage = 1;
        this_type.score_on_destruction = 0;
        this_type.width = 32;
        this_type.height = 32;
        this_type.top_speed = 300 / 1000.0f;
        this_type.max_air_jumps = 1;
        this_type.jump_duration = 300;

        this_type.action_type = ActionType::NONE;
        this_type.bump_immune = false;
    }
    return this_type;
}

BeingType& BeingType::redEnemy() {
    static BeingType this_type;
    static bool init_done = false;
    if (!init_done) {
        init_done = true;
        
        this_type.sprite_sheet = "sprites/red_enemy.png";
        this_type.frame_config =
        {
            0, 1, // idle
            6, 11, // walking
            4, 4, // jump
            6, 6, // braking
        };
        this_type.hp = 1;
        this_type.damage = 1;
        this_type.score_on_destruction = 100;
        this_type.width = 32;
        this_type.height = 32;
        this_type.top_speed = 200 / 1000.0f;
        this_type.max_air_jumps = 0;
        this_type.jump_duration = 150;
        
        this_type.action_type = ActionType::CHARGE_TO_EDGE;
        this_type.bump_immune = true;
    }
    return this_type;
}

BeingType& BeingType::blueEnemy() {
    static BeingType this_type;
    static bool init_done = false;
    if (!init_done) {
        init_done = true;
        
        this_type.sprite_sheet = "sprites/blue_enemy.png";
        this_type.frame_config =
        {
            0, 1, // idle
            6, 11, // walking
            4, 4, // jump
            2, 2, // braking
        };
        this_type.hp = 1;
        this_type.damage = 1;
        this_type.score_on_destruction = 500;
        this_type.width = 32;
        this_type.height = 32;
        this_type.top_speed = 150 / 1000.0f;
        this_type.max_air_jumps = 0;
        this_type.jump_duration = 150;
        
        this_type.action_type = ActionType::JUMP_AROUND;
        this_type.bump_immune = true;
    }
    return this_type;
}


