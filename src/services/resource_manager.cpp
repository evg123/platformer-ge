//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "resource_manager.h"

/**
 Constructor not used, things are set up in init()
 */
ResourceManager::ResourceManager() {}

/**
 Private destructor
 */
ResourceManager::~ResourceManager() {}

/**
 Get singleton instance
 */
ResourceManager& ResourceManager::instance() {
    static ResourceManager *instance = new ResourceManager();
    return *instance;
}

/**
 Set up
 */
void ResourceManager::init() {
    if (TTF_Init() != 0) {
        SDL_Log("%s\n", TTF_GetError());
        throw std::runtime_error("Failed to init TTF");
    }
}

/**
 Tear down
 */
void ResourceManager::shutdown() {
    free_images();
    free_text();
    free_music();
    free_sounds();
    free_fonts();

    TTF_Quit();
}

/**
 Unload and destroy objects
 */
void ResourceManager::free_images() {
    auto it = image_map.begin();
    while (it != image_map.end()) {
        SDL_Texture *tex = it->second;
        SDL_DestroyTexture(tex);
        ++it;
    }
}

/**
 Unload and destroy objects
 */
void ResourceManager::free_fonts() {
    auto it = font_map.begin();
    while (it != font_map.end()) {
        TTF_Font *font = it->second;
        TTF_CloseFont(font);
        ++it;
    }
}

/**
 Unload and destroy objects
 */
void ResourceManager::free_text() {
    auto it = text_map.begin();
    while (it != text_map.end()) {
        SDL_Texture *tex = it->second;
        SDL_DestroyTexture(tex);
        ++it;
    }
}

/**
 Unload and destroy objects
 */
void ResourceManager::free_music() {
    auto it = music_map.begin();
    while (it != music_map.end()) {
        Mix_Music *obj = it->second;
        Mix_FreeMusic(obj);
        ++it++;
    }
}

/**
 Unload and destroy objects
 */
void ResourceManager::free_sounds() {
    auto it = sound_map.begin();
    while (it != sound_map.end()) {
        Mix_Chunk *obj = it->second;
        Mix_FreeChunk(obj);
        ++it++;
    }
}

/**
 Load or retrieve the texture for an image
 */
SDL_Texture* ResourceManager::getImageTexture(const std::string &name) {
    SDL_Texture *texture;
    auto map_val = image_map.find(name);
    
    if (map_val == image_map.end()) {
        // not found, need to initialize
        std::string filename = IMAGE_DIR + name;
        SDL_Surface *surf = IMG_Load(filename.c_str());
        if (surf == NULL) {
            SDL_Log("%s\n", IMG_GetError());
            throw std::runtime_error("Failed to load image: " + name);
        }
        
        // create texture from surface
        texture = SDL_CreateTextureFromSurface(Graphics::instance().getRenderer(), surf);
        
        // free surface
        SDL_FreeSurface(surf);
        
        // add texture to map
        image_map.insert({name, texture});
    } else {
        texture = map_val->second;
    }
    
    return texture;
}

/**
 Load or retrieve the texture for an image
 */
TTF_Font* ResourceManager::getFont(int font_size) {
    TTF_Font *font;
    auto map_val = font_map.find(font_size);
    
    if (map_val == font_map.end()) {
        font = TTF_OpenFont(FONT_FILE, font_size);
        if (font == NULL) {
            SDL_Log("%s\n", TTF_GetError());
            throw std::runtime_error("Failed to load font");
        }

        // add font to map
        font_map.insert({font_size, font});
    } else {
        font = map_val->second;
    }
    
    return font;
}

/**
 Load or retrieve the texture for a string
 */
SDL_Texture* ResourceManager::getTextTexture(const std::string &text, int font_size) {
    SDL_Texture *texture;
    auto map_val = text_map.find({text, font_size});
    
    if (map_val == text_map.end()) {
        // not found, need to initialize
        SDL_Color text_color = {100, 0, 100};
        TTF_Font *font = getFont(font_size);
        SDL_Surface *surf = TTF_RenderText_Blended(font, text.c_str(), text_color);
        if (surf == NULL) {
            SDL_Log("%s\n", TTF_GetError());
            throw std::runtime_error("Failed to render text");
        }

        // create texture from surface
        texture = SDL_CreateTextureFromSurface(Graphics::instance().getRenderer(), surf);

        // free surface
        SDL_FreeSurface(surf);
        
        // add texture to map
        text_map.insert({{text, font_size}, texture});
    } else {
        texture = map_val->second;
    }
    
    return texture;
}

/**
 Load or retrieve the object for a song
 */
Mix_Music* ResourceManager::getMusic(const std::string &track_name) {
    Mix_Music *mix;
    auto map_val = music_map.find(track_name);

    if (map_val == music_map.end()) {
        // not found, need to initialize
        std::string filename = MUSIC_DIR + track_name;
        mix = Mix_LoadMUS(filename.c_str());
        if (mix == NULL) {
            SDL_Log("%s\n", Mix_GetError());
            throw std::runtime_error("Failed to load music: " + track_name);
        }
        
        // add texture to map
        music_map.insert({track_name, mix});
    } else {
        mix = map_val->second;
    }
    
    return mix;
}

/**
 Load or retrieve the object for a sound
 */
Mix_Chunk* ResourceManager::getSound(const std::string &sound_name) {
    Mix_Chunk *mix;
    auto map_val = sound_map.find(sound_name);
    
    if (map_val == sound_map.end()) {
        // not found, need to initialize
        std::string filename = SOUNDS_DIR + sound_name;
        mix = Mix_LoadWAV(filename.c_str());
        if (mix == NULL) {
            SDL_Log("%s\n", Mix_GetError());
            throw std::runtime_error("Failed to load sound: " + sound_name);
        }
        
        // add texture to map
        sound_map.insert({sound_name, mix});
    } else {
        mix = map_val->second;
    }
    
    return mix;
}
