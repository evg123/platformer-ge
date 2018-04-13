//
//  hopman_server.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 4/8/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "hopman_server.h"

void HopmanServer::init() {
    last_net_update = 0;
    Hopman::init();
    setupLevel();
    togglePause();
    server.init();
}

/**
 Main gameplay loop
 */
int HopmanServer::play() {
    registerInputCallbacks();
    createUI();
    createBackground();
    
    FrameTimer timer = FrameTimer(fps_limit);
    
    while (true) {
        // wait until it is time to render the next frame
        timer.delayUntilNextFrame();
        
        fps_display = timer.getFps();
        
        // signal a new frame to the fps timer and get the delta since the last frame
        int delta = timer.newFrame();
        
        // tell the input singleton to poll for events
        Input::instance().handleEvents();
        if (!running) {
            break;
        }
        
        // update game objects
        if (!paused) {
            update(delta, false);
            
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

void HopmanServer::networkUpdate() {
    // handle input from clients
    int *player_id;
    int msg_type;
    char buffer[server.msg_buffer_size];
    while (server.getMessage(msg_type, &player_id, buffer)) {
        PlayerState *pstate = NULL;
        for (auto &player_record : players) {
            if (player_record->player->getId() == *player_id) {
                pstate = player_record;
            }
        }
        switch (msg_type) {
            case MSG_TYPE::CLIENT_REGISTER: {
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
                //TODO drop inputs that are out of date
                // update the player object based on input
                auto obj_record = objects.find(*player_id);
                if (obj_record != objects.end()) {
                    Being *player = static_cast<Being*>(obj_record->second);
                    int delta;
                    if (player->getLastUpdate() == 0) {
                        // first update, use 0
                        delta = 0;
                    } else {
                        // cast delta to int, if its too big then something else is wrong anyway
                        delta = static_cast<int>(input->ts - player->getLastUpdate());
                    }
                    player->updateWithInput(*input);
                    player->update(delta, objects);
                    // handle clicks
                    if (input->clicked) {
                        advanceScreen(pstate);
                    }
                } // ignore if player is not recognized
                break;
            }
        }
    }
    
    // send updates to clients periodically
    if (shouldSendNetworkUpdate()) {
        std::set<int> loading_player_ids;
        GameStateMsg gstate;
        for (auto &player_state : players) {
            if (!player_state->assigned) {
                continue;
            }
            if (player_state->active_state == GameState::LOADING) {
                loading_player_ids.insert(player_state->player->getId());
            }
            // dispatch to clients
            gstate.player_id = player_state->player->getId();
            gstate.state = player_state->active_state;
            gstate.lives = player_state->lives;
            gstate.score = score;
            gstate.level = level;
            gstate.lower_bound = lower_bound;
            server.sendGameStateUpdate(gstate);
        }
        ObjectStateMsg ostate;
        for (auto &obj : objects) {
            // dispatch to clients
            obj.second->fillObjectState(ostate);
            if (obj.second->isStatic()) {
                server.sendObjectStateUpdate(ostate, &loading_player_ids);
            } else {
                server.sendObjectStateUpdate(ostate, NULL);
            }
        }
    }
}

bool HopmanServer::shouldSendNetworkUpdate() {
    int now = SDL_GetTicks();
    int diff = now - last_net_update;
    if (diff > NETWORK_UPDATE_INTERVAL) {
        last_net_update = now;
        return true;
    }
    return false;
}

void HopmanServer::processRegistration(PlayerState *pstate, ClientRegisterMsg *reg) {
    if (reg->obj_count >= objects.size()) {
        // client is done loading
        pstate->active_state = GameState::LEVEL_START;
    }
}

PlayerState* HopmanServer::addNewPlayer() {
    // take the first inactive player from the players list
    for (auto &player_state : players) {
        if (!player_state->assigned) {
            player_state->assigned = true;
            return player_state;
        }
    }
    // all allocated players are taken
    //TODO handle this better
    return NULL;
}

void HopmanServer::handleDeath() {
    // check if players were killed
    for (auto &pstate : players) {
        if (pstate->player->dead() || pstate->player->needsRemoval()) {
            tryRespawn(pstate);
        }
    }
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
    bool all_ready = true;
    bool all_lost = true;
    bool any_assigned = false;
    bool any_won = false;
    bool any_playing = false;
    for (auto &pstate : players) {
        if (!pstate->assigned) {
            // only look at active players
            continue;
        }
        any_assigned = true;
        all_ready &= pstate->ready;
        all_lost &= pstate->active_state == GameState::LOSS;
        any_won |= pstate->active_state == GameState::LEVEL_WON;
        any_playing |= pstate->active_state == GameState::PLAYING;
    }
    if (!any_assigned) {
        // don't do anything if no one is connected
        return;
    } else if (all_lost && all_ready) {
        // we can restart the level completely
        setAllGameStates(GameState::LOADING);
        restartGame();
    } else if (any_won && all_ready) {
        // we can move to the next level
        ++level;
        setAllGameStates(GameState::LOADING);
        setupLevel();
    } else if (paused && any_playing) {
        // someone started playing, unpause the level
        togglePause();
    }
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
    
    // start the background track for the level
    //TODO make custom for each level
    //Audio::instance().setBgTrack(BG_TRACK);
    
    // start out on the level start screen
    //setAllGameStates(GameState::LEVEL_START); should already be loading
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
    for (PlayerState *pstate : players) {
        pstate->lives = DEFAULT_EXTRA_LIVES;
    }
    setupLevel();
    
    // unpause in case we restarted from the menu
    Gui::instance().setGroupDisplay(GuiGroupId::PAUSE, false);
    paused = false;
}
