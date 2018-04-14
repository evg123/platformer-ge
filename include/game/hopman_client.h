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
#include <vector>
#include "hopman.h"
#include "network.h"

constexpr int STATE_SAVE_INTERVAL = 100;
constexpr int STATE_HISTORY_SIZE = 20;

struct ClientStateRecord {
    long ts = 0;
    ClientInputMsg input;
    ObjectStateMsg state;
};

class HopmanClient : public Hopman {
private:
    Client client;
    ClientInputMsg player_input;
    std::string server_host;
    GameState requested_game_state;
    std::vector<ClientStateRecord> client_state_history;
    int state_history_head;
    bool should_save_client_state;
    bool need_to_load;
    
    void init() override;
    int play() override;
    void updatePlayer(long delta);
    void updateNpcs(long delta);
    GameState getPlayerGameState();
    void networkUpdate() override;
    void handlePlayerObjectState(ObjectStateMsg *state);
    bool doStatesDiffer(ObjectStateMsg *state1, ObjectStateMsg *state2);
    void replayClientHistory(Being *player, ObjectStateMsg *state, int history_idx);
    void updatePlayerInput();
    void handleDeath() override;
    void registerInputCallbacks() override;
    void setGameState(PlayerState *pstate, GameState state) override;

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
