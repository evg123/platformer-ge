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
#include <chrono>
#include <set>
#include "socket.h"

constexpr int CLIENT_PORT = 2552;
constexpr int SERVER_PORT = 2553;

//TODO lots of info sent in these messages that doesn't update very often
//     should split more static info into new, less frequent msg types

enum MSG_TYPE {
    // server -> client
    GAME_STATE,
    OBJECT_STATE,

    // client -> server
    CLIENT_REGISTER,
    CLIENT_INPUT,
};

struct Msg {
    int type;
};

struct ClientRegisterMsg {
    Msg msg;
    long ts;
    size_t obj_count;
};

struct ClientInputMsg {
    Msg msg;
    long ts;
    float target_x_vel;
    bool jumped;
    bool clicked;
};

struct GameStateMsg {
    Msg msg;
    long ts;
    int player_id;
    bool you;
    int state;
    int lives;
    int score;
    int level;
    int lower_bound;
};

struct ObjectStateMsg {
    Msg msg;
    long ts;
    bool you;
    int id;
    int type;
    bool active;
    int xpos, ypos;
    int xvel, yvel;
    int hp;
    bool marked_for_removal;
    int air_jumps;
    bool on_ground;
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
    unsigned short server_port;
public:
    static const ssize_t msg_buffer_size = std::max(sizeof(GameStateMsg), sizeof(ObjectStateMsg));
    void init(std::string server_addr);
    void shutdown();
    void sendRegister(ClientRegisterMsg &reg);
    void sendInput(ClientInputMsg &input);
    bool getMessage(int &msg_type, char *buffer);
};

class Server {
private:
    Socket sock;
    std::map<unsigned int, ClientRecord*> addr_to_client;
public:
    static const ssize_t msg_buffer_size = std::max(sizeof(ClientRegisterMsg), sizeof(ClientInputMsg));
    void init();
    void shutdown();
    void sendGameStateUpdate(GameStateMsg &state);
    void sendObjectStateUpdate(ObjectStateMsg &state, std::set<int> *player_ids);
    bool getMessage(int &msg_type, int **player_id, char *buffer);
};

#endif /* network_h */
