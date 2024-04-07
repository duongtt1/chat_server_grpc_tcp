#pragma once

#include <iostream>
#include <string>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
#include "socket/tcp_socket.h"

class RequestBase {
public:
    uint16_t    id;
    uint16_t     opcode;
    TCPSocket   *socket;

    RequestBase(uint16_t id, uint16_t opcode) : id(id), opcode(opcode)  { }
    ~RequestBase() { }

    uint16_t getID() {
        return this->id;
    }

    virtual void callback(TCPSocket *socket){ }
};

