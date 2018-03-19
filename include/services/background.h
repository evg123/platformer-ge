//
//  background.h
//  platformer
//
//  Created by Vande Griek, Eric on 3/19/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef background_h
#define background_h

#ifdef __linux__
#include "../lib/linux/SDL2.framework/Headers/SDL.h"
#else // Mac
#include "../lib/osx/SDL2.framework/Headers/SDL.h"
#endif

#include <vector>
#include <string>
#include <algorithm>
#include "graphics.h"
#include "resource_manager.h"

class BgLayer {
private:
    SDL_Texture *texture;
    int distance;
    SDL_Rect rect;
    int screen_w;
    int screen_h;
public:
    BgLayer(SDL_Texture *texture, int distance, int screen_w, int screen_h);
    void update(int screen_off_x, int screen_off_y);
    void render();
    /** comparison operator to sort layers descending by distance */
    bool operator<(const BgLayer &rhs) const { return distance > rhs.distance; }
};

class Background {
private:
    std::vector<BgLayer*> layers;
    int start_x;
    int start_y;
public:
    void init(int start_x, int start_y);
    void addLayer(std::string img_file, int distance);
    void update(int center_x, int center_y);
    void render();
};

#endif /* background_h */
