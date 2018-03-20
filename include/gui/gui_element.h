//
//  gui_element.h
//  platformer
//
//  Created by Vande Griek, Eric on 3/9/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef gui_element_h
#define gui_element_h

#include <string>
#include "SDL2.framework/Headers/SDL.h"
#include "graphics.h"
#include "resource_manager.h"

class GuiElement {
protected:
    SDL_Texture *texture;
    SDL_Rect rect;
public:
    GuiElement(SDL_Rect rect, SDL_Texture *texture);
    void render();
    virtual void update() {};
    int getWidth() { return rect.w; }
    int getHeight() { return rect.h; }
};

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
