//
//  sprite.h
//  platformer
//
//  Created by Vande Griek, Eric on 3/6/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef sprite_h
#define sprite_h

#ifdef __linux__
#include "../lib/linux/SDL2.framework/Headers/SDL.h"
#else // Mac
#include "../lib/osx/SDL2.framework/Headers/SDL.h"
#endif

constexpr int FRAME_WIDTH = 32;
constexpr int FRAME_HEIGHT = 32;
constexpr int FRAMES_PER_LINE = 5;

constexpr int TICKS_PER_FRAME = 250;

enum class SpriteState {
    NONE,
    IDLE,
    WALKING,
    JUMPING,
};

struct FrameConfig {
    int idle_frame_start;
    int idle_frame_final;
    int walking_frame_start;
    int walking_frame_final;
    int jumping_frame_start;
    int jumping_frame_final;
};

class Sprite {
private:
    SpriteState current_state = SpriteState::NONE;
    bool walking = false; //TODO probably dont need this
    int current_frame;
    int frame_start; // starting frame number in the cycle
    int frame_count; // number of frames in the cycle
    unsigned int start_time;
    SDL_Rect frame_rect;
    FrameConfig frame_config;
    
    void setFrame(int frame);
public:
    void init(FrameConfig frame_config);
    SDL_Rect& getFrameRect() { return frame_rect; }
    void setState(SpriteState state);
    void update();
    
    void move();
    void stop();
    void jump();
    void land();
};

#endif /* sprite_h */
