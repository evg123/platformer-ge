//
//  player.h
//  platformer
//
//  Created by Vande Griek, Eric on 3/1/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef player_h
#define player_h

#ifdef __linux__
#include "../lib/linux/SDL2.framework/Headers/SDL.h"
#else // Mac
#include "../lib/osx/SDL2.framework/Headers/SDL.h"
#endif

#include <vector>
#include "drawable.h"
#include "graphics.h"

constexpr auto PLAYER_SPRITE_SHEET = "sprites/player.png";

class Player : public Drawable {
private:
public:
    Player();
    void render();
};

#endif /* player_h */
