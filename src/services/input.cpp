//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "input.h"

/**
 Constructor not used, things are set up in init()
 */
Input::Input() {}

/**
 Private destructor
 */
Input::~Input() {}

/**
 Get the singleton instance
 */
Input& Input::instance() {
    static Input *instance = new Input();
    return *instance;
}

/**
 Setup
 */
void Input::init() {
    SDL_StartTextInput();
}

/**
 Teardown
 */
void Input::shutdown() {
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
                callAction(Action::EXIT_GAME);
                break;
            case SDL_KEYDOWN:
                handleKey(event.key.keysym.scancode, true);
                break;
            case SDL_KEYUP:
                handleKey(event.key.keysym.scancode, false);
                break;
            case SDL_MOUSEBUTTONDOWN:
                handleClick(event.button.x, event.button.y, false);
                break;
            case SDL_MOUSEBUTTONUP:
                handleClick(event.button.x, event.button.y, true);
                break;
            default:
                break;
        }
    }
}

/**
 Add a callback to be called when action occurs
 */
void Input::registerCallback(Action action, std::function<void()> callback) {
    callbacks[action] = callback;
}

/**
 Do the callback associated with action
 */
void Input::callAction(Action action) {
    callbacks[action]();
}

/**
 Mark a key as being pressed or released
 */
void Input::handleKey(SDL_Scancode key, bool pressed) {
    // toggle fps display
    if (!pressed && key == KEY_FPS_TOGGLE) {
        callAction(Action::TOGGLE_FPS);
    }
    else if (!pressed && key == KEY_PAUSE) {
        callAction(Action::TOGGLE_PAUSE);
    }
    else if (!pressed && key == KEY_QUIT) {
        callAction(Action::EXIT_GAME);
    }
    else if (key == KEY_LEFT_1 || key == KEY_LEFT_2) {
        if (pressed) {
            callAction(Action::MOVE_LEFT);
        } else {
            callAction(Action::STOP_LEFT);
        }
    }
    else if (key == KEY_RIGHT_1 || key == KEY_RIGHT_2) {
        if (pressed) {
            callAction(Action::MOVE_RIGHT);
        } else {
            callAction(Action::STOP_RIGHT);
        }
    }
    else if (pressed && key == KEY_JUMP) {
        callAction(Action::JUMP);
    }
}

/**
 Pass a click to the GUI
 */
void Input::handleClick(int xpos, int ypos, bool released) {
    bool handled = Gui::instance().handleClick(xpos, ypos, released);
    if (!handled) {
        // nothing was clicked on, interperet as an advance action
        callAction(Action::ADVACNE);
    }
}

