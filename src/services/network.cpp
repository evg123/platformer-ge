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

    Header *msg_header = reinterpret_cast<Header*>(buffer);

    // figure out the type of the message
    msg_type = msg_header->type;

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
        addr_to_client[sender_addr] = record;
    } else {
        record = map_val->second;
    }
    if (msg_header->ts < record->latest_msg_ts) {
        return false;
    }
    record->latest_msg_ts = msg_header->ts;
    *player_id = &record->player_id;
    return true;
}
