//
//  Created by Vande Griek, Eric on 2/22/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "hopman.h"

/**
 Set up the game
 */
void Hopman::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        throw std::runtime_error("Failed to initialize SDL");
    }

    // init services
    ResourceManager::instance().init();
    Graphics::instance().init(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    Audio::instance().init();
    Input::instance().init();
    Gui::instance().init();
    
    paused = false;
}

/**
 Tear it down
 */
void Hopman::shutdown() {
    cleanupLevel();

    // shutdown services
    Gui::instance().shutdown();
    Input::instance().shutdown();
    Audio::instance().shutdown();
    Graphics::instance().shutdown();
    ResourceManager::instance().shutdown();
    SDL_Quit();
}

/**
 Main gameplay loop
 */
int Hopman::play() {
    setupLevel();
    
    registerInputCallbacks();
    createUI();
    
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
        if (game_state == GameState::PLAYING && !paused) {
            update(delta);
        }

        // focus the screen on the player
        Graphics::instance().focusScreenOffsets(player.getRect().getCollider());
        background.updateLayerOffsets(player.getRect().xPos(), player.getRect().yPos());

        // update the GUI
        Gui::instance().update();

        // draw the new frame
        render();
    }
    
    return 0;
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
    Input::instance().registerCallback(Action::MOVE_LEFT, std::bind(&Being::moveLeft, &player));
    Input::instance().registerCallback(Action::STOP_LEFT, std::bind(&Being::stopLeft, &player));
    Input::instance().registerCallback(Action::MOVE_RIGHT, std::bind(&Being::moveRight, &player));
    Input::instance().registerCallback(Action::STOP_RIGHT, std::bind(&Being::stopRight, &player));
    Input::instance().registerCallback(Action::JUMP, std::bind(&Being::jump, &player));
}

void Hopman::createUI() {
    createStatusBar();
    createPauseMenu();
    createGameMessage();
}

void Hopman::createStatusBar() {
    // bar background
    GuiElement *elem = new GuiElement({0, STATUS_BAR_Y, Graphics::instance().getWindowWidth(), 50},
                                      ResourceManager::instance().getImageTexture(STATUS_BAR_IMG));
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);

    // score display
    elem = new TextGuiElement<std::string>({10, STATUS_BAR_Y + 5, 0, 0},
                                           SCORE_STR, STATUS_BAR_TEXT_SIZE);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);
    elem = new TextGuiElement<int>({10 + elem->getWidth(), STATUS_BAR_Y + 5, 0, 0},
                                           score, STATUS_BAR_TEXT_SIZE, true);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);
    
    // level display
    elem = new TextGuiElement<std::string>({200, STATUS_BAR_Y + 5, 0, 0},
                                           LEVEL_STR, STATUS_BAR_TEXT_SIZE);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);
    elem = new TextGuiElement<int>({200 + elem->getWidth(), STATUS_BAR_Y + 5, 0, 0},
                                   level, STATUS_BAR_TEXT_SIZE, true);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);
    
    // lives display
    elem = new TextGuiElement<std::string>({500, STATUS_BAR_Y + 5, 0, 0},
                                           LIVES_STR, STATUS_BAR_TEXT_SIZE);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);
    elem = new TextGuiElement<int>({500 + elem->getWidth(), STATUS_BAR_Y + 5, 0, 0},
                                   lives, STATUS_BAR_TEXT_SIZE, true);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);

    // show the status bar
    Gui::instance().toggleGroupDisplay(GuiGroupId::STATUS_BAR);
}

void Hopman::createPauseMenu() {
    int menu_x = (Graphics::instance().getWindowWidth() / 2) - (PAUSE_MENU_WIDTH / 2);
    int menu_y = (Graphics::instance().getWindowHeight() / 2) - (PAUSE_MENU_HEIGHT / 2);
    Menu *menu = new Menu({menu_x, menu_y, PAUSE_MENU_WIDTH, PAUSE_MENU_HEIGHT},
                          ResourceManager::instance().getImageTexture(MAIN_MENU_BG_IMG),
                          PAUSE_MENU_TOP_PAD, PAUSE_MENU_BOT_PAD);
    
    menu->addItem("Pause Menu", 50, ResourceManager::instance().getImageTexture(MENU_TITLE_IMG),
                  NULL, false);
    menu->addItem("Restart", 42, ResourceManager::instance().getImageTexture(BUTTON_IMG),
                  std::bind(&Hopman::restartGame, this), true);
    menu->addItem("Quit", 42,
                  ResourceManager::instance().getImageTexture(BUTTON_IMG),
                  std::bind(&Hopman::exitGame, this), true);
    
    // add the completed menu to the gui
    Gui::instance().add(GuiGroupId::PAUSE, menu);
}

void Hopman::createGameMessage() {
    // render a message of max length and get its size in order to center the message
    int msg_w, msg_h;
    std::string max_msg_str(GAME_MESSAGE_MAX_LEN, ' ');
    SDL_Texture *max_msg_texture = ResourceManager::instance().getTextTexture(max_msg_str, GAME_MSG_TEXT_SIZE);
    SDL_QueryTexture(max_msg_texture, NULL, NULL, &msg_w, &msg_h);

    // create a gui element for the message
    int item_x = (Graphics::instance().getWindowWidth() / 2) - (msg_w / 2);
    int item_y = (Graphics::instance().getWindowHeight() / 2) - (msg_h / 2);
    GuiElement *elem = new TextGuiElement<std::string>({item_x, item_y, msg_w, msg_h},
                                                       game_message, GAME_MSG_TEXT_SIZE, true);
    Gui::instance().add(GuiGroupId::GAME_MESSAGE, elem);
    game_message.reserve(GAME_MESSAGE_MAX_LEN);
}

void Hopman::setGameMessage(std::string new_msg) {
    if (new_msg.length() > GAME_MESSAGE_MAX_LEN) {
        throw std::runtime_error("Game message is too long: " + new_msg);
    }

    // get padding at front and end of new msg
    // round down to long
    long padding = (GAME_MESSAGE_MAX_LEN - new_msg.length()) / 2;
    std::string pad_str(padding, ' ');
    game_message.assign(pad_str + new_msg);
    Gui::instance().setGroupDisplay(GuiGroupId::GAME_MESSAGE, true);
}

/**
 Update all the game objects
 */
void Hopman::update(int delta) {
    for (auto &obj : objects) {
        obj->update(delta, objects);
        // check if obj has fallen off the map
        if (obj->getRect().top() > lower_bound) {
            obj->destroy();
        }
    }

    // clean up objects that need to be removed from the game
    removeDestroyed();
}

/**
 Remove any objects that have been destroyed
 */
void Hopman::removeDestroyed() {
    // check if the player was killed
    if (player.dead() || player.needsRemoval()) {
        tryRespawn();
    }

    // remove destroyed objects
    objects.erase(
        std::remove_if(objects.begin(),
                       objects.end(),
                       [this](Drawable *obj) -> bool {
                           if (obj->needsRemoval() && obj != &this->player) {
                               this->score += obj->getScoreOnDestruction();
                               return true;
                           }
                           return false;
                       }),
                  objects.end());
}

/**
 try to respawn after player death
 */
void Hopman::tryRespawn() {
    if (lives > 0) {
        // restart the level
        --lives;
        setGameMessage("You Died!");
        game_state = GameState::RESPAWN;
    } else {
        setGameMessage("GAME OVER :(");
        game_state = GameState::LOSS;
        Audio::instance().playSound("game_over.wav");
    }
}

void Hopman::pause() {
    paused = !paused;
    Gui::instance().toggleGroupDisplay(GuiGroupId::PAUSE);
}

/**
 Restart the game from the starting level
 */
void Hopman::restartGame() {
    // restart game
    level = STARTING_LEVEL;
    score = 0;
    lives = DEFAULT_EXTRA_LIVES;
    setupLevel();

    // unpause in case we restarted from the menu
    Gui::instance().setGroupDisplay(GuiGroupId::PAUSE, false);
}

/**
 Check if the user is ready to move on from the end of a level / loss
 */
void Hopman::advanceScreen() {
    if (paused) {
        // don't advance if we are paused
        return;
    }
    
    if (game_state == GameState::LOSS) {
        restartGame();
    } else if (game_state == GameState::LEVEL_WON) {
        // move to next level
        ++level;
        setupLevel();
    } else if (game_state == GameState::RESPAWN) {
        // try the level again
        setupLevel();
    } else if (game_state == GameState::LEVEL_START) {
        // start the level
        Gui::instance().setGroupDisplay(GuiGroupId::GAME_MESSAGE, false);
        game_state = GameState::PLAYING;
    }
}

/**
 Draw everything to the screen
 */
void Hopman::render() {
    background.render();
    
    for (auto &obj : objects) {
        obj->render();
    }
    
    renderGui();
    
    Graphics::instance().swapFrame();
}

/**
 Draw the UI on top of everthing
 */
void Hopman::renderGui() {
    Gui::instance().render();
}

/**
 create a tile object at the given tile coordinates
 */
void Hopman::add_tile(int tile_type, int tx, int ty) {
    if (tile_type == TileNum::EMPTY) {
        return;
    }

    Drawable *obj;
    if (tile_type == TileNum::PLAYER) {
        player.init(BeingType::player());
        obj = &player;

    } else if (tile_type == TileNum::RED_ENEMY) {
        Being *enemy = new Being();
        enemy->init(BeingType::redEnemy());
        obj = enemy;

    } else if (tile_type == TileNum::BLUE_ENEMY) {
        Being *enemy = new Being();
        enemy->init(BeingType::blueEnemy());
        obj = enemy;

    } else {
        Tile *tile = new Tile(tile_type);
        if (tile_type == TileNum::GOAL) {
            tile->setCollisionCallback(std::bind(&Hopman::hitGoal, this, std::placeholders::_1));
        }
        obj = tile;
    }

    // calculate position based on tile index
    int xpos = tx * TILE_SIDE;
    int ypos = ty * TILE_SIDE;
    obj->setPosition(xpos, ypos);

    objects.push_back(obj);
}

void Hopman::hitGoal(Drawable& other) {
    if (&other == &player) {
        setGameMessage("YOU WIN!");
        game_state = GameState::LEVEL_WON;
        Audio::instance().playSound("you_win.wav");
    }
}

void Hopman::createBackground() {
    int sw = Graphics::instance().getWindowWidth();
    int sh = Graphics::instance().getWindowHeight();
    background.init(player.getRect().xPos(), player.getRect().yPos(), lower_bound - 200);
    background.setColor(125, 90, 125);
    
    // add layers at different distances
    background.addLayer("background/dusk/layer_0.png", sw, sh, 50);
    background.addLayer("background/dusk/layer_1.png", sw, sh, 35);
    background.addLayer("background/dusk/layer_2.png", sw, sh, 15);
    background.addLayer("background/dusk/layer_3.png", sw, sh, 6);
    background.addLayer("background/dusk/layer_4.png", sw, sh, 2);
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
    bool have_player = false;
    bool have_goal = false;
    for (int tx = 0; tx < lvl_conf.tiles.size(); ++tx) {
        for (int ty = 0; ty < lvl_conf.tiles[0].size(); ++ty) {
            int tile_num = lvl_conf.tiles[tx][ty];
            if (tile_num == TileNum::PLAYER) {
                have_player = true;
            } else if (tile_num == TileNum::GOAL) {
                have_goal = true;
            }
            add_tile(tile_num, tx, ty);
        }
    }

    // make sure we have a player tile in the level
    if (!have_player) {
        throw std::runtime_error("Invalid level file, no player tile found!");
    }
    // make sure we have a goal tile in the level
    if (!have_goal) {
        throw std::runtime_error("Invalid level file, no goal tile found!");
    }

    // setup background layers
    createBackground();

    // start the background track for the level
    //TODO make custom for each level
    Audio::instance().setBgTrack(BG_TRACK);

    // start out on the level start screen
    setGameMessage("Level " + std::to_string(level));
    game_state = GameState::LEVEL_START;
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
        
        // set lower bound of level
        lower_bound = level_height * TILE_SIDE;

        // initialize the size of the 2d vector of tiles
        config.tiles.resize(level_width, std::vector<int>(level_height, TileNum::EMPTY));

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
        if (obj != &player) {
            delete obj;
        }
    }
    objects.clear();

    Gui::instance().setGroupDisplay(GuiGroupId::GAME_MESSAGE, false);

    background.shutdown();
}

