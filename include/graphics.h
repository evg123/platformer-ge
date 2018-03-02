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
    Graphics();
    ~Graphics();
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    int window_width;
    int window_height;
    int screen_off_x = 0;
    int screen_off_y = 0;
public:
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
    
    /** return the current world offset that the screen is showing */
    std::tuple<int, int> getScreenOffsets() { return std::make_tuple(screen_off_x, screen_off_y); }
    void focusScreenOffsets(SDL_Rect &rect);
};

#endif /* graphics_h */
