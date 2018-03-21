//
//  sprite.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 3/6/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "sprite.h"

void Sprite::init(FrameConfig frame_config) {
    this->frame_config = frame_config;
    setState(SpriteState::IDLE);
}

void Sprite::setState(SpriteState state) {
    if (state == current_state) {
        // nothing to do
        return;
    }

    current_state = state;

    if (state == SpriteState::IDLE) {
        frame_start = frame_config.idle_frame_start;
        frame_count = frame_config.idle_frame_final - frame_config.idle_frame_start + 1;
    } else if (state == SpriteState::WALKING) {
        frame_start = frame_config.walking_frame_start;
        frame_count = frame_config.walking_frame_final - frame_config.walking_frame_start + 1;
    } else if (state == SpriteState::JUMPING) {
        frame_start = frame_config.jumping_frame_start;
        frame_count = frame_config.jumping_frame_final - frame_config.jumping_frame_start + 1;
    } else if (state == SpriteState::BRAKING) {
        frame_start = frame_config.braking_frame_start;
        frame_count = frame_config.braking_frame_final - frame_config.braking_frame_start + 1;
    } else if (state == SpriteState::DEAD) {
        frame_start = frame_config.dead_frame_start;
        frame_count = frame_config.dead_frame_final - frame_config.dead_frame_start + 1;
    }
    setFrame(frame_start);
    start_time = SDL_GetTicks();
}

void Sprite::update() {
    unsigned int diff = SDL_GetTicks() - start_time;
    int new_frame = frame_start + (diff / TICKS_PER_FRAME) % frame_count;
    if (current_frame != new_frame) {
        //SDL_Log("frame %d", new_frame);
        setFrame(new_frame);
    }
}

void Sprite::setFrame(int frame) {
    current_frame = frame;
    int xpos = (frame % FRAMES_PER_LINE) * FRAME_WIDTH;
    int ypos = (frame / FRAMES_PER_LINE) * FRAME_HEIGHT;
    frame_rect = {xpos, ypos, FRAME_WIDTH, FRAME_HEIGHT};
}

