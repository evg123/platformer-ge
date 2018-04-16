//
//  hopman_client.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 4/8/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "hopman_client.h"

/**
 Constructor
 */
HopmanClient::HopmanClient(std::string server_host)
: Hopman(false), server_host(server_host), client_state_history(STATE_HISTORY_SIZE) {}

/**
 Set up the game to run as a client
 */
void HopmanClient::init() {
    Hopman::init();
    // create a player for this client
    // id will be filled in when we register with the server
    addTile(TileNum::PLAYER, 0, 0, -1);
    client.init(server_host);
    player_input.clicked = false;
    player_input.jumped = false;
    for (auto &state : client_state_history) {
        state.ts = 0;
    }
    state_history_head = 0;
    should_save_client_state = true;
    need_to_load = true;
    last_time_sync = 0;
}

/**
 Main gameplay loop
 */
int HopmanClient::play() {
    registerInputCallbacks();
    createUI();
    
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
            updatePlayer(delta);
        }
        
        updateNpcs(delta);

        // focus the screen on the player
        Graphics::instance().focusScreenOffsets(getPlayer()->getRect().getCollider());
        background.updateLayerOffsets(getPlayer()->getRect().xPos(),
                                      getPlayer()->getRect().yPos());

        // update the GUI
        Gui::instance().update();

        // draw the new frame
        render();

        // send/receive updates on the network
        networkUpdate();
    }

    return 0;
}

/**
 Update the player being object
 Players will be updated as input is received from their clients
 */
void HopmanClient::updatePlayer(long delta) {
    Being *obj = getPlayer();
    obj->update(delta, objects);
    // check if obj has fallen off the map
    if (obj->getRect().top() > lower_bound && !obj->needsRemoval()) {
        obj->destroy();
    }
    
    // clean up objects that need to be removed from the game
    removeDestroyed();
}

/**
 Update the state of all non-player objects
 */
void HopmanClient::updateNpcs(long delta) {
    for (auto &obj_rec : objects) {
        if (getPlayerState(obj_rec.second) == NULL) {
            obj_rec.second->updateState(delta);
        }
    }
}

/**
 See if it is time to get a new clock sync reading from the server
 */
bool HopmanClient::shouldTimeSync() {
    long now = getTime();
    if (now - last_time_sync >= TIME_SYNC_INTERVAL) {
        last_time_sync = now;
        return true;
    }
    return false;
}

/**
 Send and receive messages to/from the server
 */
void HopmanClient::networkUpdate() {
    if (shouldTimeSync()) {
        client.sendTimeSync();
    }
    if (getPlayerGameState() == GameState::LOADING) {
        // send a registration message
        ClientRegisterMsg reg;
        reg.obj_count = objects.size() - players.size();
        reg.need_to_load = need_to_load;
        client.sendRegister(reg);
    } else {
        // send an update about the player's action
        updatePlayerInput();
        client.sendInput(player_input);
        player_input.clicked = false;
        player_input.jumped = false;
    }
    
    // process incoming state updates
    int msg_type;
    char buffer[client.msg_buffer_size];
    while (client.getMessage(msg_type, buffer)) {
        switch (msg_type) {
            case MSG_TYPE::GAME_STATE: {
                GameStateMsg *state = reinterpret_cast<GameStateMsg*>(buffer);
                PlayerState *pstate;
                if (state->you) {
                    // we got a state update for our client
                    pstate = getPlayerState();
                    pstate->lives = state->lives;
                    score = state->score;
                    level = state->level;
                    if (lower_bound != state->lower_bound) {
                        lower_bound = state->lower_bound;
                        createBackground();
                    }
                    if (need_to_load) {
                        if (state->state == GameState::LOADING) {
                            // server has accepted our request to load
                            need_to_load = false;
                        }
                    } else {
                        setGameState(pstate, static_cast<GameState>(state->state));
                    }
                } else {
                    if (!state->assigned) {
                        // player hasn't been assigned yet, ignore
                        break;
                    }
                    // update gui based on the state of other players
                    for (int pidx = 0; pidx < players.size(); pidx++) {
                        if (players[pidx]->player->getId() == state->player_id) {
                            pstate = players[pidx];
                            if (pstate->active_state == state->state) {
                                // no update
                                break;
                            }
                            std::string new_status;
                            switch (state->state) {
                                case GameState::LOADING:
                                    new_status = "Loading";
                                    break;
                                case GameState::LOSS:
                                    new_status = "GameOver";
                                    break;
                                case GameState::LEVEL_START:
                                    new_status = "Starting";
                                    break;
                                case GameState::RESPAWN:
                                    new_status = "Dead";
                                    break;
                                default:
                                    new_status = "Playing";
                                    break;
                            }
                            player_status_strs.at(pidx).replace(PLAYER_STATUS_DESCRIPTION_START,
                                                                PLAYER_STATUS_MAX_LEN,
                                                                new_status);
                            setGameState(pstate, static_cast<GameState>(state->state));
                            break;
                        }
                    }
                }
                break;
            }
            case MSG_TYPE::OBJECT_STATE: {
                ObjectStateMsg *state = reinterpret_cast<ObjectStateMsg*>(buffer);
                if (state->you) {
                    handlePlayerObjectState(state);
                    break;
                }
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

/**
 Handle an update from the server on our player object
 */
void HopmanClient::handlePlayerObjectState(ObjectStateMsg *state) {
    Being *player = getPlayer();
    if (player->getId() != state->id) {
        player->setId(state->id);
    }
    if (objects.find(player->getId()) == objects.end()) {
        objects[state->id] = player;
    }
    for (int idx = state_history_head; idx < state_history_head + STATE_HISTORY_SIZE; idx++) {
        ClientStateRecord *csr = &client_state_history[idx % STATE_HISTORY_SIZE];
        if (csr->ts == 0) {
            // not filled in yet, we've reached the end
            break;
        }
        if (csr->ts <= state->hdr.ts) {
            if (doStatesDiffer(&csr->state, state)) {
                // start replaying from this saved state
                replayClientHistory(player, state, idx);
            }
            // we found the corresponding csr
            // whether or not we needed to replay, we are done
            return;
        }
    }
    // we don't have state that goes back this far, ignore the update
    return;
}

/**
 True if two states differe by a minumum threshold
 */
bool HopmanClient::doStatesDiffer(ObjectStateMsg *state1, ObjectStateMsg *state2) {
    return (std::abs(state1->xpos - state2->xpos) > MIN_SMOOTH_POS ||
            std::abs(state1->ypos - state2->ypos) > MIN_SMOOTH_POS ||
            state1->hp != state2->hp);
        
}

/**
 Using state as the starting point, replay our inputs and get a new state.
 This is called when we have gotten out of sync with the server and receive an update
 that conflicts with our view of out state.
 */
void HopmanClient::replayClientHistory(Being *player, ObjectStateMsg *state, int history_idx) {
    ClientStateRecord *csr = &client_state_history[history_idx % STATE_HISTORY_SIZE];
    // start with the first state
    player->updateWithObjectState(*state);
    long prev_state_ts = state->hdr.ts;
    player->updateWithInput(csr->input);
    for (int idx = history_idx - 1; idx >= state_history_head; idx--) {
        csr = &client_state_history[idx % STATE_HISTORY_SIZE];
        long diff = csr->ts - prev_state_ts;
        player->update(diff, objects);
        // update the remembered state object with the adjusted state at that ts
        player->fillObjectState(csr->state);
        prev_state_ts = csr->ts;
        player->updateWithInput(csr->input);
    }
    long diff = timer.getFrameStart() - prev_state_ts;
    // update to the current time
    player->update(diff, objects);
    //TODO dont accept updates before prev_state_ts
}

/**
 Update the player input msg and get it ready to send
 */
void HopmanClient::updatePlayerInput() {
    player_input.target_x_vel = getPlayer()->getTargetXVel();
    if (should_save_client_state) {
        --state_history_head;
        if (state_history_head < 0) {
            state_history_head = STATE_HISTORY_SIZE - 1;
        }
        client_state_history[state_history_head].ts = timer.getFrameStart();
        client_state_history[state_history_head].input = player_input;
        getPlayer()->fillObjectState(client_state_history[state_history_head].state);
    }
}

/**
 Not currently used
 */
void HopmanClient::handleDeath() {
    // check if the player was killed
    /*
    if (getPlayer()->dead() || getPlayer()->needsRemoval()) {
        tryRespawn(getPlayer());
    }
     */
}

/**
 return the active state of the player
 */
GameState HopmanClient::getPlayerGameState() {
    if (players.size() < 1) {
        // haven't been assigned a player yet
        return GameState::LOADING;
    }
    return getPlayerState()->active_state;
}

/**
 Update the active state of the player
 */
void HopmanClient::setGameState(PlayerState *pstate, GameState gstate) {
    if (pstate == getPlayerState() && pstate->active_state != gstate) {
        // handle state transition for our client
        if (gstate == GameState::LOADING) {
            // the first time we eneter loading we need to clear out our objects
            cleanupLevel();
        } else if (gstate == GameState::LEVEL_START) {
            // keep loading until we have synced server time
            if (!client.hasAccurateSync()) {
                return;
            }
        }
    }
    Hopman::setGameState(pstate, gstate);
}

/**
 Check if the user is ready to move on from the end of a level / loss
 */
void HopmanClient::advanceScreenCallback() {
    player_input.clicked = true;
}

/**
 Signal that we want to move right
 */
void HopmanClient::moveRight() {
    if (getPlayerGameState() == GameState::PLAYING) {
        getPlayer()->moveRight();
        should_save_client_state = true;
    }
}

/**
 Signal that we want to stop moving to the right
 Called when a right key is released
 */
void HopmanClient::stopRight() {
    getPlayer()->stopRight();
    should_save_client_state = true;
}

/**
 Signal that we want to move left
 */
void HopmanClient::moveLeft() {
    if (getPlayerGameState() == GameState::PLAYING) {
        getPlayer()->moveLeft();
        should_save_client_state = true;
    }
}

/**
 Signal that we want to stop moving left.
 Called when a left key is released
 */
void HopmanClient::stopLeft() {
    getPlayer()->stopLeft();
    should_save_client_state = true;
}

/**
 Signal that we want to start a jump
 */
void HopmanClient::jump() {
    if (getPlayerGameState() == GameState::PLAYING) {
        player_input.jumped = true;
        getPlayer()->jump();
        should_save_client_state = true;
    }
}

/**
 Register callbacks with the input subsystem that get called on certain actions
 */
void HopmanClient::registerInputCallbacks() {
    // game events
    Input::instance().registerCallback(Action::EXIT_GAME, std::bind(&HopmanClient::exitGame, this));
    Input::instance().registerCallback(Action::ADVACNE, std::bind(&HopmanClient::advanceScreenCallback, this));
    Input::instance().registerCallback(Action::TOGGLE_FPS, std::bind(&HopmanClient::toggleFps, this));
    Input::instance().registerCallback(Action::TOGGLE_PAUSE, std::bind(&HopmanClient::togglePause, this));
    
    // player movement
    Input::instance().registerCallback(Action::MOVE_LEFT, std::bind(&HopmanClient::moveLeft, this));
    Input::instance().registerCallback(Action::STOP_LEFT, std::bind(&HopmanClient::stopLeft, this));
    Input::instance().registerCallback(Action::MOVE_RIGHT, std::bind(&HopmanClient::moveRight, this));
    Input::instance().registerCallback(Action::STOP_RIGHT, std::bind(&HopmanClient::stopRight, this));
    Input::instance().registerCallback(Action::JUMP, std::bind(&HopmanClient::jump, this));
}
