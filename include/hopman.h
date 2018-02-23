//
//  hopman.h
//  Main class for the Hopman platformer
//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef hopman_h
#define hopman_h

constexpr int ERROR = 1;
constexpr int OK = 0;

constexpr int STARTING_LEVEL = 1;

constexpr int DEFAULT_FPS_LIMIT = 60;
constexpr int DEFAULT_WINDOW_WIDTH = 1280;
constexpr int DEFAULT_WINDOW_HEIGHT = 720;

constexpr int UI_FONT_SIZE = 24;

constexpr auto LEVEL_FILE_DIR = "./Assets/levels/";
constexpr auto LEVEL_FILE_PREFIX = "level_";

constexpr int DEFAULT_EXTRA_LIVES = 2;

#include <iostream>
#include <vector>
#include <set>
#include <tuple>
#include <string>
#include <fstream>
#include <sstream>
#include "frame_timer.h"
#include "graphics.h"
#include "audio.h"
#include "input.h"
#include "resource_manager.h"
#include "drawable.h"


enum GameState {
    PLAYING,
    EXITING,
    PAUSED,
    LOSS,
    LEVEL_WON,
};

/**
 Structure to hold config from level config files
 */
struct LevelConfig {
    std::string bg_track;
};

class Hopman {
private:
    Input input;
    int level = STARTING_LEVEL;
    GameState game_state = GameState::PLAYING;
    int fps_limit = DEFAULT_FPS_LIMIT;
    int score = 0;
    int lives = DEFAULT_EXTRA_LIVES;

    int fps_display = 0;
    std::string screen_message = "";

    //Player player;
    std::vector<Drawable*> objects = {};

    void handleInput();
    void handleLevelEnd();
    void registerInputCallbacks();
    void update(int delta);
    void render();
    void renderUI();
    void renderText(int xpos, int ypos, int font_size, std::string text);

    bool checkRemoval(Drawable *obj);
    void removeDestroyed();
    void tryRespawn();
    void cleanupLevel();

    void parseLevelConfig(LevelConfig &config);
public:
    Hopman();
    ~Hopman();
    int play();
    void setupLevel();
};

#endif /* hopman_h */
