//
//  network.h
//  platformer
//
//  Created by Vande Griek, Eric on 4/3/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef network_h
#define network_h

#include <map>
#include "socket.h"
#include "drawable.h"
#include "being.h"

struct Input {
    int ts;
    float target_x_vel;
    bool jumping;
};

struct State {
    int id;
    bool active;
    int xpos, ypos;
    int xvel, yvel;
    int hp;
    bool marked_for_removal;
    int air_jumps;
};

class ClientRecord {
private:
    int player_id;
    unsigned int client_addr;
public:
    
};

class Client {
private:
    Socket sock;
    unsigned int server_addr;
public:
    void init();
    void shutdown();
    void sendInput(Input &input);
    bool getMessage(State &state);
};

class Server {
private:
    Socket sock;
    std::map<unsigned int, ClientRecord*> addr_to_client;
public:
    void init();
    void shutdown();
    bool sendStateUpdate(State &state);
    bool getInput(int &player_id, Input &input);
};

#endif /* network_h */
