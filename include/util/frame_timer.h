//
//  FrameTimer.h
//  Timer class that tracks and caps fps
//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef frame_timer_h
#define frame_timer_h

#include "SDL.h"

// limit maximum time between frames
// game will slow down past this point
constexpr int MAX_DELTA = 64;

constexpr float FPS_UPDATE_INTERVAL_MS = 1000.0f;

/**
 Used to track frames per second and cap the framerate
 */
class FrameTimer {
private:
    int fps_target;
    long frame_start;
    long fps_update_start;
    int frame_count;
    int fps;
public:
    void init(int fps_target);

    /** Return the last recorded fps count */
    int getFps() { return fps; };
    /** Return the timestamp of the current frame */
    long getFrameStart() { return frame_start; };

    int newFrame();
    void delayUntilNextFrame();
};

#endif /* frame_timer_h */
