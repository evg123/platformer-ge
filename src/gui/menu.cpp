//
//  Created by Vande Griek, Eric on 3/8/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "menu.h"

// Button definitions

/**
 Create a new title or button for a Menu
 */
MenuItem::MenuItem(std::string name, int font_size, SDL_Texture *texture,
                   std::function<void()> callback, bool interactive)
: box_texture(texture), callback(callback), interactive(interactive) {
    // get button size
    int text_w, text_h;
    SDL_QueryTexture(texture, NULL, NULL, &text_w, &text_h);
    box_rect = {0, 0, text_w, text_h};
    
    // text inside the button
    text_texture = ResourceManager::instance().getTextTexture(name, font_size);
    SDL_QueryTexture(text_texture, NULL, NULL, &text_w, &text_h);
    text_rect = {0, 0, text_w, text_h};
    pressed = false;
}

/**
 Draw the item
 */
void MenuItem::render() {
    SDL_Renderer *renderer = Graphics::instance().getRenderer();
    // background can be transparent
    if (box_texture != NULL) {
        if (pressed) {
            SDL_SetTextureColorMod(box_texture,
                                   PRESSED_RED_MOD,
                                   PRESSED_GREEN_MOD,
                                   PRESSED_BLUE_MOD);
        }
        SDL_RenderCopy(renderer, box_texture, NULL, &box_rect);
        if (pressed) {
            SDL_SetTextureColorMod(box_texture, 255, 255, 255);
        }
    }
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
}

/**
 Call the callback for a button
 */
void MenuItem::activateButton() {
    if (callback != NULL) {
        callback();
    }
}

/**
 Show the button as depressed
 */
void MenuItem::pressButton() {
    pressed = true;
}

/**
 Show the button as released
 */
void MenuItem::releaseButton() {
    pressed = false;
}

// Menu definitions

/**
 Create a new Menu.
 It will not have any buttons until they are added with addItem().
 This constructor allows you to add padding to the top and/or bottom of the menu
 */
Menu::Menu(SDL_Rect rect, SDL_Texture *texture, int top_padding, int bottom_padding)
: rect(rect), texture(texture), top_padding(top_padding), bottom_padding(bottom_padding) {
}

/**
 Create a new Menu.
 It will not have any buttons until they are added with addItem().
 */
Menu::Menu(SDL_Rect rect, SDL_Texture *texture)
: Menu(rect, texture, 0, 0) {
}

/**
 Draw the Menu
 */
void Menu::render() {
    SDL_Renderer *renderer = Graphics::instance().getRenderer();
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    
    for (auto &item : items) {
        item->render();
    }
}

/**
 Add a new item or title to the menu.
 It will be added to the end of the menu and spaced based on how many
 other items are already in the menu.
 This will also re-position the other items in the menu.
 */
void Menu::addItem(std::string text, int font_size, SDL_Texture *texture,
                   std::function<void()> callback, bool interactive) {
    MenuItem *item = new MenuItem(text, font_size, texture, callback, interactive);
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
    int total_item_height = top_padding + bottom_padding;
    for (auto &item : items) {
        total_item_height += item->box_rect.h;
    }
    int padding = (rect.h - total_item_height) / (items.size() + 1);
    
    // place each item
    int cur_height = top_padding;
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

/**
 Handle a click and activate a button if it is pressed
 */
bool Menu::handleClick(int xpos, int ypos, bool released) {
    SDL_Point pnt = {xpos, ypos};

    // check if the point was within this menu
    if (!SDL_PointInRect(&pnt, &rect)) {
        return false;
    }

    for (auto item : items) {
        if (item->interactive && SDL_PointInRect(&pnt, &item->box_rect)) {
            if (released) {
                // this button was clicked
                item->activateButton();
                break;
            } else {
                // this button was depressed
                item->pressButton();
                break;
            }
        }
    }

    return true;
}

/**
 Release all buttons
 */
void Menu::releaseAll() {
    for (auto item : items) {
        if (item->interactive) {
            item->releaseButton();
        }
    }
}

