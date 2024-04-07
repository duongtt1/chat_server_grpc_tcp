#ifndef SERVER_H
#define SERVER_H

#include "socket/tcp_server.h"
#include "protocols/generate/request_handler.h"
#include "protocols/generate/factory_request.h"
#include "utils/SafeQueue.h"
#include "utils/macros.h"
#include <iostream>
#include <memory>
#include <vector>
#include "utils/CacheThread.h"

// Forward declarations
class RequestBase;
class RequestFactory;

class Server final {
private:
    SafeQueue<std::unique_ptr<RequestBase>> requestsQueue;
    TCPServer *tcpServer;
    std::string iface;
    std::string ip;
    int port;
    CacheThread threadQueue;
public:
    Server(std::string ip, int port, std::string iface);
    ~Server();
    void run();
    void start_cache()
    {
        threadQueue.start();
    }

private:
    void serverRecvCallback(TCPSocket* socket) noexcept;
};

#endif // SERVER_H
