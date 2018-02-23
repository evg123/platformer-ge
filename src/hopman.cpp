//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "../include/hopman.h"

/**
 Set up the game
 */
Hopman::Hopman() {
    // init SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        throw std::runtime_error("Failed to initialize SDL");
    }
    
    Graphics::instance().init(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
}

/**
 Tear it down
 */
Hopman::~Hopman() {
    for (auto obj : objects) {
        delete obj;
    }
    SDL_Quit();
}

/**
 Main gameplay loop
 */
int Hopman::play() {
    setupLevel();
    
    registerInputCallbacks();
    
    FrameTimer timer = FrameTimer(fps_limit);
    
    while (game_state != GameState::EXITING) {
        // wait until it is time to render the next frame
        timer.delayUntilNextFrame();
        
        fps_display = timer.getFps();
        
        // signal a new frame to the fps timer and get the delta since the last frame
        int delta = timer.newFrame();
        
        // update game objects
        if (game_state == GameState::PLAYING) {
            update(delta);
        }
        
        // handle game over / next level
        handleLevelEnd();
        
        // draw the new frame
        render();
    }
    
    return OK;
}

/**
 register callbacks with the input subsystem that get called on certain actions
 */
void Hopman::registerInputCallbacks() {
    
}

/**
 Update all the game objects
 */
void Hopman::update(int delta) {
    for (auto &obj : objects) {
        obj->update(delta, objects);
    }
    
    removeDestroyed();
}

/**
 Remove any objects that have been destroyed
 */
void Hopman::removeDestroyed() {
    // remove destroyed objects
    //TODO
}

/**
 Check if the user is ready to move on from the end of a level / loss
 */
void Hopman::handleLevelEnd() {
    if (game_state == GameState::LOSS) {
        if (input.hasClicked()) {
            // restart game
            level = STARTING_LEVEL;
            score = 0;
            lives = DEFAULT_EXTRA_LIVES;
            setupLevel();
        }
    } else if (game_state == GameState::LEVEL_WON) {
        if (input.hasClicked()) {
            // move to next level
            ++level;
            setupLevel();
        }
    }
}

/**
 Draw everything to the screen
 */
void Hopman::render() {
    Graphics::instance().clear();
    
    for (auto &obj : objects) {
        obj->render(Graphics::instance().getRenderer());
    }
    
    renderUI();
    
    Graphics::instance().swapFrame();
}

/**
 Draw the UI on top of everthing
 */
void Hopman::renderUI() {
    int xpos = 0, ypos = 0;
    if (input.shouldDisplayFps()) {
        renderText(xpos, ypos, UI_FONT_SIZE, std::to_string(fps_display));
    }
    renderText(xpos, ypos, UI_FONT_SIZE, "score " + std::to_string(score));
    renderText(xpos, ypos, UI_FONT_SIZE, "lives " + std::to_string(lives));
    if (!screen_message.empty()) {
        renderText(xpos, ypos, UI_FONT_SIZE, screen_message);
    }
}

/**
 Draw text at coordinates in a certain size
 */
void Hopman::renderText(int xpos, int ypos, int font_size, std::string text) {
    int tw = 0, th = 0;
    SDL_Texture *fps_text = ResourceManager::instance().getTextTexture(text);
    SDL_Rect rect = {xpos, ypos, tw, th};
    SDL_RenderCopy(Graphics::instance().getRenderer(), fps_text, NULL, &rect);
}

/**
 Load the next level from config
 */
void Hopman::setupLevel() {
    // cleanup previous level
    cleanupLevel();
    
    LevelConfig lvl_conf;
    parseLevelConfig(lvl_conf);

    // start the background track for the level
    Audio::instance().setBgTrack(lvl_conf.bg_track);
    
    // ready to go!
    game_state = GameState::PLAYING;
}

/**
 Read a level config file and fill in the passed in config struct
 */
void Hopman::parseLevelConfig(LevelConfig &config) {
    // glob levels in level dir and order alphabetically
    // repeat last level if we run out
    std::ifstream cfg_stream("");
    //TODO
    
    if (!cfg_stream.good()) {
        // couldn't load any level files
        throw std::runtime_error("Failed to find a level config file");
    }
    
    // default to no bg track
    config.bg_track = "";
    
    try {
        //TODO read level config file
    } catch (const std::exception &ex) {
        throw std::runtime_error(std::string("Failed to parse config file: %s") + ex.what());
    }
}

/**
 Remove objects at the end of a level or loss
 */
void Hopman::cleanupLevel() {
    // deallocate everything in the drawable list
    for (auto &obj : objects) {
        delete obj;
    }
    objects.clear();
    
    screen_message = "";
}

