//
//  Created by Vande Griek, Eric on 4/8/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "hopman_server.h"

/**
 Constructur
 */
HopmanServer::HopmanServer(bool headless) : Hopman(headless) {}

/**
 Set up the game in dedicated server mode
 */
void HopmanServer::init() {
    last_net_update = 0;
    Hopman::init();
    Audio::instance().setEnabled(false);
    setupLevel();
    togglePause();
    server.init();
}

/**
 Main gameplay loop
 */
int HopmanServer::play() {
    registerInputCallbacks();
    if (!headless) {
        createUI();
    }

    while (true) {
        // wait until it is time to render the next frame
        timer.delayUntilNextFrame();

        fps_display = timer.getFps();

        // signal a new frame to the fps timer and get the delta since the last frame
        long delta = timer.newFrame();

        // tell the input singleton to poll for events
        Input::instance().handleEvents();
        if (!running) {
            break;
        }

        // update game objects
        if (!paused) {
            update(delta);
        }

        if (!headless) {
            // focus the screen on the player
            Graphics::instance().focusScreenOffsets(getPlayer()->getRect().getCollider());

            // update the GUI
            Gui::instance().update();

            // draw the new frame
            render();
        }

        // update overall game state based on client states
        handleGameState();

        // send/receive updates on the network
        sendNetworkUpdates();
        listenNetworkUpdates();
    }

    return 0;
}

/**
 Update all object
 */
void HopmanServer::update(long delta) {
    for (auto &obj_record : objects) {
        Drawable *obj = obj_record.second;
        obj->update(delta, objects);
        // check if obj has fallen off the map
        if (obj->getRect().top() > lower_bound && !obj->needsRemoval()) {
            obj->destroy();
        }
    }
    
    // clean up objects that need to be removed from the game
    removeDestroyed();
}

/**
 Send and receive messages to/from clients
 Also listen for new clients
 */
void HopmanServer::sendNetworkUpdates() {
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
                    // now that the player is assigned, add it to the objects list
                    objects[*player_id] = pstate->player;
                }
                ClientRegisterMsg *reg = reinterpret_cast<ClientRegisterMsg*>(buffer);
                processRegistration(pstate, reg);
                break;
            }
            case MSG_TYPE::CLIENT_INPUT: {
                if (pstate == NULL) {
                    // ignore unrecognized clients until they register
                    break;
                }
                auto *input = reinterpret_cast<ClientInputMsg*>(buffer);
                // update the player object based on input
                Being *player = pstate->player;
                if (pstate->active_state == GameState::PLAYING) {
                    // only update player if we are playing
                    player->updateWithInput(*input);
                }
                // handle clicks
                if (input->clicked) {
                    advanceScreen(pstate);
                }
                pstate->last_update = input->hdr.ts;
                break;
            }
        }
    }
}

/**
 Listen for messages from clients
 Register new clients
 */
void HopmanServer::listenNetworkUpdates() {
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
            gstate.assigned = player_state->assigned;
            gstate.lives = player_state->lives;
            gstate.score = score;
            gstate.level = level;
            gstate.lower_bound = lower_bound;
            server.sendGameStateUpdate(gstate);
        }
        ObjectStateMsg ostate;
        for (auto &obj : objects) {
            PlayerState *pstate = getPlayerState(obj.second);
            if (pstate != NULL && !pstate->assigned) {
                // player has not been assigned yet, don't send
                continue;
            }
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

/**
 True if it is time to send another round of updates to clients
 */
bool HopmanServer::shouldSendNetworkUpdate() {
    long now = getTime();
    long diff = now - last_net_update;
    if (diff > NETWORK_UPDATE_INTERVAL) {
        last_net_update = now;
        return true;
    }
    return false;
}

/**
 Handle a new client registration
 */
void HopmanServer::processRegistration(PlayerState *pstate, ClientRegisterMsg *reg) {
    if (reg->need_to_load) {
        // client is requesting we set it to loading mode
        // could be a reconnection
        setGameState(pstate, GameState::LOADING);
    } else if (reg->obj_count >= objects.size() - players.size()) {
        // client is done loading
        setGameState(pstate, GameState::LEVEL_START);
    }
}

/**
 Assign a player to a new client
 */
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

/**
 Update players that have died
 */
void HopmanServer::handleDeath() {
    // check if players were killed
    for (auto &pstate : players) {
        if (pstate->player->dead() || pstate->player->needsRemoval()) {
            tryRespawn(pstate);
        }
    }
}

/**
 Register callbacks for when keys are pressed
 */
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
        for (auto &pstate : players) {
            // give every player an extra life
            // we will bring back dead players as well
            if (!pstate->player->dead()) {
                ++pstate->lives;
            }
        }
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
