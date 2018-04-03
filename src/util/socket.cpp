//
//  socket.cpp
//  platformer
//
//  Created by Vande Griek, Eric on 4/3/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#include "socket.h"

bool Socket::open(unsigned short port) {
    handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (handle <= 0) {
        return false;
    }

    // bind to port on all interfaces
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(handle, (const sockaddr*)&addr, sizeof(sockaddr_in)) < 0) {
        return false;
    }

    // set nonblocking
    if (fcntl(handle, F_SETFL, O_NONBLOCK, 1) == -1) {
        return false;
    }

    return true;
}

void Socket::close() {
    ::close(handle);
}

bool Socket::send(unsigned int dest_addr, const void *data, size_t data_size) {
    send_addr_buffer.sin_addr.s_addr = dest_addr;
    size_t sent = sendto(handle, data, data_size, 0,
                         (sockaddr*)&send_addr_buffer, sizeof(sockaddr_in));
    if (sent != data_size) {
        return false;
    }
    return true;
}

size_t Socket::receive(unsigned int *sender_addr, unsigned short *sender_port,
                       void *buffer, size_t buffer_size) {
    while (true) {
        socklen_t sender_size;
        size_t bytes = recvfrom(handle, buffer, buffer_size, 0,
                                (sockaddr*)&recv_addr_buffer, &sender_size);
        if (bytes <= 0) {
            return -1;
        }

        // we got a packet, set the address we received it from
        if (sender_addr != NULL) {
            *sender_addr = ntohl(recv_addr_buffer.sin_addr.s_addr);
        }
        if (sender_port != NULL) {
            *sender_port = ntohs(recv_addr_buffer.sin_port);
        }
        return bytes;
    }
}
