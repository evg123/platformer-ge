//
//  graphics.h
//  Singleton for drawing to the screen
//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef graphics_h
#define graphics_h

#ifdef __linux__
#include "../lib/linux/SDL2.framework/Headers/SDL.h"
#else // Mac
#include "../lib/osx/SDL2.framework/Headers/SDL.h"
#endif

#include <string>
#include <exception>
#include <tuple>

/**
 Singleton class for drawing to the screen
 */
class Graphics {
private:
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    int window_width;
    int window_height;
public:
    Graphics();
    ~Graphics();
    void init(int window_width, int window_height);
    static Graphics& instance();
    
    /** return the SDL_Window object */
    SDL_Window* getWindow() { return window; }
    
    /** return the SDL_Renderer object */
    SDL_Renderer* getRenderer() { return renderer; }
    
    /** return the width of the SDL window */
    int getWindowWidth() { return window_width; }
    
    /** return the width of the SDL window */
    int getWindowHeight() { return window_height; }
    
    void clear();
    void swapFrame();
    void updateWindowTitle(std::string window_title);
};

#endif /* graphics_h */
