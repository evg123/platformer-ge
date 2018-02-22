//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "../include/input.h"

/**
 Setup
 */
Input::Input() {
    SDL_StartTextInput();
}

/**
 Teardown
 */
Input::~Input() {
    SDL_StopTextInput();
}

/**
 Check for events from SDL.
 This includes keyboard, mouse, and SDL window changes
 */
void Input::handleEvents() {
    while(SDL_PollEvent(&event) != 0) {
        switch (event.type) {
            case SDL_QUIT:
                // user closed window
                should_exit = true;
                break;
            case SDL_KEYDOWN:
                setKey(event.key.keysym.scancode, true);
                break;
            case SDL_KEYUP:
                setKey(event.key.keysym.scancode, false);
                break;
            case SDL_MOUSEBUTTONUP:
                waiting_for_click = false;
                break;
            default:
                break;
        }
    }
}

/**
 Mark a key as being pressed or released
 */
void Input::setKey(SDL_Scancode key, bool pressed) {
    keystates[key] = pressed;
    
    // toggle fps display
    if (!pressed && key == FPS_TOGGLE_KEY) {
        display_fps = !display_fps;
    } else if (!pressed && key == QUIT_KEY) {
        should_exit = true;
    }
    
}

/**
 Figure out which direction the player wants to move based on keypresses
 */
int Input::movementX() {
    int movex = 0;
    if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A]) {
        movex -= 1;
    }
    if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]) {
        movex += 1;
    }
    return movex;
}
