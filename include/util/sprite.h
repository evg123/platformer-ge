//
//  sprite.h
//  Code related to sprites
//
//  Created by Vande Griek, Eric on 3/6/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef sprite_h
#define sprite_h

#include "SDL.h"

constexpr int FRAME_WIDTH = 16;
constexpr int FRAME_HEIGHT = 16;
constexpr int FRAMES_PER_LINE = 5;

constexpr int TICKS_PER_FRAME = 120;

/**
 The list of states that a sprite can be in
 */
enum class SpriteState {
    NONE,
    IDLE,
    WALKING,
    JUMPING,
    BRAKING,
    DEAD,
};

/**
 Struct that holds indexes for mapping sprite states onto a sprite sheet
 */
struct FrameConfig {
    int idle_frame_start;
    int idle_frame_final;
    int walking_frame_start;
    int walking_frame_final;
    int jumping_frame_start;
    int jumping_frame_final;
    int braking_frame_start;
    int braking_frame_final;
    int dead_frame_start;
    int dead_frame_final;
};

/**
 Represents a sprite and manages its transition between states
 */
class Sprite {
private:
    SpriteState current_state = SpriteState::NONE;
    int current_frame;
    int frame_start; // starting frame number in the cycle
    int frame_count; // number of frames in the cycle
    unsigned int start_time;
    SDL_Rect frame_rect;
    FrameConfig frame_config;
    
    void setFrame(int frame);
public:
    void init(FrameConfig frame_config);
    /** Get the current source rect from the sprite sheet */
    SDL_Rect& getFrameRect() { return frame_rect; }
    void setState(SpriteState state);
    void update();

    /** set the sprite to the walking state */
    void setWalking() { setState(SpriteState::WALKING); }
    /** set the sprite to the idle state */
    void setIdle() { setState(SpriteState::IDLE); }
    /** set the sprite to the jumping state */
    void setJumping() { setState(SpriteState::JUMPING); }
    /** set the sprite to the braking state */
    void setBraking() { setState(SpriteState::BRAKING); }
    /** set the sprite to the dead state */
    void setDead() { setState(SpriteState::DEAD); }
};

#endif /* sprite_h */
