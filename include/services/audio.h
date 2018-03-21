//
//  audio.h
//  Singleton audio manager
//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef audio_h
#define audio_h

#include <string>
#include <map>
#include "SDL2.framework/Headers/SDL.h"
#include "SDL2_mixer.framework/Headers/SDL_mixer.h"
#include "resource_manager.h"

/**
 Singleton class for playing music and sound effects
 */
class Audio {
private:
    Audio();
    ~Audio();
    Mix_Music *bg_track;
    std::map<std::string, Uint8> last_played_map;

    void setPlayed(const std::string &track_name);
public:
    static Audio& instance();
    void init();
    void shutdown();
    void setBgTrack(const std::string &track_name);
    void playSound(const std::string &sound_name);
    Uint8 getLastPlayed(const std::string &sound_name);
};

#endif /* audio_h */
