#include "protocols/generate/request_template.h"
#include "socket/tcp_server.h"
#include <thread>

#include "chat.grpc.pb.h" // Replace 'your_proto_file'
#include <grpcpp/grpcpp.h>

using ChatPackage::ChatStream;
using ChatPackage::ClientMessage;
using ChatPackage::ServerMessage;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;
using namespace std::chrono_literals;

const std::string iface = "lo";
const std::string ip	= "127.0.0.1";
const int		  port	= 12345;

void send_msg(std::shared_ptr<TCPSocket> client, const std::string &name, const std::string &room) {
	RequestChat data;

	std::vector<char> buffer;
	uint			  ping_cnt = 0;
	while (true) {
		std::string input = "ping from " + name + "#" + std::to_string(ping_cnt);
		ChatBody	body{name, room, input};
		data.setBody(body);
		buffer.clear();
		data.serialize(buffer);
		client->send(buffer.data(), buffer.size());
		++ping_cnt;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void recv_msg(std::shared_ptr<TCPSocket> client, const std::string &name, const std::string &room) {
	while (true) {
		if (client->sendAndRecv() == -1) {
			std::cerr << "Error receiving message.\n";
			break;
		}
	}
}

class ChatClient {
public:
	ChatClient(std::string client_id, std::string room_id, std::shared_ptr<Channel> channel)
		: id(client_id), room_id(room_id), stub_(ChatStream::NewStub(channel)) {}

	void SendAndReceiveMessages() {
		ClientContext context;
		context.AddMetadata("client_id", id);
		context.AddMetadata("room_id", room_id);
		std::shared_ptr<ClientReaderWriter<ClientMessage, ServerMessage>> stream(
			stub_->ChatStream(&context));

		std::thread reader_thread([stream]() {
			ServerMessage serverMessage;
			while (stream->Read(&serverMessage)) {
				std::cout << "Received: " << serverMessage.payload() << std::endl;
			}
		});

		// std::thread writer_thread([stream]() {
		//     ClientMessage clientMessage;
		//     int i = 0;
		//     while (1)
		//     {
		//         clientMessage.set_message("ping from client: " + std::to_string(i));
		//         ++i;
		//         // stream->Write(clientMessage);
		//         std::this_thread::sleep_for(1000ms);
		//     }
		// });
		// writer_thread.join();   // Wait for writer thread to finish
		reader_thread.join(); // Wait for reader thread to finish

		Status status = stream->Finish();
		if (!status.ok()) {
			std::cerr << "Error: RPC failed. " << status.error_message() << std::endl;
		}
	}

private:
	std::unique_ptr<ChatStream::Stub> stub_;
	std::string						  id;
	std::string						  room_id;
};

int main(int argc, char *argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <name> <room>\n";
		return 1;
	}

	// std::string id = argv[1];
	std::string name = argv[1];
	std::string room = argv[2];

	if (name.empty() || room.empty()) {
		std::cerr << "Name or Room is empty.\n";
		return 1;
	}

	std::shared_ptr<TCPSocket> client = std::make_shared<TCPSocket>();
	client->recv_callback_			  = [&](TCPSocket *socket) {
		   const std::string recv_msg =
			   std::string(socket->inbound_data_.data(), socket->next_rcv_valid_index_);
		   std::cout << "recv_msg: " << recv_msg << "\n";
	};

	if (client->connect(ip, iface, port, false) == -1) {
		std::cerr << "Connect to server failed.\n";
		return 1;
	} else {
		std::cout << "Connected to TCP server on " << ip << ":" << iface << ":" << port
				  << std::endl;
	}

	std::string target_str("localhost:50051"); // Adjust address and port
	ChatClient	client_grpc(name, room,
							grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

	std::thread grpc([&]() { client_grpc.SendAndReceiveMessages(); });
	std::thread recv_thread(recv_msg, client, name, room);
	std::thread send_thread(send_msg, client, name, room);

	grpc.join();
	recv_thread.join();
	send_thread.join();

	return 0;
}
