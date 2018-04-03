//
//  socket.h
//  platformer
//
//  Created by Vande Griek, Eric on 4/3/18.
//  Copyright © 2018 Vande Griek, Eric. All rights reserved.
//

#ifndef socket_h
#define socket_h

class Socket {
private:
    int handle;
public:
    bool open(unsigned short port);
    bool close();
    bool send(unsigned int dest_addr, const void *data, int data_size);
    int receive(unsigned int &sender_addr, void *buffer, int buffer_size);
};

#endif /* socket_h */
