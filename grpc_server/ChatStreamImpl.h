#ifndef CHAT_STREAM_H
#define CHAT_STREAM_H

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <hiredis/hiredis.h>
#include <grpcpp/grpcpp.h>
#include "chat.grpc.pb.h"
#include "chat.pb.h"
#include <mutex>
#include <unordered_map>

using ChatPackage::ChatStream;
using ChatPackage::ClientMessage;
using ChatPackage::ServerMessage;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using namespace std::chrono_literals;

typedef ServerReaderWriter<ServerMessage, ClientMessage>* Ptr_ServerRW;
typedef ServerReaderWriter<ServerMessage, ClientMessage> ServerRW;

struct ClientSubscription {
    std::shared_ptr<ServerRW> stream;
    std::vector<std::string> channels;
};

class ChatStreamImpl final : public ChatStream::Service
{
private:
    std::unordered_map<std::string, std::shared_ptr<ClientSubscription>> m_clients;
    std::mutex m_mutex;

public:
    Status ChatStream(ServerContext* context, ServerReaderWriter<ServerMessage, ClientMessage>* stream) override;
    ClientSubscription* getClientSubscriptionById(const std::string& clientId);   
    int getNumClients();
    bool sendMsgToRoom(const ServerMessage& msg);
};

#endif

