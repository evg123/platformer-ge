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
