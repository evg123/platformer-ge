//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "graphics.h"

/**
 Constructor not used, things are set up in init()
 */
Graphics::Graphics() {}

/**
 Private destructor
 */
Graphics::~Graphics() {}

/**
 Get the instance of the Graphics class
 */
Graphics& Graphics::instance() {
    static Graphics *instance = new Graphics();
    return *instance;
}

/**
 Set up a window and renderer.
 Must be called before use.
 */
void Graphics::init(int window_width, int window_height) {
    screen_off_x = 0;
    screen_off_y = 0;
    this->window_width = window_width;
    this->window_height = window_height;
    
    window = SDL_CreateWindow("",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              window_width,
                              window_height,
                              SDL_WINDOW_SHOWN);
    if (window == NULL) {
        throw std::runtime_error("Failed to create SDL window");
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        throw std::runtime_error("Failed to create SDL renderer");
    }
}

/**
 free the resources used for rendering
 */
void Graphics::shutdown() {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
}

/**
 Wipe the frame so we can start drawing the next one
 */
void Graphics::clear() {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
}

/**
 Wipe the frame so we can start drawing the next one
 */
void Graphics::clearColor(int red, int green, int blue) {
    SDL_SetRenderDrawColor(renderer, red, green, blue, 0xFF);
    SDL_RenderClear(renderer);
}


/**
 Replace the screen with the frame that has just been drawn
 */
void Graphics::swapFrame() {
    SDL_GL_SwapWindow(window);
}

/**
 Update the title of the SDL window
 */
void Graphics::updateWindowTitle(std::string window_title) {
    SDL_SetWindowTitle(window, window_title.c_str());
}

/** set the world offset that the screen is showing */
void Graphics::focusScreenOffsets(const SDL_Rect &rect) {
    screen_off_x = rect.x + (rect.w / 2) - (window_width / 2);
    // keep the player towards the bottom of the screen
    screen_off_y = rect.y + (rect.h / 2) - ((3 * window_height) / 4);
}
