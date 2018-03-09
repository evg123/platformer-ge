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
                callAction(Action::EXIT_GAME);
                break;
            case SDL_KEYDOWN:
                handleKey(event.key.keysym.scancode, true);
                break;
            case SDL_KEYUP:
                handleKey(event.key.keysym.scancode, false);
                break;
            case SDL_MOUSEBUTTONUP:
                handleClick(event.button.x, event.button.y);
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

void Input::addMenu(Menu *menu) {
    menus[menu->getId()] = menu;
}

void Input::handleClick(int xpos, int ypos) {
    for (auto &menu : menus) {
        if (menu.second->handleClick(xpos, ypos)) {
            // this menu handled the click, we can stop
            return;
        }
    }

    // nothing was clicked on, interperet as an advance action
    callAction(Action::ADVACNE);
}

void Input::renderMenus() {
    for (auto &menu : menus) {
        menu.second->render();
    }
}

void Input::toggleMenuDisplay(MenuId menu_id) {
    menus[menu_id]->toggleDisplay();
}

