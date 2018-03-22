//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "audio.h"

/**
 Constructor not used, things are set up in init()
 */
Audio::Audio() {}

/**
 Private destructor
 */
Audio::~Audio() {}

/**
 Get the singleton instance
 */
Audio& Audio::instance() {
    static Audio *instance = new Audio();
    return *instance;
}

/**
 Set up
 */
void Audio::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("%s\n", Mix_GetError());
        throw std::runtime_error("Failed to init audio");
    }
    bg_track = NULL;
}

/**
 Tear down
 */
void Audio::shutdown() {
    Mix_Quit();
}

/**
 Record the time that this sound was played
 */
void Audio::setPlayed(const std::string &track_name) {
    last_played_map[track_name] = SDL_GetTicks();
}

/**
 Get the timestamp for the last time the passed in sound was played.
 Returns 0 if this is the first time it was played.
 */
Uint8 Audio::getLastPlayed(const std::string &sound_name) {
    auto item = last_played_map.find(sound_name);
    if (item == last_played_map.end()) {
        return 0;
    }
    return item->second;
}

/**
 Set the background music track to play.
 Use an empty string to stop playing music.
 track_name should be the name of a file in the music directory.
 */
void Audio::setBgTrack(const std::string &track_name) {
    if (track_name.empty()) {
        Mix_HaltMusic();
        bg_track = NULL;
        return;
    }

    Mix_Music *track = ResourceManager::instance().getMusic(track_name);
    if (track == bg_track) {
        // already playing this track
        return;
    }

    Mix_PlayMusic(track, -1);
    setPlayed(track_name);
}

/**
 Play a sound effect
 sound_name should be the name of a file in the sounds directory.
 */
void Audio::playSound(const std::string &sound_name) {
    if (sound_name.empty()) {
        // don't play anything
        return;
    }
    Mix_Chunk *sound = ResourceManager::instance().getSound(sound_name);
    Mix_PlayChannel(-1, sound, 0);
    setPlayed(sound_name);
}
