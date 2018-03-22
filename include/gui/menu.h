//
//  menu.h
//  A menu that is part of the GUI
//
//  Created by Vande Griek, Eric on 3/8/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef menu_h
#define menu_h

#import <vector>
#import <string>
#import <functional>
#include "SDL2.framework/Headers/SDL.h"
#import "graphics.h"
#import "resource_manager.h"

constexpr Uint8 PRESSED_RED_MOD = 200;
constexpr Uint8 PRESSED_GREEN_MOD = 200;
constexpr Uint8 PRESSED_BLUE_MOD = 200;

/**
 One element of a menu.
 Can be a button or just a static title
 */
class MenuItem {
private:
    SDL_Texture *text_texture;
    SDL_Texture *box_texture;
    std::function<void()> callback;
public:
    SDL_Rect text_rect;
    SDL_Rect box_rect;
    bool interactive;
    bool pressed;
    
    MenuItem(std::string name, int font_size, SDL_Texture *texture,
             std::function<void()> callback, bool interactive);
    void render();
    void activateButton();
    void pressButton();
    void releaseButton();
};

/**
 Holds draws and updates menu items.
 */
class Menu {
private:
    std::vector<MenuItem*> items;
    SDL_Rect rect;
    SDL_Texture *texture;
    int top_padding;
    int bottom_padding;

    void repositionItems();
public:
    Menu(SDL_Rect rect, SDL_Texture *texture);
    Menu(SDL_Rect rect, SDL_Texture *texture, int top_padding, int bottom_padding);
    void render();
    void addItem(std::string text, int font_size, SDL_Texture *texture,
                 std::function<void()> callback, bool interactive);
    bool handleClick(int xpos, int ypos, bool released);
    void releaseAll();
};

#endif /* menu_h */
