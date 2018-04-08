//
//  hopman_client.h
//  platformer
//
//  Created by Vande Griek, Eric on 4/8/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef hopman_client_h
#define hopman_client_h

#include <string>
#include "hopman.h"
#include "network.h"

class HopmanClient : public Hopman {
private:
    Client client;
    ClientInputMsg player_input;
    std::string server_host;
    GameState requested_game_state;
    
    void init() override;
    int play() override;
    GameState getPlayerGameState();
    void networkUpdate() override;
    void updatePlayerInput();
    void handleDeath() override;
    void registerInputCallbacks() override;

    void advanceScreenCallback();
    void moveRight();
    void stopRight();
    void moveLeft();
    void stopLeft();
    void jump();
public:
    HopmanClient(std::string server_host);
};

#endif /* hopman_client_h */
