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
#include "frame_timer.h"

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

struct Header {
    int type;
    long ts;
};

struct ClientRegisterMsg {
    Header hdr;
    size_t obj_count;
    bool need_to_load;
};

struct ClientInputMsg {
    Header hdr;
    float target_x_vel;
    bool jumped;
    bool clicked;
};

struct GameStateMsg {
    Header hdr;
    int player_id;
    bool you;
    int state;
    int assigned;
    int lives;
    int score;
    int level;
    int lower_bound;
};

struct ObjectStateMsg {
    Header hdr;
    bool you;
    int id;
    int type;
    int xpos, ypos;
    float xvel, yvel;
    bool intangible;
    int hp;
    bool marked_for_removal;
    int air_jumps;
    bool on_ground;
};

struct ClientRecord {
    int player_id;
    unsigned int client_addr;
    unsigned short client_port;
    long latest_msg_ts;
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
