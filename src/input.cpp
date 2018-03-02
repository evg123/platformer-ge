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
 Get the singleton instance
 */
Input& Input::instance() {
    static Input *instance = new Input();
    return *instance;
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
                callAction(EXIT_GAME);
                break;
            case SDL_KEYDOWN:
                handleKey(event.key.keysym.scancode, true);
                break;
            case SDL_KEYUP:
                handleKey(event.key.keysym.scancode, false);
                break;
            case SDL_MOUSEBUTTONUP:
                callAction(ADVACNE);
                break;
            default:
                break;
        }
    }
}

void Input::registerCallback(Action action, std::function<void()> callback) {
    callbacks[action] = callback;
}

void Input::callAction(Action action) {
    callbacks[action]();
}

/**
 Mark a key as being pressed or released
 */
void Input::handleKey(SDL_Scancode key, bool pressed) {
    // toggle fps display
    if (!pressed && key == KEY_FPS_TOGGLE) {
        callAction(TOGGLE_FPS);
    }
    else if (!pressed && key == KEY_QUIT) {
        callAction(EXIT_GAME);
    }
    else if (key == KEY_LEFT_1 || key == KEY_LEFT_2) {
        if (pressed) {
            callAction(MOVE_LEFT);
        } else {
            callAction(STOP_LEFT);
        }
    }
    else if (key == KEY_RIGHT_1 || key == KEY_RIGHT_2) {
        if (pressed) {
            callAction(MOVE_RIGHT);
        } else {
            callAction(STOP_RIGHT);
        }
    }
    else if (!pressed && key == KEY_JUMP) {
        callAction(JUMP);
    }
}
