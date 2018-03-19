//
//  menu.h
//  platformer
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
    std::vector<MenuItem*> items;
    SDL_Rect rect;
    SDL_Texture *texture;

    void repositionItems();
public:
    Menu(SDL_Rect rect, SDL_Texture *texture);
    void render();
    void addItem(std::string text, int font_size, SDL_Rect btn_size, SDL_Texture *texture, std::function<void()> callback);
    bool handleClick(int xpos, int ypos);
};

#endif /* menu_h */
