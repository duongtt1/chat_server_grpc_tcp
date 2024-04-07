#ifndef PROTOCOL_GENERATED_HPP
#define PROTOCOL_GENERATED_HPP

// Include library

#include <iostream>
#include <string>
#include <stdint.h>
#include "interfaces/RequestBase.h"
#include "socket/tcp_socket.h"
#include "interfaces/ResponseBase.h"
#include "protocol_utils.h"
#include "request_handler.h"
#include "types.h"

using namespace std;
class ResponseChat : public ResponseBase {
protected:
    ChatRes body;
public:
    ResponseChat( ) : ResponseBase( ) { }
    ~ResponseChat(){ }
    // Serialize ResponseChat
    void serialize(std::vector<char>& buffer) const {
        Serializer<uint8_t>::serialize(statusCode, buffer);
        
        Serializer<bool>::serialize(body.success, buffer);}

    // Deserialize ResponseChat
    bool deserialize(const std::vector<char>& buffer, size_t length) {
        bool ret_code = true;
        size_t offset = 0;
        statusCode = Serializer<uint8_t>::deserialize(buffer, offset);
        
        body.success = Serializer<bool>::deserialize(buffer, offset);return ret_code;
    }

    ChatRes getResponseData( ) {
        return body;
    }

    void setBodyResonpe( const ChatRes& res ){
        this->body = res;
    }
};


#endif