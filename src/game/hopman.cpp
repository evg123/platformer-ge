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

    objects = {};
    fps_display = 0;
    paused = false;
    level = STARTING_LEVEL;
    game_state = GameState::NONE;
    setGameState(GameState::LOADING);
    fps_limit = DEFAULT_FPS_LIMIT;
    score = 0;
    lives = DEFAULT_EXTRA_LIVES;
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
    registerInputCallbacks();
    createUI();

    FrameTimer timer = FrameTimer(fps_limit);

    while (true) {
        // wait until it is time to render the next frame
        timer.delayUntilNextFrame();

        fps_display = timer.getFps();

        // signal a new frame to the fps timer and get the delta since the last frame
        int delta = timer.newFrame();

        // tell the input singleton to poll for events
        Input::instance().handleEvents();
        if (game_state == GameState::EXITING) {
            break;
        }

        // update game objects
        if (game_state == GameState::PLAYING && !paused) {
            update(delta);

            // focus the screen on the player
            Graphics::instance().focusScreenOffsets(getPlayer()->getRect().getCollider());
            background.updateLayerOffsets(getPlayer()->getRect().xPos(),
                                          getPlayer()->getRect().yPos());
        }

        // update the GUI
        Gui::instance().update();

        // draw the new frame
        render();

        handleGameState();

        // send/receive updates on the network
        networkUpdate();
    }
    
    return 0;
}

/**
 toggle the fps display UI
 */
void Hopman::toggleFps() {
    Gui::instance().toggleGroupDisplay(GuiGroupId::FPS_DISPLAY);
}

/**
 Set up the UI for the game
 */
void Hopman::createUI() {
    createStatusBar();
    createPauseMenu();
    createGameMessage();
}

/**
 Set up the status bar displayed at the top of the screen
 */
void Hopman::createStatusBar() {
    int screen_w = Graphics::instance().getWindowWidth();

    // bar background
    GuiElement *elem = new GuiElement({0, STATUS_BAR_Y, screen_w, STATUS_BAR_THICKNESS},
                                      ResourceManager::instance().getImageTexture(STATUS_BAR_IMG));
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);

    // score display
    int elem_pos = 10;
    elem = new TextGuiElement<std::string>({elem_pos, STATUS_BAR_Y + 5, 0, 0},
                                           SCORE_STR, STATUS_BAR_TEXT_SIZE);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);
    elem = new TextGuiElement<int>({elem_pos + elem->getWidth(), STATUS_BAR_Y + 5, 0, 0},
                                           score, STATUS_BAR_TEXT_SIZE, true);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);
    
    // level display
    elem_pos = 250;
    elem = new TextGuiElement<std::string>({elem_pos, STATUS_BAR_Y + 5, 0, 0},
                                           LEVEL_STR, STATUS_BAR_TEXT_SIZE);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);
    elem = new TextGuiElement<int>({elem_pos + elem->getWidth(), STATUS_BAR_Y + 5, 0, 0},
                                   level, STATUS_BAR_TEXT_SIZE, true);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);
    
    // lives display
    elem_pos = screen_w - 150;
    elem = new TextGuiElement<std::string>({elem_pos, STATUS_BAR_Y + 5, 0, 0},
                                           LIVES_STR, STATUS_BAR_TEXT_SIZE);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);
    elem = new TextGuiElement<int>({elem_pos + elem->getWidth(), STATUS_BAR_Y + 5, 0, 0},
                                   lives, STATUS_BAR_TEXT_SIZE, true);
    Gui::instance().add(GuiGroupId::STATUS_BAR, elem);

    // fps display
    // in a different group so it can be hidden independently
    elem_pos = screen_w - 300;
    elem = new TextGuiElement<std::string>({elem_pos, STATUS_BAR_Y + 5, 0, 0},
                                           FPS_STR, STATUS_BAR_TEXT_SIZE);
    Gui::instance().add(GuiGroupId::FPS_DISPLAY, elem);
    elem = new TextGuiElement<int>({elem_pos + elem->getWidth(), STATUS_BAR_Y + 5, 0, 0},
                                   fps_display, STATUS_BAR_TEXT_SIZE, true);
    Gui::instance().add(GuiGroupId::FPS_DISPLAY, elem);

    // show the status bar
    Gui::instance().setGroupDisplay(GuiGroupId::STATUS_BAR, true);
}

void Hopman::createPauseMenu() {
    int menu_x = (Graphics::instance().getWindowWidth() / 2) - (PAUSE_MENU_WIDTH / 2);
    int menu_y = (Graphics::instance().getWindowHeight() / 2) - (PAUSE_MENU_HEIGHT / 2);
    Menu *menu = new Menu({menu_x, menu_y, PAUSE_MENU_WIDTH, PAUSE_MENU_HEIGHT},
                          ResourceManager::instance().getImageTexture(MAIN_MENU_BG_IMG),
                          PAUSE_MENU_TOP_PAD, PAUSE_MENU_BOT_PAD);
    
    menu->addItem("Pause Menu", 50, ResourceManager::instance().getImageTexture(MENU_TITLE_IMG),
                  NULL, false);
    //TODO replace
    //menu->addItem("Restart", 42, ResourceManager::instance().getImageTexture(BUTTON_IMG),
    //              std::bind(&Hopman::restartGame, this), true);
    menu->addItem("Quit", 42,
                  ResourceManager::instance().getImageTexture(BUTTON_IMG),
                  std::bind(&Hopman::exitGame, this), true);
    
    // add the completed menu to the gui
    Gui::instance().add(GuiGroupId::PAUSE, menu);
}

/**
 Set up the message occasionally displayed across the center of the screen
 */
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

/**
 Set the message displayed at the center of the screen
 */
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
    for (auto &obj_record : objects) {
        Drawable *obj = obj_record.second;
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
    handleDeath();

    // remove destroyed objects
    for (auto iter = objects.begin(); iter != objects.end();) {
        if (iter->second->needsRemoval() && !isPlayer(iter->second)) {
           this->score += iter->second->getScoreOnDestruction();
            iter = objects.erase(iter);
        } else {
            ++iter;
        }
    }
}

/**
 try to respawn after player death
 */
void Hopman::tryRespawn() {
    if (lives > 0) {
        // restart the level
        --lives;
        setGameState(GameState::RESPAWN);
    } else {
        setGameState(GameState::LOSS);
    }
}

void Hopman::pause() {
    paused = !paused;
    Gui::instance().toggleGroupDisplay(GuiGroupId::PAUSE);
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
        setGameState(GameState::LOADING);
    } else if (game_state == GameState::LEVEL_WON) {
        // move to next level
        setGameState(GameState::LOADING);
    } else if (game_state == GameState::RESPAWN) {
        // try the level again
        setGameState(GameState::LEVEL_START);
    } else if (game_state == GameState::LEVEL_START) {
        // start the level
        setGameState(GameState::PLAYING);
    }
}

/**
 Draw everything to the screen
 */
void Hopman::render() {
    background.render();
    
    for (auto &obj_rec : objects) {
        Drawable *obj = obj_rec.second;
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
Drawable* Hopman::addTile(int tile_type, int tx, int ty, int id) {
    if (tile_type == TileNum::EMPTY) {
        return NULL;
    }

    // calculate position based on tile index
    int xpos = tx * TILE_SIDE;
    int ypos = ty * TILE_SIDE;

    Drawable *obj;
    if (tile_type == TileNum::PLAYER) {
        Being *player = new Being();
        player->init(BeingType::player());
        PlayerState player_state(player);
        players.push_back(player_state);
        obj = player;

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

    if (id > 0) {
        // id was specified, override the auto-assigned id
        obj->setId(id);
    }

    obj->setPosition(xpos, ypos);
    objects[obj->getId()] = obj;
    return obj;
}

Being* Hopman::getPlayer() {
    return players[0].player;
}

/**
 Called when something hits a goal tile.
 If its the player then they beat the level
 */
void Hopman::hitGoal(Drawable& other) {
    if (isPlayer(&other)) {
        setGameState(GameState::LEVEL_WON);
    }
}

/**
 Set up a parallax background
 */
void Hopman::createBackground() {
    int sw = Graphics::instance().getWindowWidth();
    int sh = Graphics::instance().getWindowHeight();
    background.init(getPlayer()->getRect().xPos(),
                    getPlayer()->getRect().yPos(),
                    lower_bound - 200);
    background.setColor(125, 90, 125);
    
    // add layers at different distances
    background.addLayer("background/dusk/layer_0.png", sw, sh, 60);
    background.addLayer("background/dusk/layer_1.png", sw, sh, 30);
    background.addLayer("background/dusk/layer_2.png", sw, sh, 20);
    background.addLayer("background/dusk/layer_3.png", sw, sh, 16);
    background.addLayer("background/dusk/layer_4.png", sw, sh, 6);
    background.addLayer("background/dusk/layer_5.png", sw, sh, 2);
}

/**
 Remove objects at the end of a level or loss
 */
void Hopman::cleanupLevel() {
    // deallocate everything in the drawable list
    for (auto &obj_record : objects) {
        Drawable *obj = obj_record.second;
        if (!isPlayer(obj)) {
            delete obj;
        }
    }
    objects.clear();

    Gui::instance().setGroupDisplay(GuiGroupId::GAME_MESSAGE, false);

    background.shutdown();
}

/**
 Return true if this object is one of the player characters
 */
bool Hopman::isPlayer(Drawable *obj) {
    for (auto &player_state : players) {
        if (player_state.player == obj) {
            return true;
        }
    }
    return false;
}

void Hopman::setGameState(GameState state) {
    if (state == game_state) {
        // no change
        return;
    }

    switch (state) {
        case GameState::LOADING:
            setGameMessage("Loading...");
            break;
        case GameState::RESPAWN:
            setGameMessage("You Died!");
            break;
        case GameState::LOSS:
            setGameMessage("GAME OVER :(");
            Audio::instance().playSound("game_over.wav");
            break;
        case GameState::PLAYING:
            Gui::instance().setGroupDisplay(GuiGroupId::GAME_MESSAGE, false);
            break;
        case GameState::LEVEL_WON:
            setGameMessage("YOU WIN!");
            Audio::instance().playSound("you_win.wav");
            break;
        case GameState::LEVEL_START:
            setGameMessage("Level " + std::to_string(level));
            break;
        default:
            break;
    }
    game_state = state;
}

// PlayerState Definitions

PlayerState::PlayerState(Being *player)
: player(player), assigned(false), state(GameState::LOADING) {}

// Client Definitions

HopmanClient::HopmanClient(std::string server_host)
: server_host(server_host) {}

void HopmanClient::init() {
    Hopman::init();
    client.init(server_host);
}

void HopmanClient::networkUpdate() {
    if (game_state == GameState::LOADING) {
        // send a registration message
        ClientRegisterMsg reg;
        reg.obj_count = objects.size();
        client.sendRegister(reg);
    } else {
        // send an update about the player's action
        updatePlayerInput();
        client.sendInput(player_input);
    }
    
    // process incoming state updates
    int msg_type;
    char buffer[client.msg_buffer_size];
    while (client.getMessage(msg_type, buffer)) {
        switch (buffer[0]) {
            case MSG_TYPE::GAME_STATE: {
                GameStateMsg *state = reinterpret_cast<GameStateMsg*>(buffer);
                if (state->you) {
                    // we got a state update for our client
                    score = state->score;
                    lives = state->lives;
                    level = state->level;
                    setGameState(static_cast<GameState>(state->state));
                } else {
                    //TODO update gui based on the state of other players
                }
                break;
            }
            case MSG_TYPE::OBJECT_STATE: {
                ObjectStateMsg *state = reinterpret_cast<ObjectStateMsg*>(buffer);
                Drawable *obj;
                auto obj_record = objects.find(state->id);
                if (obj_record == objects.end()) {
                    // new object
                    obj = addTile(state->type, 0, 0, state->id);
                    obj->setId(state->id);
                } else {
                    // existing object
                    obj = obj_record->second;
                }
                // update the object with the state info
                obj->updateWithObjectState(*state);
                break;
            }
        }
    }
}

void HopmanClient::updatePlayerInput() {
    player_input.target_x_vel = getPlayer()->getTargetXVel();
    //TODO player_input.jumping = player.is
}

void HopmanClient::handleDeath() {
    // check if the player was killed
    if (getPlayer()->dead() || getPlayer()->needsRemoval()) {
        tryRespawn();
    }
}

void HopmanClient::moveRight() {
    if (game_state != GameState::LOADING) {
        getPlayer()->moveRight();
    }
}

void HopmanClient::stopRight() {
    if (game_state != GameState::LOADING) {
        getPlayer()->stopRight();
    }
}

void HopmanClient::moveLeft() {
    if (game_state != GameState::LOADING) {
        getPlayer()->moveLeft();
    }
}

void HopmanClient::stopLeft() {
    if (game_state != GameState::LOADING) {
        getPlayer()->stopLeft();
    }
}

void HopmanClient::jump() {
    if (game_state != GameState::LOADING) {
        getPlayer()->jump();
    }
}

/**
 register callbacks with the input subsystem that get called on certain actions
 */
void HopmanClient::registerInputCallbacks() {
    // game events
    Input::instance().registerCallback(Action::EXIT_GAME, std::bind(&HopmanClient::exitGame, this));
    Input::instance().registerCallback(Action::ADVACNE, std::bind(&HopmanClient::advanceScreen, this));
    Input::instance().registerCallback(Action::TOGGLE_FPS, std::bind(&HopmanClient::toggleFps, this));
    Input::instance().registerCallback(Action::TOGGLE_PAUSE, std::bind(&HopmanClient::pause, this));
    
    // player movement
    Input::instance().registerCallback(Action::MOVE_LEFT, std::bind(&HopmanClient::moveLeft, this));
    Input::instance().registerCallback(Action::STOP_LEFT, std::bind(&HopmanClient::stopLeft, this));
    Input::instance().registerCallback(Action::MOVE_RIGHT, std::bind(&HopmanClient::moveRight, this));
    Input::instance().registerCallback(Action::STOP_RIGHT, std::bind(&HopmanClient::stopRight, this));
    Input::instance().registerCallback(Action::JUMP, std::bind(&HopmanClient::jump, this));
}

// Server Definitions

void HopmanServer::init() {
    Hopman::init();
    server.init();
    setupLevel();
}

void HopmanServer::networkUpdate() {
    // handle input from clients
    int *player_id;
    int msg_type;
    char buffer[server.msg_buffer_size];
    while (server.getMessage(msg_type, &player_id, buffer)) {
        switch (msg_type) {
            case MSG_TYPE::CLIENT_REGISTER: {
                PlayerState *pstate = NULL;
                for (auto &player_record : players) {
                    if (player_record.player->getId() == *player_id) {
                        pstate = &player_record;
                    }
                }
                if (pstate == NULL) {
                    // new player
                    pstate = addNewPlayer();
                    if (pstate == NULL) {
                        // all players have been assigned
                        return;
                    }
                    *player_id = pstate->player->getId();
                }
                ClientRegisterMsg *reg = reinterpret_cast<ClientRegisterMsg*>(buffer);
                processRegistration(pstate, reg);
                break;
            }
            case MSG_TYPE::CLIENT_INPUT: {
                auto *input = reinterpret_cast<ClientInputMsg*>(buffer);
                // update the player object based on input
                auto obj_record = objects.find(*player_id);
                if (obj_record != objects.end()) {
                    static_cast<Being*>(obj_record->second)->updateWithInput(*input);
                    // handle clicks
                    //TODO
                    /*
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
                        setGameState(GameState::PLAYING);
                    }
                     */
                } // ignore if player is not recognized
                break;
            }
        }
    }

    // send updates to clients
    GameStateMsg gstate;
    for (auto &player_state : players) {
        // dispatch to clients
        gstate.player_id = player_state.player->getId();
        gstate.state = player_state.state;
        gstate.lives = lives;
        gstate.score = score;
        gstate.level = level;
        server.sendGameStateUpdate(gstate);
    }
    ObjectStateMsg ostate;
    for (auto &obj : objects) {
        // dispatch to clients
        obj.second->fillObjectState(ostate);
        server.sendObjectStateUpdate(ostate);
    }
}

void HopmanServer::processRegistration(PlayerState *pstate, ClientRegisterMsg *reg) {
    if (reg->obj_count >= objects.size()) {
        // client is done loading
        pstate->state = GameState::LEVEL_START;
    }
}

PlayerState* HopmanServer::addNewPlayer() {
    // take the first inactive player from the players list
    for (auto &player_state : players) {
        if (!player_state.assigned) {
            player_state.assigned = true;
            return &player_state;
        }
    }
    // all allocated players are taken
    //TODO handle this better
    return NULL;
}

void HopmanServer::handleDeath() {
    //TODO
}

void HopmanServer::registerInputCallbacks() {
    // game events
    Input::instance().registerCallback(Action::EXIT_GAME, std::bind(&HopmanServer::exitGame, this));
}

/**
 Update each client game_state
 Update server's game state based on client's states
 */
void HopmanServer::handleGameState() {
    
}

/**
 Load the next level from config
 */
void HopmanServer::setupLevel() {
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
            // use id = 0 to auto-assign
            addTile(tile_num, tx, ty, 0);
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
    setGameState(GameState::LEVEL_START);
}

/**
 Read a level config file and fill in the passed in config struct
 */
void HopmanServer::parseLevelConfig(LevelConfig &config) {
    // find the largest level file that is <= level
    // this lets you repeat levels by skipping some filenames
    // you must at least provide a level_1 file
    // wanted to read all files from the level directory, but OSX still doesn't
    // have support for std::filesystem
    std::ifstream cfg_stream("");
    int effective_lvl = level;
    while (effective_lvl >= 0 && !cfg_stream.good()) {
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
 Restart the game from the starting level
 */
void HopmanServer::restartGame() {
    // restart game
    level = STARTING_LEVEL;
    score = 0;
    lives = DEFAULT_EXTRA_LIVES;
    setupLevel();
    
    // unpause in case we restarted from the menu
    Gui::instance().setGroupDisplay(GuiGroupId::PAUSE, false);
    paused = false;
}
