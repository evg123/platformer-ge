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

// keycode assignments
constexpr SDL_Scancode KEY_QUIT = SDL_SCANCODE_Q;
constexpr SDL_Scancode KEY_FPS_TOGGLE = SDL_SCANCODE_F;
constexpr SDL_Scancode KEY_PAUSE = SDL_SCANCODE_ESCAPE;
constexpr SDL_Scancode KEY_RIGHT_1 = SDL_SCANCODE_D;
constexpr SDL_Scancode KEY_RIGHT_2 = SDL_SCANCODE_RIGHT;
constexpr SDL_Scancode KEY_LEFT_1 = SDL_SCANCODE_A;
constexpr SDL_Scancode KEY_LEFT_2 = SDL_SCANCODE_LEFT;
constexpr SDL_Scancode KEY_JUMP = SDL_SCANCODE_SPACE;

enum class Action {
    EXIT_GAME,
    ADVACNE,
    TOGGLE_FPS,
    TOGGLE_PAUSE,
    
    MOVE_LEFT,
    STOP_LEFT,
    MOVE_RIGHT,
    STOP_RIGHT,
    JUMP,
};

/**
 Handles keyboard and mouse input from the user
 */
class Input {
private:
    Input();
    ~Input();
    
    SDL_Event event;

    std::unordered_map<Action, std::function<void()>> callbacks;
    void callAction(Action action);
    
    void handleKey(SDL_Scancode key, bool pressed);
public:
    static Input& instance();
    void handleEvents();
    void registerCallback(Action action, std::function<void()> callback);
};

#endif /* input_h */
