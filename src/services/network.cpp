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
    sockaddr_in addr;
    inet_pton(AF_INET, server_addr_str.c_str(), &(addr.sin_addr));
    this->server_addr = addr.sin_addr.s_addr;
    sock.open(CLIENT_PORT);
}

void Client::shutdown() {
    sock.close();
}

void Client::sendInput(ClientInput &input) {
    sock.send(server_addr, &input, sizeof(input));
}

bool Client::getMessage(State &state) {
    size_t bytes = sock.receive(NULL, NULL, &state, sizeof(state));
    return bytes == sizeof(state);
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
void Server::sendStateUpdate(State &state) {
    int client_id = state.id;
    for (auto &addr_obj : addr_to_client) {
        if (addr_obj.first != client_id) {
            sock.send(addr_obj.first, &state, sizeof(state));
        }
    }
    // send to the client whose player is being updated
    // use a special signifier so the client knows this is its player
    //TODO consider using a hash of addr and port instead so that client and server can both determine id
    state.id = -1;
    sock.send(addr_to_client[client_id]->client_addr, &state, sizeof(state));
}

bool Server::getInput(int &player_id, ClientInput &input) {
    unsigned int sender_addr;
    unsigned short sender_port;
    size_t bytes = sock.receive(&sender_addr, &sender_port, &input, sizeof(input));
    if (bytes != sizeof(input)) {
        return false;
    }

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
    player_id = record->player_id;
    return true;
}
