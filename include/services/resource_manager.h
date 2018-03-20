//
//  resource_manager.h
//  loads and saves game assets
//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef resource_manager_h
#define resource_manager_h

#include <string>
#include <string_view>
#include <map>
#include <utility>
#include "SDL2.framework/Headers/SDL.h"
#include "SDL2_image.framework/Headers/SDL_image.h"
#include "SDL2_mixer.framework/Headers/SDL_mixer.h"
#include "SDL2_ttf.framework/Headers/SDL_ttf.h"
#include "graphics.h"

constexpr auto IMAGE_DIR = "./Assets/images/";
constexpr auto MUSIC_DIR = "./Assets/music/";
constexpr auto SOUNDS_DIR = "./Assets/sounds/";

constexpr auto FONT_FILE = "./Assets/fonts/space-mono/SpaceMono-Bold.ttf";

/**
 Singleton that manages images, rendered text, sound effects and music data.
 Remembers what has been loaded already so that it can be re-used
 */
class ResourceManager {
private:
    ResourceManager();
    ~ResourceManager();
    std::map<std::string, SDL_Texture*> image_map;
    std::map<int, TTF_Font*> font_map;
    std::map<std::pair<std::string, int>, SDL_Texture*> text_map;
    std::map<std::string, Mix_Music*> music_map;
    std::map<std::string, Mix_Chunk*> sound_map;
    
    TTF_Font* getFont(int font_size);
    
    void free_images();
    void free_text();
    void free_fonts();
    void free_music();
    void free_sounds();
public:
    static ResourceManager& instance();
    void init();
    void shutdown();
    SDL_Texture* getImageTexture(const std::string &filename);
    SDL_Texture* getTextTexture(const std::string &text, int font_size);
    Mix_Music* getMusic(const std::string &track_name);
    Mix_Chunk* getSound(const std::string &sound_name);
};

#endif /* resource_manager_h */
