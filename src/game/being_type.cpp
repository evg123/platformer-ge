//
//  Created by Vande Griek, Eric on 3/1/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "being_type.h"

// these should be read from a config file instead of statically defined

/**
 Get the BeingType for a player
 */
BeingType& BeingType::player() {
    static BeingType this_type;
    static bool init_done = false;
    if (!init_done) {
        init_done = true;

        this_type.tile_num = TileNum::PLAYER;
        this_type.sprite_sheet = "sprites/player.png";
        this_type.frame_config =
        {
            0, 3, // idle
            5, 7, // walking
            8, 8, // jump
            9, 9, // braking
            4, 4, // dead
        };
        this_type.pad_top = 4;
        this_type.pad_right = 6;
        this_type.pad_bot = 0;
        this_type.pad_left = 6;

        this_type.hp = 1;
        this_type.damage = 1;
        this_type.score_on_destruction = 0;
        this_type.width = 32 - this_type.pad_left - this_type.pad_right;
        this_type.height = 32 - this_type.pad_top - this_type.pad_bot;
        this_type.top_speed = 300 / 1000.0f;
        this_type.max_air_jumps = 1;
        this_type.jump_duration = 275;

        this_type.action_type = ActionType::NONE;
        this_type.bump_immune = false;
        this_type.bouncy = true;
        this_type.hit_back_when_hopped_on = false;

        this_type.walk_sound = "player_walk.wav";
        this_type.jump_sound = "player_jump.wav";
        this_type.landed_sound = "player_landed.wav";
        this_type.damaged_sound = "player_damaged.wav";
        this_type.death_sound = "player_death.wav";
    }
    return this_type;
}

/**
 Get the BeingType for a red enemey
 */
BeingType& BeingType::redEnemy() {
    static BeingType this_type;
    static bool init_done = false;
    if (!init_done) {
        init_done = true;

        this_type.tile_num = TileNum::RED_ENEMY;
        this_type.sprite_sheet = "sprites/red_enemy.png";
        this_type.frame_config =
        {
            0, 1, // idle
            5, 9, // walking
            3, 3, // jump
            9, 9, // braking
            2, 2, // dead
        };
        this_type.pad_top = 4;
        this_type.pad_right = 3;
        this_type.pad_bot = 0;
        this_type.pad_left = 3;

        this_type.hp = 1;
        this_type.damage = 1;
        this_type.score_on_destruction = 100;
        this_type.width = 32 - this_type.pad_left - this_type.pad_right;
        this_type.height = 32 - this_type.pad_top - this_type.pad_bot;
        this_type.top_speed = 200 / 1000.0f;
        this_type.max_air_jumps = 0;
        this_type.jump_duration = 150;
        
        this_type.action_type = ActionType::CHARGE;
        this_type.bump_immune = true;
        this_type.bouncy = true;
        this_type.hit_back_when_hopped_on = false;

        this_type.walk_sound = "";
        this_type.jump_sound = "";
        this_type.landed_sound = "";
        this_type.damaged_sound = "";
        this_type.death_sound = "";
    }
    return this_type;
}

/**
 Get the BeingType for a blue enemy
 */
BeingType& BeingType::blueEnemy() {
    static BeingType this_type;
    static bool init_done = false;
    if (!init_done) {
        init_done = true;

        this_type.tile_num = TileNum::BLUE_ENEMY;
        this_type.sprite_sheet = "sprites/blue_enemy.png";
        this_type.frame_config =
        {
            0, 1, // idle
            5, 9, // walking
            4, 4, // jump
            9, 9, // braking
            3, 3, // dead
        };
        this_type.pad_top = 4;
        this_type.pad_right = 3;
        this_type.pad_bot = 0;
        this_type.pad_left = 3;

        this_type.hp = 1;
        this_type.damage = 1;
        this_type.score_on_destruction = 500;
        this_type.width = 32 - this_type.pad_left - this_type.pad_right;
        this_type.height = 32 - this_type.pad_top - this_type.pad_bot;
        this_type.top_speed = 150 / 1000.0f;
        this_type.max_air_jumps = 0;
        this_type.jump_duration = 25;
        
        this_type.action_type = ActionType::JUMP_AROUND;
        this_type.bump_immune = true;
        this_type.bouncy = true;
        this_type.hit_back_when_hopped_on = true;

        this_type.walk_sound = "";
        this_type.jump_sound = "";
        this_type.landed_sound = "";
        this_type.damaged_sound = "";
        this_type.death_sound = "";
    }
    return this_type;
}


