//
//  Created by Vande Griek, Eric on 3/9/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "gui_element.h"

// GuiElement definitions

/**
 Create a new GuiElement at rect to display texture
 */
GuiElement::GuiElement(SDL_Rect rect, SDL_Texture *texture)
: rect(rect), texture(texture) {
}

/**
 Draw the element
 */
void GuiElement::render() {
    SDL_Renderer *renderer = Graphics::instance().getRenderer();
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

// TextGuiElement definitions

/**
 Create a new static TextGuiElement.
 Can be a string, int, or float.
 */
template<typename T>
TextGuiElement<T>::TextGuiElement(SDL_Rect rect, T &value, int font_size)
: TextGuiElement(rect, value, font_size, false) {}

/**
 Create a new TextGuiElement.
 Can be a string, int, or float.
 If dynamic is true it will be updated to match value whenever value is changed
 */
template<typename T>
TextGuiElement<T>::TextGuiElement(SDL_Rect rect, T &value, int font_size, bool dynamic)
: GuiElement(rect, NULL), value(value), font_size(font_size), dynamic(dynamic) {
    renderText();
}

/**
 Get a string for the value
 */
template<typename T>
std::string TextGuiElement<T>::getValueString() {
    return std::to_string(value);
}

/**
 Get a string for the value.
 Special case for string.
 */
template<>
std::string TextGuiElement<std::string>::getValueString() {
    return value;
}

/**
 Update the element if it is dynamic
 */
template<typename T>
void TextGuiElement<T>::update() {
    if (dynamic && value != rendered_value) {
        renderText();
    }
}

/**
 Get a texture for the current value of value
 */
template<typename T>
void TextGuiElement<T>::renderText() {
    texture = ResourceManager::instance().getTextTexture(getValueString(), font_size);
    int tex_w, tex_h;
    SDL_QueryTexture(texture, NULL, NULL, &tex_w, &tex_h);
    rect.w = tex_w;
    rect.h = tex_h;
    rendered_value = value;
}

// declare possible template types here
template class TextGuiElement<std::string>;
template class TextGuiElement<int>;
template class TextGuiElement<float>;
