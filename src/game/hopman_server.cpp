//
//  hopman_server.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 4/8/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "hopman_server.h"

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
    //Audio::instance().setBgTrack(BG_TRACK);
    
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
