#include "request_handler.h"
#include "protocols/generate/request_template.h"
#include <thread>
#include <hiredis/hiredis.h>

#include "chat.grpc.pb.h"

using ChatPackage::ServerMessage;

void RequestHandler::RequestChatCallbackIpml(TCPSocket   *socket, ChatBody body)
{
    ServerMessage msg;
    msg.set_name(body.name);
    msg.set_channel(body.room);
    msg.set_payload(body.msg);
    redisContext *context = redisConnect("127.0.0.1", 6379); // Adjust hostname and port as needed
    redisReply *reply = (redisReply *)redisCommand(context, "PUBLISH %s %s", "node1", msg.SerializeAsString().c_str());
    redisFree(context);
}