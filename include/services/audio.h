//
//  audio.h
//  Singleton audio manager
//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef audio_h
#define audio_h

#ifdef __linux__
#include "../lib/linux/SDL2.framework/Headers/SDL.h"
#include "../lib/linux/SDL2_mixer.framework/Headers/SDL_mixer.h"
#else // Mac
#include "../lib/osx/SDL2.framework/Headers/SDL.h"
#include "../lib/osx/SDL2_mixer.framework/Headers/SDL_mixer.h"
#endif

#include <string>
#include "resource_manager.h"

/**
 Singleton class for playing music and sound effects
 */
class Audio {
private:
    Audio();
    ~Audio();
    Mix_Music *bg_track = NULL;
public:
    static Audio& instance();
    void setBgTrack(const std::string &track_name);
    void playSound(const std::string &sound_name);
};

#endif /* audio_h */
