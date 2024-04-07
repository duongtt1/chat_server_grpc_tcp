#ifndef FACTORY_GENERATED_H
#define FACTORY_GENERATED_H

// Include library

#include <iostream>
#include <string>
#include <stdint.h>
#include "interfaces/RequestBase.h"
#include "socket/tcp_socket.h"
#include "interfaces/ResponseBase.h"
#include "protocol_utils.h"
#include "request_handler.h"
#include "request_template.h"
#include "types.h"

using namespace std;

class RequestFactory {
    public:
    static RequestBase* createRequestWithID( uint16_t id ){
        RequestBase* req;
        switch (id)
        {
        case 0x03u:
            req = new RequestChat();
            break;

        default:
            req = nullptr;
            break;
        }

        return req;
    }
};

#endif