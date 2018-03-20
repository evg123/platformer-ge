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
}

/**
 Tear down
 */
void Audio::shutdown() {
    Mix_Quit();
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
}

/**
 Play a sound effect
 sound_name should be the name of a file in the sounds directory.
 */
void Audio::playSound(const std::string &sound_name) {
    Mix_Chunk *sound = ResourceManager::instance().getSound(sound_name);
    Mix_PlayChannel(-1, sound, 0);
}
