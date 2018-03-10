//
//  gui_element.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 3/9/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "../include/gui_element.h"

// GuiElement definitions

GuiElement::GuiElement(SDL_Rect rect, SDL_Texture *texture)
: rect(rect), texture(texture) {
}

void GuiElement::render() {
    SDL_Renderer *renderer = Graphics::instance().getRenderer();
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

// TextGuiElement definitions

template<typename T>
TextGuiElement<T>::TextGuiElement(SDL_Rect rect, T &value, int font_size)
: GuiElement(rect, NULL), value(value), font_size(font_size) {
    renderText();
}

template<typename T>
std::string TextGuiElement<T>::getValueString() {
    return std::to_string(value);
}

// special case for string
template<>
std::string TextGuiElement<std::string>::getValueString() {
    return value;
}


template<typename T>
void TextGuiElement<T>::update() {
    if (value != rendered_value) {
        renderText();
    }
}

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
