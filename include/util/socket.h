//
//  socket.h
//  platformer
//
//  Created by Vande Griek, Eric on 4/3/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef socket_h
#define socket_h

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

class Socket {
private:
    int handle;
    sockaddr_in send_addr_buffer;
    sockaddr_in recv_addr_buffer;
public:
    bool open(unsigned short port);
    void close();
    bool send(unsigned int addr, const void *data, size_t data_size);
    size_t receive(unsigned int *sender_addr, unsigned short *sender_port,
                   void *buffer, size_t buffer_size);
};

#endif /* socket_h */
