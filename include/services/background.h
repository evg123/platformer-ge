//
//  background.h
//  Classes related to the background of the game
//
//  Created by Vande Griek, Eric on 3/19/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef background_h
#define background_h

#include <vector>
#include <string>
#include <algorithm>
#include "SDL.h"
#include "graphics.h"
#include "resource_manager.h"

/**
 One layer of a parallax background
 */
class BgLayer {
private:
    SDL_Texture *texture;
    int distance;
    SDL_Rect rect;
    int screen_w;
public:
    BgLayer(SDL_Texture *texture, int width, int height, int distance,
            int screen_w, int screen_h, int lower_bound);
    void update(int screen_off_x, int screen_off_y);
    void render();
    /** Get the distance away from the foreground */
    int getDistance() { return distance; }
};

/** comparison operator to sort layers descending by distance */
bool bgLayerComparator(BgLayer *lhs, BgLayer *rhs);

/**
 Draws the background in independently moving layers
 */
class Background {
private:
    std::vector<BgLayer*> layers;
    int start_x;
    int start_y;
    int lower_bound;
    int bg_red;
    int bg_green;
    int bg_blue;
public:
    void init(int start_x, int start_y, int lower_bound);
    void shutdown();
    void setColor(int red, int green, int blue);
    void setLowerBound(int lower_bound);
    void addLayer(std::string img_file, int width, int height, int distance);
    void updateLayerOffsets(int center_x, int center_y);
    void render();
    void clearLayers();
};

#endif /* background_h */
