//
//  graphics.h
//  Singleton for drawing to the screen
//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef graphics_h
#define graphics_h

#include <string>
#include <exception>
#include <tuple>
#include "SDL2.framework/Headers/SDL.h"

/**
 Singleton class for drawing to the screen
 */
class Graphics {
private:
    Graphics();
    ~Graphics();
    SDL_Window *window;
    SDL_Renderer *renderer;
    int window_width;
    int window_height;
    int screen_off_x;
    int screen_off_y;
public:
    void init(int window_width, int window_height);
    void shutdown();
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
    void clearColor(int red, int green, int blue);
    void swapFrame();
    void updateWindowTitle(std::string window_title);
    
    /** return the current world offset that the screen is showing */
    std::tuple<int, int> getScreenOffsets() { return std::make_tuple(screen_off_x, screen_off_y); }
    void focusScreenOffsets(SDL_Rect &rect);
};

#endif /* graphics_h */
