#ifndef REQ_GENERATED_HPP
#define REQ_ENERATED_HPP

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
class RequestChat : public RequestBase {
protected:
    ChatBody data;
public:
    RequestChat( ) : RequestBase(0x03, 0x01) {
    }
    ~RequestChat(){ }
    // Serialize RequestChat
    void serialize(std::vector<char>& buffer) const {
        Serializer<uint16_t>::serialize(id, buffer);
        Serializer<uint16_t>::serialize(opcode, buffer);
        StringSerializer::serialize(data.name, buffer);
        StringSerializer::serialize(data.room, buffer);
        StringSerializer::serialize(data.msg, buffer);
        }

    // Deserialize RequestChat
    ChatBody deserialize(const std::vector<char>& buffer) {
        size_t offset = 0;
        id = Serializer<uint16_t>::deserialize(buffer, offset);
        opcode = Serializer<uint16_t>::deserialize(buffer, offset);
        data.name = StringSerializer::deserialize(buffer, offset);
        data.room = StringSerializer::deserialize(buffer, offset);
        data.msg = StringSerializer::deserialize(buffer, offset);
        return (data);
    }

    ChatBody getBody( ) {
        return data;
    }

    void setBody( const ChatBody& body ){
        this->data = body;
    }

    uint16_t getClientId()
    {
        return getID();
    }

    void callback( TCPSocket   *socket ) override {
        this->data = this->deserialize(socket->inbound_data_);
        RequestHandler::RequestChatCallbackIpml(socket, this->data);
    }
};

#endif // PROTOCOL_GENERATED_HPP