//
//  hopman_server.h
//  platformer
//
//  Created by Vande Griek, Eric on 4/8/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef hopman_server_h
#define hopman_server_h

#include <string>
#include "hopman.h"
#include "network.h"

class HopmanServer : public Hopman {
private:
    Server server;
    
    void init() override;
    int play() override;
    PlayerState* addNewPlayer();
    void processRegistration(PlayerState *pstate, ClientRegisterMsg *reg);
    void handleGameState() override;
    void networkUpdate() override;
    void handleDeath() override;
    void registerInputCallbacks() override;
    void setupLevel();
    void parseLevelConfig(LevelConfig &config);
    void restartGame();
public:
};

#endif /* hopman_server_h */
