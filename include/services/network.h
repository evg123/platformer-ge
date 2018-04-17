//
//  network.h
//  Handle message send/receive over the network
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
#include <limits>
#include <vector>
#include <algorithm>
#include "socket.h"
#include "frame_timer.h"

constexpr int CLIENT_PORT = 2552;
constexpr int SERVER_PORT = 2553;

constexpr long UNSET_SERVER_TIME = std::numeric_limits<long>::max();
constexpr long PREVIOUS_DATAPOINT_FACTOR = 3;
constexpr long DATAPOINTS_PER_UPDATE = 5;

//TODO lots of info sent in these messages that doesn't update very often
//     should split more static info into new, less frequent msg types

enum MSG_TYPE {
    // both ways
    TIME_SYNC,
    
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

struct TimeSyncMsg {
    Header hdr;
    long client_ts;
    long server_ts;
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
    long jump_start_ts;
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
    long server_clock_offset;
    std::vector<long> server_time_datapoints;
    bool accurate_time_sync;
public:
    static const ssize_t msg_buffer_size = std::max({sizeof(TimeSyncMsg),
                                                     sizeof(GameStateMsg),
                                                     sizeof(ObjectStateMsg)});
    void init(std::string server_addr);
    void shutdown();
    void sendTimeSync();
    void sendRegister(ClientRegisterMsg &reg);
    void sendInput(ClientInputMsg &input);
    bool getMessage(int &msg_type, char *buffer);
    void updateServerTime(TimeSyncMsg *msg);
    /** True if we have done a full calculation of server time offset */
    bool hasAccurateSync() { return accurate_time_sync; }
};

class Server {
private:
    Socket sock;
    std::map<unsigned int, ClientRecord*> addr_to_client;
public:
    static const ssize_t msg_buffer_size = std::max({sizeof(TimeSyncMsg),
                                                     sizeof(ClientRegisterMsg),
                                                     sizeof(ClientInputMsg)});
    void init();
    void shutdown();
    void sendTimeSync(TimeSyncMsg &msg, ClientRecord *record);
    void sendGameStateUpdate(GameStateMsg &state);
    void sendObjectStateUpdate(ObjectStateMsg &state, std::set<int> *player_ids);
    bool getMessage(int &msg_type, int **player_id, char *buffer);
};

#endif /* network_h */
