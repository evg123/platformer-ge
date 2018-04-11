//
//  hopman_client.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 4/8/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "hopman_client.h"

HopmanClient::HopmanClient(std::string server_host)
: server_host(server_host) {}

void HopmanClient::init() {
    Hopman::init();
    // create a player for this client
    // id will be filled in when we register with the server
    addTile(TileNum::PLAYER, 0, 0, -1);

    client.init(server_host);
}

/**
 Main gameplay loop
 */
int HopmanClient::play() {
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
        if (!running) {
            break;
        }

        // update game objects
        if (getPlayerGameState() == GameState::PLAYING && !paused) {
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

void HopmanClient::networkUpdate() {
    if (getPlayerGameState() == GameState::LOADING) {
        // send a registration message
        ClientRegisterMsg reg;
        reg.obj_count = objects.size();
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
                PlayerState *pstate = getPlayerState();
                if (state->you && pstate != NULL) {
                    // we got a state update for our client
                    pstate->lives = state->lives;
                    score = state->score;
                    level = state->level;
                    setGameState(pstate, static_cast<GameState>(state->state));
                } else {
                    //TODO update gui based on the state of other players
                }
                break;
            }
            case MSG_TYPE::OBJECT_STATE: {
                ObjectStateMsg *state = reinterpret_cast<ObjectStateMsg*>(buffer);
                Drawable *obj;
                if (state->you) {
                    obj = getPlayer();
                    if (obj->getId() != state->id) {
                        obj->setId(state->id);
                    }
                } else {
                    auto obj_record = objects.find(state->id);
                    if (obj_record == objects.end()) {
                        // new object
                        obj = addTile(state->type, 0, 0, state->id);
                        obj->setId(state->id);
                    } else {
                        // existing object
                        obj = obj_record->second;
                    }
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
}

void HopmanClient::handleDeath() {
    // check if the player was killed
    if (getPlayer()->dead() || getPlayer()->needsRemoval()) {
        //tryRespawn(players[0]);
    }
}

GameState HopmanClient::getPlayerGameState() {
    if (players.size() < 1) {
        // haven't been assigned a player yet
        return GameState::LOADING;
    }
    return getPlayerState()->active_state;
}

void HopmanClient::setGameState(PlayerState *pstate, GameState gstate) {
    Hopman::setGameState(pstate, gstate);
    if (pstate == getPlayerState() && pstate->active_state != gstate) {
        // handle state transition for our client
        if (gstate == GameState::LOADING) {
            // the first time we eneter loading we need to clear out our objects
            cleanupLevel();
        }
    }
}

/**
 Check if the user is ready to move on from the end of a level / loss
 */
void HopmanClient::advanceScreenCallback() {
    player_input.clicked = true;
    //advanceScreen(getPlayerState());
}

void HopmanClient::moveRight() {
    if (getPlayerGameState() != GameState::LOADING) {
        getPlayer()->moveRight();
    }
}

void HopmanClient::stopRight() {
    if (getPlayerGameState() != GameState::LOADING) {
        getPlayer()->stopRight();
    }
}

void HopmanClient::moveLeft() {
    if (getPlayerGameState() != GameState::LOADING) {
        getPlayer()->moveLeft();
    }
}

void HopmanClient::stopLeft() {
    if (getPlayerGameState() != GameState::LOADING) {
        getPlayer()->stopLeft();
    }
}

void HopmanClient::jump() {
    if (getPlayerGameState() != GameState::LOADING) {
        player_input.jumped = true;
        getPlayer()->jump();
    }
}

/**
 register callbacks with the input subsystem that get called on certain actions
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
