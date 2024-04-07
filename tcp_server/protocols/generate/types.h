#ifndef TYPE_GENERATED_HPP
#define TYPE_GENERATED_HPP

// Include library

#include <iostream>
#include <string>
#include <stdint.h>
#include "interfaces/RequestBase.h"
#include "socket/tcp_socket.h"
#include "interfaces/ResponseBase.h"
#include "protocol_utils.h"
#include "request_handler.h"

using namespace std;

// Generated Enums

enum class E_ReturnType : uint8_t {
    Ok = 0,
    Error = 1,
    Invalid = 2,
};

enum class E_StreamDevice : uint8_t {
    Cam0 = 0,
    Screen = 1,
    Invalid = 2,
};

struct ChatBody
{
    string name;
    string room;
    string msg;
};

struct ChatRes
{
    bool success;
};


#endif // PROTOCOL_GENERATED_HPP