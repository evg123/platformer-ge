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
#include "SDL.h"
#include "SDL_mixer.h"
#include "resource_manager.h"
#include "frame_timer.h"

/**
 Singleton class for playing music and sound effects
 */
class Audio {
private:
    Audio();
    ~Audio();
    Mix_Music *bg_track;
    std::map<std::string, long> last_played_map;
    bool enabled;

    void setPlayed(const std::string &track_name);
public:
    static Audio& instance();
    void init(bool enabled);
    /** set whether sound should be played */
    void setEnabled(bool value) { enabled = value; };
    void shutdown();
    void setBgTrack(const std::string &track_name);
    void playSound(const std::string &sound_name);
    long getLastPlayed(const std::string &sound_name);
};

#endif /* audio_h */
