//
//  hopman.h
//  Main class for the Hopman platformer
//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef hopman_h
#define hopman_h

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
#include "gui.h"
#include "menu.h"
#include "gui_element.h"
#include "resource_manager.h"
#include "drawable.h"
#include "player.h"
#include "tile.h"

constexpr int ERROR = 1;
constexpr int OK = 0;

constexpr int STARTING_LEVEL = 1;
constexpr auto BG_TRACK = "bg_track.wav";

constexpr int DEFAULT_FPS_LIMIT = 60;
constexpr int DEFAULT_WINDOW_WIDTH = 1280;
constexpr int DEFAULT_WINDOW_HEIGHT = 720;

constexpr int UI_FONT_SIZE = 24;

constexpr auto LEVEL_FILE_PREFIX = "./Assets/levels/level_";

constexpr int DEFAULT_EXTRA_LIVES = 2;

constexpr unsigned int EMPTY_TILE_NUM = 0;
constexpr unsigned int PLAYER_POS_TILE = 1;

constexpr int PAUSE_MENU_WIDTH = 450;
constexpr int PAUSE_MENU_HEIGHT = 500;
constexpr int MENU_SIDE_PADDING = 20;
constexpr auto MAIN_MENU_BG_IMG = "ui/pause_menu.png";
constexpr int PAUSE_MENU_BTN_HEIGHT = 120;
constexpr auto BUTTON_IMG = "ui/button.png";

constexpr auto STATUS_BAR_IMG = "ui/status_bar.png";
constexpr int STATUS_BAR_Y = 0;
constexpr int STATUS_BAR_TEXT_SIZE = 25;

constexpr int GAME_MSG_WIDTH = 400;
constexpr int GAME_MSG_HEIGHT = 250;
constexpr int GAME_MSG_TEXT_SIZE = 120;

// These are not const/constexpr so that TextGuiElements
// can be made from them,
// but they should be treated as constants.
// There's probably a better way to do this...
static std::string SCORE_STR = "Score:";
static std::string LIVES_STR = "Lives:";
static std::string LEVEL_STR = "Level:";

enum class GameState {
    LEVEL_START,
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
    std::vector<std::vector<int>> tiles;
};

class Hopman {
private:
    int level = STARTING_LEVEL;
    GameState game_state = GameState::PAUSED;
    int fps_limit = DEFAULT_FPS_LIMIT;
    int score = 0;
    int lives = DEFAULT_EXTRA_LIVES;

    bool display_fps = false;
    int fps_display = 0;
    std::string game_message;

    Player player;
    std::vector<Drawable*> objects = {};
    /** player dies if they fall past here */
    int lower_bound;

    /** set game state to start quitting */
    void exitGame() { game_state = GameState::EXITING; }
    /** toggle the fps display UI */
    void toggleFps() { display_fps = !display_fps; }
    void pause();
    
    void handleInput();
    void advanceScreen();
    void registerInputCallbacks();
    void update(int delta);
    void render();
    void renderGui();
    void renderText(int xpos, int ypos, int font_size, std::string text);

    void createUI();
    void createStatusBar();
    void createPauseMenu();
    void createGameMessage();
    void setGameMessage(std::string new_msg);
    
    bool checkRemoval(Drawable *obj);
    void removeDestroyed();
    void tryRespawn();
    void cleanupLevel();

    void parseLevelConfig(LevelConfig &config);
    void setupLevel();
    void add_tile(int tile_type, int tx, int ty);
public:
    Hopman();
    ~Hopman();
    int play();
};

#endif /* hopman_h */
