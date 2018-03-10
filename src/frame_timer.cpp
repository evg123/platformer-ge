//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "../include/frame_timer.h"

/**
 Create a new timer.
 The framerate will be capped at fps_target
 */
FrameTimer::FrameTimer(int fps_target) {
    this->fps_target = fps_target;
    frame_start = SDL_GetTicks();
    fps_update_start = SDL_GetTicks();
    frame_count = 0;
    fps = 0;
}

/**
 Mark the start of a new frame for timing purposes
 */
int FrameTimer::newFrame() {
    int now = SDL_GetTicks();
    int delta = now - frame_start;
    frame_start = now;

    // limit delta
    if (delta > MAX_DELTA) {
        SDL_Log("WARNING: limiting frame delta of %d ms to %d ms", delta, MAX_DELTA);
        delta = MAX_DELTA;
    }

    ++frame_count;
    if (now - fps_update_start > FPS_UPDATE_INTERVAL_MS) {
        fps = frame_count;
        frame_count = 0;
        fps_update_start = now;
    }

    return delta;
}

/**
 Delay until it is time to draw the next frame
 */
void FrameTimer::delayUntilNextFrame() {
    float delay_time = (fps_update_start + (frame_count + 1) * (1000.0f / fps_target)) - SDL_GetTicks();
    if (delay_time > 0) {
        SDL_Delay(delay_time);
    }
}
