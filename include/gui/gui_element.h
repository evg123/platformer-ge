//
//  gui_element.h
//  A single text or image displayed in the GUI
//
//  Created by Vande Griek, Eric on 3/9/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef gui_element_h
#define gui_element_h

#include <string>
#include "SDL.h"
#include "graphics.h"
#include "resource_manager.h"

/**
 Represents an texture that gets drawn as part of the GUI
 */
class GuiElement {
protected:
    SDL_Texture *texture;
    SDL_Rect rect;
public:
    GuiElement(SDL_Rect rect, SDL_Texture *texture);
    void render();
    /** See if anything needs to change. Base class does nothing */
    virtual void update() {};
    /** return the width of this element */
    int getWidth() { return rect.w; }
    /** return the height of this element */
    int getHeight() { return rect.h; }
};

/**
 Specific GuiElement for displaying text.
 Can be dynamic by holding a reference to a string or value
 */
template<typename T>
class TextGuiElement : public GuiElement {
private:
    T &value;
    T rendered_value;
    int font_size;
    bool dynamic;
    
    std::string getValueString();
public:
    TextGuiElement(SDL_Rect rect, T &value, int font_size);
    TextGuiElement(SDL_Rect rect, T &value, int font_size, bool dynamic);
    void update() override;
    void renderText();
};


#endif /* gui_element_h */
