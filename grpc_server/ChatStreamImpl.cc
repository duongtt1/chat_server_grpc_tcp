#include "ChatStreamImpl.h"

Status ChatStreamImpl::ChatStream(ServerContext* context, ServerReaderWriter<ServerMessage, ClientMessage>* stream)
{
    const auto clientMetadata = context->client_metadata();
    auto clientIdIt = clientMetadata.find("client_id");
    auto roomIdIt = clientMetadata.find("room_id");

    if (clientIdIt == clientMetadata.end() || roomIdIt == clientMetadata.end()) {
        return Status(grpc::INVALID_ARGUMENT, "Missing client_id or room_id metadata");
    }

    try {
        std::string clientId(clientIdIt->second.begin(), clientIdIt->second.end());
        std::string roomId(roomIdIt->second.begin(), roomIdIt->second.end());

        std::shared_ptr<ClientSubscription> cs = std::make_shared<ClientSubscription>();
        if (!cs) {
            return Status(grpc::INTERNAL, "Failed to create ClientSubscription");
        }

        cs->stream = std::make_shared<ServerRW>(*stream);
        if (!cs->stream) {
            return Status(grpc::INTERNAL, "Failed to create ServerReaderWriter");
        }

        cs->channels.push_back(roomId);

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_clients.insert({clientId, cs});
        }

        ClientMessage clientMessage;
        while (stream->Read(&clientMessage)) {
            std::cout << "Received: " << clientMessage.message() << std::endl;
        }

        // Client disconnected here
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_clients.erase(clientId); // Remove client from m_clients
        }

        // Add your logic here for handling client messages
    } catch(const std::exception& e) {
        std::cerr << "Exception in ChatStream: " << e.what() << std::endl;
        return Status(grpc::INTERNAL, "Internal server error");
    }
    
    return Status::OK;
}

ClientSubscription* ChatStreamImpl::getClientSubscriptionById(const std::string& clientId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_clients.find(clientId);
    return (it != m_clients.end()) ? it->second.get() : nullptr;
}

bool ChatStreamImpl::sendMsgToRoom(const ServerMessage& msg)
{
    bool ret = true;
    std::string roomId = msg.channel();

    for (auto it = m_clients.begin(); it != m_clients.end(); it++) 
    {
        auto itChannel = std::find(it->second->channels.begin(), it->second->channels.end(), roomId);
        if (itChannel != it->second->channels.end() && it->second->stream != nullptr) {
            it->second->stream->Write(msg);
        }
    }

    return ret;
}

int ChatStreamImpl::getNumClients()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_clients.size();
}
