#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>
#include "chat.grpc.pb.h" // Replace 'your_proto_file'

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;
using ChatPackage::ChatStream;
using ChatPackage::ClientMessage;
using ChatPackage::ServerMessage;
using namespace std::chrono_literals;

class ChatClient {
public:
  ChatClient(std::string client_id, std::string room_id, std::shared_ptr<Channel> channel)
      : id(client_id), room_id(room_id), stub_(ChatStream::NewStub(channel)) {}

  void SendAndReceiveMessages() {
    ClientContext context;
    context.AddMetadata("client_id", id);
    context.AddMetadata("room_id", room_id);
    std::shared_ptr<ClientReaderWriter<ClientMessage, ServerMessage> > stream(
    stub_->ChatStream(&context));
    
    std::thread reader_thread([stream]() {
        ServerMessage serverMessage;
        while (stream->Read(&serverMessage)) {
          std::cout << "Received: " << serverMessage.payload() << std::endl;
        }
    });

    std::thread writer_thread([stream]() {
        ClientMessage clientMessage;
        int i = 0;
        while (1)
        {
            clientMessage.set_message("ping from client: " + std::to_string(i));
            ++i;
            // stream->Write(clientMessage);
            std::this_thread::sleep_for(1000ms);
        }
    });

    
    writer_thread.join();   // Wait for writer thread to finish
    reader_thread.join();   // Wait for reader thread to finish

    Status status = stream->Finish();
    if (!status.ok()) {
      std::cerr << "Error: RPC failed. " << status.error_message() << std::endl;
    }
  }

private:
  std::unique_ptr<ChatStream::Stub> stub_;
  std::string id;
  std::string room_id;
};

int main(int argc, char** argv) {

	std::string client_id = argv[1];
  std::string room_id = argv[2];

	std::string target_str("localhost:50051"); // Adjust address and port
	ChatClient client(client_id, room_id, grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
	client.SendAndReceiveMessages();

  return 0;
}

