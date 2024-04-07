#include "Server.h"

Server::Server(std::string ip, int port, std::string iface) : requestsQueue((size_t)MAX_REQUEST_QUEUE), port(port), ip(ip), iface(iface) {
    
    tcpServer = new TCPServer();
    using namespace std::placeholders; // for _1
    tcpServer->recv_callback_ = std::bind(&Server::serverRecvCallback, this, _1);
}

Server::~Server() {
    delete tcpServer;
}

void Server::run() {
    if (!tcpServer->listen(iface, port)) {
        std::cerr << "Failed to listen on " << iface << ":" << port << std::endl;
        return;
    }

    while (true) {
        tcpServer->poll();
        tcpServer->sendAndRecv();
    }
}

void Server::serverRecvCallback(TCPSocket* socket) noexcept {
    // std::vector<char> recv_msg(socket->inbound_data_.begin(), socket->inbound_data_.begin() + socket->next_rcv_valid_index_);
    socket->next_rcv_valid_index_ = 0;
    size_t offset = 0;
    uint16_t id = Serializer<uint16_t>::deserialize(socket->inbound_data_, offset);
    uint16_t opcode = Serializer<uint16_t>::deserialize(socket->inbound_data_, offset);
    auto req = RequestFactory::createRequestWithID(id);
    req->socket = socket;
    this->threadQueue.addRequest(req);
}




