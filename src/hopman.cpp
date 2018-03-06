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
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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

        // tell the input singleton to poll for events
        Input::instance().handleEvents();
        
        // signal a new frame to the fps timer and get the delta since the last frame
        int delta = timer.newFrame();
        
        // update game objects
        if (game_state == GameState::PLAYING) {
            update(delta);
        }

        // draw the new frame
        render();
    }
    
    return OK;
}

/**
 register callbacks with the input subsystem that get called on certain actions
 */
void Hopman::registerInputCallbacks() {
    // game events
    Input::instance().registerCallback(Action::EXIT_GAME, std::bind(&Hopman::exitGame, this));
    Input::instance().registerCallback(Action::ADVACNE, std::bind(&Hopman::advanceScreen, this));
    Input::instance().registerCallback(Action::TOGGLE_FPS, std::bind(&Hopman::toggleFps, this));
    Input::instance().registerCallback(Action::TOGGLE_PAUSE, std::bind(&Hopman::pause, this));
    
    // player movement
    Input::instance().registerCallback(Action::MOVE_LEFT, std::bind(&Player::moveLeft, player));
    Input::instance().registerCallback(Action::STOP_LEFT, std::bind(&Player::stopLeft, player));
    Input::instance().registerCallback(Action::MOVE_RIGHT, std::bind(&Player::moveRight, player));
    Input::instance().registerCallback(Action::STOP_RIGHT, std::bind(&Player::stopRight, player));
    Input::instance().registerCallback(Action::JUMP, std::bind(&Player::jump, player));
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

void Hopman::pause() {
    if (game_state == GameState::PAUSED) {
        game_state = GameState::PLAYING;
    } else if (game_state == GameState::PAUSED) {
        game_state = GameState::PLAYING;
    }
}

/**
 Check if the user is ready to move on from the end of a level / loss
 */
void Hopman::advanceScreen() {
    if (game_state == GameState::LOSS) {
        // restart game
        level = STARTING_LEVEL;
        score = 0;
        lives = DEFAULT_EXTRA_LIVES;
        setupLevel();
    } else if (game_state == GameState::LEVEL_WON) {
        // move to next level
        ++level;
        setupLevel();
    }
}

/**
 Draw everything to the screen
 */
void Hopman::render() {
    Graphics::instance().clear();
    
    for (auto &obj : objects) {
        obj->render();
    }
    
    renderUI();
    
    Graphics::instance().swapFrame();
}

/**
 Draw the UI on top of everthing
 */
void Hopman::renderUI() {
    int xpos = 0, ypos = 0;
    if (display_fps) {
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
 create a tile object at the given tile coordinates
 */
void Hopman::add_tile(int tile_type, int tx, int ty) {
    if (tile_type == EMPTY_TILE_NUM) {
        return;
    }

    Drawable *obj;
    if (tile_type == PLAYER_POS_TILE) {
        player = new Player();
        obj = player;
        
    } else {
        obj = new Tile(tile_type);
    }

    // calculate position based on tile index
    int xpos = tx * TILE_SIDE;
    int ypos = ty * TILE_SIDE;
    obj->setPosition(xpos, ypos);

    objects.push_back(obj);
}

/**
 Load the next level from config
 */
void Hopman::setupLevel() {
    // cleanup previous level
    cleanupLevel();
    
    LevelConfig lvl_conf;
    parseLevelConfig(lvl_conf);

    // instantiate level objects
    for (int tx = 0; tx < lvl_conf.tiles.size(); ++tx) {
        for (int ty = 0; ty < lvl_conf.tiles[0].size(); ++ty) {
            int tile_num = lvl_conf.tiles[tx][ty];
            add_tile(tile_num, tx, ty);
        }
    }

    // make sure we had a player tile in the level
    if (player == NULL) {
        throw std::runtime_error("Invalid level file, no player tile found!");
    }
    
    // start the background track for the level
    //TODO make custom for each level
    Audio::instance().setBgTrack(BG_TRACK);

    
    // start out paused
    game_state = GameState::PAUSED;
}

/**
 Read a level config file and fill in the passed in config struct
 */
void Hopman::parseLevelConfig(LevelConfig &config) {
    // find the largest level file that is <= level
    // this lets you repeat levels by skipping some filenames
    // you must at least provide a level_1 file
    // wanted to read all files from the level directory, but OSX still doesn't
    // have support for std::filesystem
    std::ifstream cfg_stream("");
    int effective_lvl = level;
    while (effective_lvl > 0 && !cfg_stream.good()) {
        // see if we can load this level file
        std::string lvl_file = LEVEL_FILE_PREFIX + std::to_string(effective_lvl);
        cfg_stream.open(lvl_file);
        --effective_lvl;
    }
    
    if (!cfg_stream.good()) {
        // couldn't load any level files
        throw std::runtime_error("Failed to find a level config file");
    }
    
    try {
        std::string line;
        
        // first line is level dimensions
        std::getline(cfg_stream, line);
        std::stringstream ss(line);
        std::string tok;
        ss >> tok;
        int level_width = std::stoi(tok);
        ss >> tok;
        int level_height = std::stoi(tok);
        // initialize the size of the 2d vector of tiles
        config.tiles.resize(level_width, std::vector<int>(level_height, EMPTY_TILE_NUM));

        // read line by line
        int xt = 0, yt = 0; // tile indicies
        while (std::getline(cfg_stream, line)) {
            std::stringstream line_stream(line);
            std::string tile_str;
            while (std::getline(line_stream, tile_str, ' ')) {
                config.tiles.at(xt).at(yt) = std::stoi(tile_str);
                ++xt;
            }
            ++yt;
            xt = 0;
        }
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

