//
//  resource_manager.h
//  loads and saves game assets
//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef resource_manager_h
#define resource_manager_h

#ifdef __linux__
#include "../lib/linux/SDL2.framework/Headers/SDL.h"
#include "../lib/linux/SDL2_image.framework/Headers/SDL_image.h"
#include "../lib/linux/SDL2_mixer.framework/Headers/SDL_mixer.h"
#include "../lib/linux/SDL2_ttf.framework/Headers/SDL_ttf.h"
#else // Mac
#include "../lib/osx/SDL2.framework/Headers/SDL.h"
#include "../lib/osx/SDL2_image.framework/Headers/SDL_image.h"
#include "../lib/osx/SDL2_mixer.framework/Headers/SDL_mixer.h"
#include "../lib/osx/SDL2_ttf.framework/Headers/SDL_ttf.h"
#endif

#include <string>
#include <string_view>
#include <unordered_map>
#include "graphics.h"

constexpr int FONT_SIZE = 64;
constexpr auto IMAGE_DIR = "./Assets/images/";
constexpr auto MUSIC_DIR = "./Assets/music/";
constexpr auto SOUNDS_DIR = "./Assets/sounds/";

/**
 Singleton that manages images, rendered text, sound effects and music data.
 Remembers what has been loaded already so that it can be re-used
 */
class ResourceManager {
private:
    ResourceManager();
    ~ResourceManager();
    std::unordered_map<std::string, SDL_Texture*> image_map;
    std::unordered_map<std::string, SDL_Texture*> text_map;
    std::unordered_map<std::string, Mix_Music*> music_map;
    std::unordered_map<std::string, Mix_Chunk*> sound_map;
    TTF_Font *font = NULL;
    
    void free_images();
    void free_text();
    void free_music();
    void free_sounds();
public:
    static ResourceManager& instance();
    SDL_Texture* getImageTexture(const std::string &filename);
    SDL_Texture* getTextTexture(const std::string &text);
    Mix_Music* getMusic(const std::string &track_name);
    Mix_Chunk* getSound(const std::string &sound_name);
};

#endif /* resource_manager_h */
