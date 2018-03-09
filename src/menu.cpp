//
//  menu.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 3/8/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "../include/menu.h"

// Button definitions

MenuItem::MenuItem(std::string name, int font_size, SDL_Rect rect, SDL_Texture *texture, std::function<void()> callback)
: box_rect(rect), box_texture(texture), callback(callback) {
    text_texture = ResourceManager::instance().getTextTexture(name, font_size);
    int text_w, text_h;
    SDL_QueryTexture(text_texture, NULL, NULL, &text_w, &text_h);
    text_rect = {0, 0, text_w, text_h};
}

void MenuItem::render() {
    SDL_Renderer *renderer = Graphics::instance().getRenderer();
    // background can be transparent
    if (box_texture != NULL) {
        SDL_RenderCopy(renderer, box_texture, NULL, &box_rect);
    }
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
}

void MenuItem::pressButton() {
    if (callback != NULL) {
        callback();
    }
}

// Menu definitions

Menu::Menu(MenuId id, SDL_Rect rect, SDL_Texture *texture)
: id(id), rect(rect), texture(texture) {
}

void Menu::render() {
    if (!display) {
        // we are hidden
        return;
    }
    
    SDL_Renderer *renderer = Graphics::instance().getRenderer();
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    
    for (auto &item : items) {
        item->render();
    }
}

void Menu::addItem(std::string text, int font_size, SDL_Rect btn_size, SDL_Texture *texture, std::function<void()> callback) {
    MenuItem *item = new MenuItem(text, font_size, btn_size, texture, callback);
    items.push_back(item);
    
    // if we wanted to be really efficient we would just call this once after adding all the items
    repositionItems();
}

/**
 Recalculate the position of each item in the menu
 Needs to be called when a new item is added
 */
void Menu::repositionItems() {
    // first we need to calculate padding
    int total_item_height = 0;
    for (auto &item : items) {
        total_item_height += item->box_rect.h;
    }
    int padding = (rect.h - total_item_height) / (items.size() + 1);
    
    // place each item
    int cur_height = 0;
    for (int num = 0; num < items.size(); ++num) {
        MenuItem *item = items[num];

        // place the item at the end of the list based on the other items already placed
        item->box_rect.x = rect.x + ((rect.w - item->box_rect.w) / 2);
        item->box_rect.y = rect.y + cur_height + padding;
        item->text_rect.x = item->box_rect.x + ((item->box_rect.w - item->text_rect.w) / 2);
        item->text_rect.y = item->box_rect.y + ((item->box_rect.h - item->text_rect.h) / 2);

        // keep track of how tall we've gotten
        cur_height += item->box_rect.h + padding;
    }
}

void Menu::toggleDisplay() {
    display = !display;
}

bool Menu::handleClick(int xpos, int ypos) {
    if (!display) {
        // we are hidden
        return false;
    }

    SDL_Point pnt = {xpos, ypos};
    for (auto item : items) {
        if (SDL_PointInRect(&pnt, &item->box_rect)) {
            item->pressButton();
            break;
        }
    }

    // return true if the point was within this menu
    return SDL_PointInRect(&pnt, &rect);
}

