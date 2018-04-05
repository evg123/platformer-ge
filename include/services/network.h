//
//  network.h
//  platformer
//
//  Created by Vande Griek, Eric on 4/3/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef network_h
#define network_h

#include <string>
#include <map>
#include <arpa/inet.h>
#include "socket.h"

constexpr int CLIENT_PORT = 2552;
constexpr int SERVER_PORT = 2553;

struct ClientInput {
    int ts;
    float target_x_vel;
    bool jumping;
};

struct State {
    int id;
    int type;
    bool active;
    int xpos, ypos;
    int xvel, yvel;
    int hp;
    bool marked_for_removal;
    int air_jumps;
};

struct ClientRecord {
    int player_id;
    unsigned int client_addr;
    unsigned short client_port;
public:
    
};

class Client {
private:
    Socket sock;
    unsigned int server_addr;
public:
    void init(std::string server_addr);
    void shutdown();
    void sendInput(ClientInput &input);
    bool getMessage(State &state);
};

class Server {
private:
    Socket sock;
    std::map<unsigned int, ClientRecord*> addr_to_client;
public:
    void init();
    void shutdown();
    void sendStateUpdate(State &state);
    bool getInput(int &player_id, ClientInput &input);
};

#endif /* network_h */
