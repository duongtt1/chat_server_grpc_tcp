#include "ChatStreamImpl.h"
#include "SubRedisThread.h"

void RunServer()
{
    std::string server_address("0.0.0.0:50051"); // Adjust port if needed
    std::unique_ptr<ChatStreamImpl> service = std::make_unique<ChatStreamImpl>();

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(service.get());
    SubRedisThread subThread("127.0.0.1", 6379, std::move(service), "node1");
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char **argv)
{
    RunServer();
    return 0;
}