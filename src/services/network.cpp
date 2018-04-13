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
}

void Client::shutdown() {
    sock.close();
}

void Client::sendRegister(ClientRegisterMsg &reg) {
    reg.msg.type = MSG_TYPE::CLIENT_REGISTER;
    auto now = std::chrono::steady_clock::now();
    reg.ts = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
    sock.send(server_addr, server_port, &reg, sizeof(reg));
}

void Client::sendInput(ClientInputMsg &input) {
    input.msg.type = MSG_TYPE::CLIENT_INPUT;
    auto now = std::chrono::steady_clock::now();
    input.ts = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
    sock.send(server_addr, server_port, &input, sizeof(input));
}

bool Client::getMessage(int &msg_type, char *buffer) {
    ssize_t bytes = sock.receive(NULL, NULL, buffer, msg_buffer_size);

    // figure out the type of the message
    msg_type = reinterpret_cast<Msg*>(buffer)->type;

    return bytes > 0;
}

// Server definitions

void Server::init() {
    sock.open(SERVER_PORT);
}

void Server::shutdown() {
    sock.close();
}

/**
 Send the given state update to each client
 */
void Server::sendGameStateUpdate(GameStateMsg &state) {
    state.msg.type = MSG_TYPE::GAME_STATE;
    auto now = std::chrono::steady_clock::now();
    state.ts = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
    // send state update to everyone, game state of other players could be displayed
    for (auto &addr_obj : addr_to_client) {
        // set you to true when sent to the client whose state this is
        // this is how the client learns which player they are
        state.you = addr_obj.second->player_id == state.player_id;
        sock.send(addr_obj.first, addr_obj.second->client_port, &state, sizeof(state));
    }
}

void Server::sendObjectStateUpdate(ObjectStateMsg &state, std::set<int> *player_ids) {
    state.msg.type = MSG_TYPE::OBJECT_STATE;
    auto now = std::chrono::steady_clock::now();
    state.ts = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
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

    // figure out the type of the message
    msg_type = reinterpret_cast<Msg*>(buffer)->type;

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
        addr_to_client[sender_addr] = record;
    } else {
        record = map_val->second;
    }
    *player_id = &record->player_id;
    return true;
}
