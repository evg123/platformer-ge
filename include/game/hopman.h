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
#include "being.h"
#include "tile.h"
#include "background.h"
#include "network.h"

constexpr int STARTING_LEVEL = 1;
constexpr auto BG_TRACK = "bg_track.mp3";

constexpr int DEFAULT_FPS_LIMIT = 60;
constexpr int DEFAULT_WINDOW_WIDTH = 1280;
constexpr int DEFAULT_WINDOW_HEIGHT = 720;

constexpr int UI_FONT_SIZE = 24;

constexpr auto LEVEL_FILE_PREFIX = "./Assets/levels/level_";

constexpr int DEFAULT_EXTRA_LIVES = 2;

constexpr int PAUSE_MENU_WIDTH = 500;
constexpr int PAUSE_MENU_HEIGHT = 500;
constexpr int PAUSE_MENU_TOP_PAD = 50;
constexpr int PAUSE_MENU_BOT_PAD = 100;
constexpr auto MAIN_MENU_BG_IMG = "ui/pause_menu.png";
constexpr auto MENU_TITLE_IMG = "ui/button_blank.png";
constexpr auto BUTTON_IMG = "ui/button.png";

constexpr auto STATUS_BAR_IMG = "ui/status_bar.png";
constexpr int STATUS_BAR_Y = 0;
constexpr int STATUS_BAR_THICKNESS = 55;
constexpr int STATUS_BAR_TEXT_SIZE = 25;

constexpr int GAME_MESSAGE_MAX_LEN = 25;
constexpr int GAME_MSG_WIDTH = 400;
constexpr int GAME_MSG_HEIGHT = 250;
constexpr int GAME_MSG_TEXT_SIZE = 120;

// These are not const/constexpr so that TextGuiElements
// can be made from them,
// but they should be treated as constants.
// There's probably a better way to do this...
static std::string SCORE_STR = "Score: ";
static std::string LIVES_STR = "Lives: ";
static std::string LEVEL_STR = "Level: ";
static std::string FPS_STR = "FPS: ";

enum class GameState {
    LEVEL_START,
    PLAYING,
    EXITING,
    RESPAWN,
    LOSS,
    LEVEL_WON,
};

/**
 Structure to hold config from level config files
 */
struct LevelConfig {
    std::vector<std::vector<int>> tiles;
};

/**
 Main class for the Hopman platformer
 Most of the game logic is handled by this class.
 */
class Hopman {
protected:
    bool server_mode;
    Client client;
    Server server;
    ClientInput player_input;

    int level;
    GameState game_state;
    bool paused;
    int fps_limit;
    int score;
    int lives;

    int fps_display;
    std::string game_message;

    std::vector<Being*> players;
    Background background;
    std::map<int, Drawable*> objects;
    /** player dies if they fall past here */
    int lower_bound;

    /** set game state to start quitting */
    void exitGame() { game_state = GameState::EXITING; }
    void toggleFps();
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
    virtual void handleDeath() = 0;
    void tryRespawn();
    void restartGame();
    void cleanupLevel();
    void hitGoal(Drawable &other);

    void parseLevelConfig(LevelConfig &config);
    void setupLevel();
    void createBackground();
    Drawable* addTile(int tile_type, int tx, int ty);
    virtual Being* addPlayerTile(int xpos, int ypos) = 0;
    bool isPlayer(Drawable *obj);
    virtual void networkUpdate() = 0;
public:
    void init(bool server_mode, std::string host);
    void shutdown();
    int play();
};

class HopmanClient : public Hopman {
private:
    Being* addPlayerTile(int xpos, int ypos);
    void networkUpdate();
    void updatePlayerInput();
    void handleDeath();
public:
};

class HopmanServer : public Hopman {
private:
    int player_start_pos_x;
    int player_start_pos_y;

    Being* addPlayerTile(int xpos, int ypos);
    Being* addNewPlayer();
    void networkUpdate();
    void handleDeath();
public:
};

#endif /* hopman_h */
