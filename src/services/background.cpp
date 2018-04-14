//
//  Created by Vande Griek, Eric on 3/19/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "background.h"

// BgLayer definitions

/**
 Create a new layer of the background
 */
BgLayer::BgLayer(SDL_Texture *texture, int width, int height, int distance,
                 int screen_w, int screen_h, int lower_bound)
: texture(texture), distance(distance), screen_w(screen_w) {
    rect.x = 0;
    rect.y = lower_bound - screen_h;
    rect.w = width;
    rect.h = height;
}

/**
 Update the position of the layer based on the center of the screen
 */
void BgLayer::update(int screen_off_x, int screen_off_y) {
    int adjusted_off_x = screen_off_x * (1.0f / distance);
    rect.x = adjusted_off_x % screen_w - screen_w;
    rect.y = screen_off_y * (1.0f / distance);
}

/**
 Draw this layer at its location.
 Draw the layer all the way accross the screen, repeating if needed
 */
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

/**
 Set up the background
 */
void Background::init(int start_x, int start_y, int lower_bound) {
    clearLayers();
    start_x = -1;
    start_y = -1;
    setLowerBound(lower_bound);
}

void Background::setLowerBound(int lower_bound) {
    this->lower_bound = lower_bound - 200;
}

/**
 Clean up things allocated by the background
 */
void Background::shutdown() {
    clearLayers();
}

/**
 Set the backdrop color that is filled in behind the layers
 */
void Background::setColor(int red, int green, int blue) {
    bg_red = red;
    bg_green = green;
    bg_blue = blue;
}

/**
 Add a new layer that is distance away
 */
void Background::addLayer(std::string img_file, int width, int height, int distance) {
    int sw = Graphics::instance().getWindowWidth();
    int sh = Graphics::instance().getWindowHeight();
    SDL_Texture *tx = ResourceManager::instance().getImageTexture(img_file);
    BgLayer *bgl = new BgLayer(tx, width, height, distance, sw, sh, lower_bound);
    layers.push_back(bgl);

    // sort the layers so they are drawn back-to-front
    std::sort(layers.begin(), layers.end(), bgLayerComparator);
}

/**
 Update each layer of the background based on the new center of the screen
 */
void Background::updateLayerOffsets(int center_x, int center_y) {
    if (start_x == -1) {
        // first update, use as the starting point
        start_x = center_x;
        start_y = center_y;
        return;
    }
    int sox = start_x - center_x;
    int soy = start_y - center_y;
    for (auto bgl : layers) {
        bgl->update(sox, soy);
    }
}

/**
 Fill in the backdrop color and draw each layer of the bakground
 */
void Background::render() {
    // fill the background color
    Graphics::instance().clearColor(bg_red, bg_green, bg_blue);
    
    // draw the layers
    for (auto bgl : layers) {
        bgl->render();
    }
}

/**
 Delete each layer
 */
void Background::clearLayers() {
    for (auto bgl : layers) {
        delete bgl;
    }
    layers.clear();
}
