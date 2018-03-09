//
//  menu.h
//  platformer
//
//  Created by Vande Griek, Eric on 3/8/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef menu_h
#define menu_h

#ifdef __linux__
#include "../lib/linux/SDL2.framework/Headers/SDL.h"
#else // Mac
#include "../lib/osx/SDL2.framework/Headers/SDL.h"
#endif

#import <vector>
#import <string>
#import <functional>
#import "graphics.h"
#import "resource_manager.h"

/**
 Identifiers used to distinguish between menus
 Not all of these are implemented
 */
enum class MenuId {
    MAIN,
    PAUSE,
    OPTIONS,
};

class MenuItem {
private:
    SDL_Texture *text_texture;
    SDL_Texture *box_texture;
    std::function<void()> callback;
public:
    SDL_Rect text_rect;
    SDL_Rect box_rect;
    
    MenuItem(std::string name, int font_size, SDL_Rect rect, SDL_Texture *texture, std::function<void()> callback);
    void render();
    void pressButton();
};

class Menu {
private:
    MenuId id;
    std::vector<MenuItem*> items;
    SDL_Rect rect;
    SDL_Texture *texture;
    bool display = true;

    void repositionItems();
public:
    Menu(MenuId id, SDL_Rect rect, SDL_Texture *texture);
    void render();
    MenuId getId() { return id; }
    void addItem(std::string text, int font_size, SDL_Rect btn_size, SDL_Texture *texture, std::function<void()> callback);
    void toggleDisplay();
    bool handleClick(int xpos, int ypos);
};

#endif /* menu_h */
