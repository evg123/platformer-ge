//
//  background.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 3/19/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "background.h"

// BgLayer definitions

BgLayer::BgLayer(SDL_Texture *texture, int width, int height, int distance,
                 int screen_w, int screen_h, int lower_bound)
: texture(texture), distance(distance), screen_w(screen_w) {
    rect.x = 0;
    rect.y = lower_bound - screen_h;
    rect.w = width;
    rect.h = height;
}

void BgLayer::update(int screen_off_x, int screen_off_y) {
    int adjusted_off_x = screen_off_x * (1.0f / distance);
    rect.x = adjusted_off_x % screen_w - screen_w;
    rect.y = screen_off_y * (1.0f / distance);
}

void BgLayer::render() {
    SDL_Renderer *renderer = Graphics::instance().getRenderer();
    SDL_Rect tiled_rect(rect);
    while (tiled_rect.x <= screen_w) {
        SDL_RenderCopy(renderer, texture, NULL, &tiled_rect);
        tiled_rect.x += tiled_rect.w;
    }
}

/**
 comparison operator to sort layers descending by distance
 */
bool bgLayerComparator(BgLayer *lhs, BgLayer *rhs) {
    return lhs->getDistance() > rhs->getDistance();
}

// Background definitions

void Background::init(int start_x, int start_y, int lower_bound) {
    clearLayers();
    this->lower_bound = lower_bound;
    this->start_x = start_x;
    this->start_y = start_y;
}

void Background::shutdown() {
    clearLayers();
}

void Background::setColor(int red, int green, int blue) {
    bg_red = red;
    bg_green = green;
    bg_blue = blue;
}

void Background::addLayer(std::string img_file, int width, int height, int distance) {
    int sw = Graphics::instance().getWindowWidth();
    int sh = Graphics::instance().getWindowHeight();
    SDL_Texture *tx = ResourceManager::instance().getImageTexture(img_file);
    BgLayer *bgl = new BgLayer(tx, width, height, distance, sw, sh, lower_bound);
    layers.push_back(bgl);

    // sort the layers so they are drawn back-to-front
    std::sort(layers.begin(), layers.end(), bgLayerComparator);
}

void Background::updateLayerOffsets(int center_x, int center_y) {
    int sox = start_x - center_x;
    int soy = start_y - center_y;
    for (auto bgl : layers) {
        bgl->update(sox, soy);
    }
}

void Background::render() {
    // fill the background color
    Graphics::instance().clearColor(bg_red, bg_green, bg_blue);
    
    // draw the layers
    for (auto bgl : layers) {
        bgl->render();
    }
}

void Background::clearLayers() {
    for (auto bgl : layers) {
        delete bgl;
    }
    layers.clear();
}
