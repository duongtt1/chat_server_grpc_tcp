#ifndef REQ_HANDLE_H
#define REQ_HANDLE_H

// Include library

#include <iostream>
#include <string>
#include <stdint.h>
#include "interfaces/RequestBase.h"
#include "socket/tcp_socket.h"
#include "interfaces/ResponseBase.h"
#include "protocol_utils.h"
#include "types.h"

using namespace std;

class RequestHandler final {
public:
    static void RequestChatCallbackIpml(TCPSocket   *socket, ChatBody body);
};

#endif