//
//  input.h
//  handles player input
//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef input_h
#define input_h

#ifdef __linux__
#include "../lib/linux/SDL2.framework/Headers/SDL.h"
#else // Mac
#include "../lib/osx/SDL2.framework/Headers/SDL.h"
#endif

#include <unordered_map>
#include "graphics.h"

constexpr SDL_Scancode QUIT_KEY = SDL_SCANCODE_Q;
constexpr SDL_Scancode FPS_TOGGLE_KEY = SDL_SCANCODE_F;

/**
 Handles keyboard and mouse input from the user
 */
class Input {
private:
    SDL_Event event;
    std::unordered_map<SDL_Scancode, bool> keystates;
    void setKey(SDL_Scancode key, bool pressed);
    bool should_exit = false;
    bool display_fps = false;
    bool waiting_for_click = false;
public:
    Input();
    ~Input();
    
    /** true if the user has asked to quit the game */
    bool shouldExit() { return should_exit; }
    
    /** true if the fps display has been toggled on */
    bool shouldDisplayFps() { return display_fps; }
    void handleEvents();
    int movementX();
    
    /** begin waiting for the user to click */
    void waitForClick() { waiting_for_click = true; }
    
    /** true if the user has clicked since the last time waitForClick() was called */
    bool hasClicked() { return !waiting_for_click; }
};

#endif /* input_h */
