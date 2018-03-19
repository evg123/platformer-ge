//
//  background.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 3/19/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "background.h"

// BgLayer definitions

BgLayer::BgLayer(SDL_Texture *texture, int distance, int screen_w, int screen_h)
: texture(texture), distance(distance) {
    rect.x = 0;
    rect.y = 0;
    SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
}

void BgLayer::update(int screen_off_x, int screen_off_y) {
    int adjusted_off_x = screen_off_x * (1.0f / distance);
    rect.x = adjusted_off_x % screen_w - screen_w;
    rect.y = screen_off_y * (1.0f / distance);
}

void BgLayer::render() {
    SDL_Renderer *renderer = Graphics::instance().getRenderer();
    SDL_Rect tiled_rect(rect);
    while (tiled_rect.x + tiled_rect.w < screen_w) {
        SDL_RenderCopy(renderer, texture, NULL, &tiled_rect);
        tiled_rect.x += tiled_rect.w;
    }
}

// Background definitions

void Background::init(int start_x, int start_y) {
    this->start_x = start_x;
    this->start_y = start_y;
}

void Background::addLayer(std::string img_file, int distance) {
    int sw = Graphics::instance().getWindowWidth();
    int sh = Graphics::instance().getWindowHeight();
    SDL_Texture *tx = ResourceManager::instance().getImageTexture(img_file);
    BgLayer *bgl = new BgLayer(tx, distance, sw, sh);
    layers.push_back(bgl);

    // sort the layers so they are drawn back-to-front
    std::sort(layers.begin(), layers.end());
}

void Background::update(int center_x, int center_y) {
    int sox = start_x - center_x;
    int soy = start_y - center_y;
    for (auto bgl : layers) {
        bgl->update(sox, soy);
    }
}

void Background::render() {
    for (auto bgl : layers) {
        bgl->render();
    }
}