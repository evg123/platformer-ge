//
//  network.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 4/3/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "network.h"

// Client definitions

void Client::init(std::string server_addr_str) {
    server_port = SERVER_PORT;
    sockaddr_in addr;
    inet_pton(AF_INET, server_addr_str.c_str(), &(addr.sin_addr));
    this->server_addr = addr.sin_addr.s_addr;
    sock.open(CLIENT_PORT);
    server_clock_offset = UNSET_SERVER_TIME;
    accurate_time_sync = false;
}

void Client::shutdown() {
    sock.close();
}

void Client::sendTimeSync() {
    TimeSyncMsg sync_msg;
    sync_msg.hdr.type = MSG_TYPE::TIME_SYNC;
    sync_msg.hdr.ts = getTime();
    sync_msg.client_ts = sync_msg.hdr.ts;
    sock.send(server_addr, server_port, &sync_msg, sizeof(sync_msg));
}

void Client::sendRegister(ClientRegisterMsg &reg) {
    reg.hdr.type = MSG_TYPE::CLIENT_REGISTER;
    reg.hdr.ts = getTime();
    sock.send(server_addr, server_port, &reg, sizeof(reg));
}

void Client::sendInput(ClientInputMsg &input) {
    input.hdr.type = MSG_TYPE::CLIENT_INPUT;
    input.hdr.ts = getTime();
    sock.send(server_addr, server_port, &input, sizeof(input));
}

bool Client::getMessage(int &msg_type, char *buffer) {
    ssize_t bytes = sock.receive(NULL, NULL, buffer, msg_buffer_size);
    if (bytes <= 0) {
        return false;
    }

    Header *msg_header = reinterpret_cast<Header*>(buffer);

    // figure out the type of the message
    msg_type = msg_header->type;
    
    // adjust msg timestamp based on server clock offset measurement
     msg_header->ts += server_clock_offset;
    
    if (msg_type == MSG_TYPE::TIME_SYNC) {
        // update the server clock offset
        updateServerTime(reinterpret_cast<TimeSyncMsg*>(buffer));
    }

    return true;
}

void Client::updateServerTime(TimeSyncMsg *msg) {
    long now = getTime();
    long rtt = now - msg->client_ts;
    long server_time_datapoint = now - msg->server_ts - rtt / 2;
    if (server_time_datapoint == UNSET_SERVER_TIME) {
        // use the first value we get, have to start somewhere
        server_clock_offset = server_time_datapoint;
    }
    server_time_datapoints.push_back(server_time_datapoint);
    if (server_time_datapoints.size() >= DATAPOINTS_PER_UPDATE) {
        // use the collected points to update our guess at the server time
        std::sort(server_time_datapoints.begin(), server_time_datapoints.end());
        server_clock_offset = server_time_datapoints[DATAPOINTS_PER_UPDATE / 2];
        //server_clock_offset = (server_clock_offset + new_guess) / 2;
        server_time_datapoints.clear();
        accurate_time_sync = true;
    }
    
}

// Server definitions

void Server::init() {
    sock.open(SERVER_PORT);
}

void Server::shutdown() {
    sock.close();
}

void Server::sendTimeSync(TimeSyncMsg &msg, ClientRecord *record) {
    msg.hdr.ts = getTime();
    msg.server_ts = msg.hdr.ts;
    sock.send(record->client_addr, record->client_port, &msg, sizeof(msg));
}

/**
 Send the given state update to each client
 */
void Server::sendGameStateUpdate(GameStateMsg &state) {
    state.hdr.type = MSG_TYPE::GAME_STATE;
    state.hdr.ts = getTime();
    // send state update to everyone, game state of other players could be displayed
    for (auto &addr_obj : addr_to_client) {
        // set you to true when sent to the client whose state this is
        // this is how the client learns which player they are
        state.you = addr_obj.second->player_id == state.player_id;
        sock.send(addr_obj.first, addr_obj.second->client_port, &state, sizeof(state));
    }
}

void Server::sendObjectStateUpdate(ObjectStateMsg &state, std::set<int> *player_ids) {
    state.hdr.type = MSG_TYPE::OBJECT_STATE;
    state.hdr.ts = getTime();
    for (auto &addr_obj : addr_to_client) {
        int player_id = addr_obj.second->player_id;
        if (player_ids != NULL && player_ids->find(player_id) == player_ids->end()) {
            // not in the set, skip this one
            continue;
        }
        // set you to true when sent to the client whose state this is
        // this is how the client learns which player they are
        state.you = addr_obj.second->player_id == state.id;
        sock.send(addr_obj.first, addr_obj.second->client_port, &state, sizeof(state));
    }
}

bool Server::getMessage(int &msg_type, int **player_id, char *buffer) {
    unsigned int sender_addr;
    unsigned short sender_port;
    ssize_t bytes = sock.receive(&sender_addr, &sender_port, buffer, msg_buffer_size);
    if (bytes <= 0) {
        return false;
    }

    Header *msg_header = reinterpret_cast<Header*>(buffer);

    // figure out the type of the message
    msg_type = msg_header->type;

    // we got a packet, see who it was from
    auto map_val = addr_to_client.find(sender_addr);
    ClientRecord *record;
    if (map_val == addr_to_client.end()) {
        // this is the first packet we received from this client
        // make a new record
        record = new ClientRecord();
        record->client_addr = sender_addr;
        record->client_port = sender_port;
        record->player_id = -1;
        record->latest_msg_ts = 0;
        addr_to_client[sender_addr] = record;
    } else {
        record = map_val->second;
    }
    if (msg_header->ts < record->latest_msg_ts) {
        return false;
    }
    record->latest_msg_ts = msg_header->ts;
    
    if (msg_type == MSG_TYPE::TIME_SYNC) {
        // respond with the server's time
        sendTimeSync(*reinterpret_cast<TimeSyncMsg*>(buffer), record);
    }
    *player_id = &record->player_id;
    return true;
}
