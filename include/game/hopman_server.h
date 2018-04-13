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
#include <set>
#include "hopman.h"
#include "network.h"

constexpr int NETWORK_UPDATE_INTERVAL = 50;

class HopmanServer : public Hopman {
private:
    Server server;
    int last_net_update;
    
    void init() override;
    int play() override;
    PlayerState* addNewPlayer();
    void processRegistration(PlayerState *pstate, ClientRegisterMsg *reg);
    void handleGameState() override;
    void networkUpdate() override;
    bool shouldSendNetworkUpdate();
    void handleDeath() override;
    void registerInputCallbacks() override;
    void setupLevel();
    void parseLevelConfig(LevelConfig &config);
    void restartGame();
public:
};

#endif /* hopman_server_h */
