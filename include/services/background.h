//
//  background.h
//  platformer
//
//  Created by Vande Griek, Eric on 3/19/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef background_h
#define background_h

#include <vector>
#include <string>
#include <algorithm>
#include "SDL2.framework/Headers/SDL.h"
#include "graphics.h"
#include "resource_manager.h"

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
    int getDistance() { return distance; }
};

/** comparison operator to sort layers descending by distance */
bool bgLayerComparator(BgLayer *lhs, BgLayer *rhs);

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
    void addLayer(std::string img_file, int width, int height, int distance);
    void updateLayerOffsets(int center_x, int center_y);
    void render();
    void clearLayers();
};

#endif /* background_h */
