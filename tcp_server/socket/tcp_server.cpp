#include "tcp_server.h"
#include "utils/macros.h"

/// Add and remove socket file descriptors to and from the EPOLL list.
auto TCPServer::addToEpollList(TCPSocket *socket) {
	epoll_event ev{EPOLLET | EPOLLIN, {reinterpret_cast<void *>(socket)}};
	return !epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket->socket_fd_, &ev);
}

/// Start listening for connections on the provided interface and port.
auto TCPServer::listen(const std::string &iface, int port) -> bool {
    // Create epoll file descriptor
    epoll_fd_ = epoll_create(1);
    if (epoll_fd_ < 0) {
        std::cerr << "epoll_create() failed error: " << std::strerror(errno) << std::endl;
        return false;
    }

    // Connect the listener socket
    if (listener_socket_.connect("", iface, port, true) < 0) {
        std::cerr << "Listener socket failed to connect. iface: " << iface << " port: " << port
                  << " error: " << std::strerror(errno) << std::endl;
        return false;
    }
	
    // Add listener socket to epoll list
    if (!addToEpollList(&listener_socket_)) {
        std::cerr << "epoll_ctl() failed. error: " << std::strerror(errno) << std::endl;
        return false;
    }

    // All operations succeeded
    return true;
}

/// Publish outgoing data from the send buffer and read incoming data from the receive buffer.
auto TCPServer::sendAndRecv() noexcept -> void {
	auto recv = false;

	FOR_EACH_AUTO(receive_sockets_, [&recv](auto socket) { recv |= socket->sendAndRecv(); });
	
	if (recv && recv_finished_callback_ != nullptr) // There were some events and they have all been dispatched, inform listener.
		recv_finished_callback_();

	FOR_EACH_AUTO(send_sockets_, [&recv](auto socket) { recv |= socket->sendAndRecv(); });

}

/// Check for new connections or dead connections and update containers that track the sockets.
auto TCPServer::poll() noexcept -> void {
	const int max_events = 1 + send_sockets_.size() + receive_sockets_.size();

	const int n					  = epoll_wait(epoll_fd_, events_, max_events, 0);
	bool	  have_new_connection = false;
	for (int i = 0; i < n; ++i) {
		const auto &event  = events_[i];
		auto		socket = reinterpret_cast<TCPSocket *>(event.data.ptr);

		// Check for new connections.
		if (event.events & EPOLLIN) {
			if (socket == &listener_socket_) {
				have_new_connection = true;
				continue;
			}

			if (std::find(receive_sockets_.begin(), receive_sockets_.end(), socket) ==
				receive_sockets_.end())
				receive_sockets_.push_back(socket);
		}

		if (event.events & EPOLLOUT) {
			if (std::find(send_sockets_.begin(), send_sockets_.end(), socket) ==
				send_sockets_.end())
				send_sockets_.push_back(socket);
		}

		if (event.events & (EPOLLERR | EPOLLHUP)) {
			if (std::find(receive_sockets_.begin(), receive_sockets_.end(), socket) ==
				receive_sockets_.end())
				receive_sockets_.push_back(socket);
		}
	}

	// Accept a new connection, create a TCPSocket and add it to our containers.
	while (have_new_connection) {
		sockaddr_storage addr;
		socklen_t		 addr_len = sizeof(addr);
		int				 fd = accept(listener_socket_.socket_fd_, reinterpret_cast<sockaddr *>(&addr), &addr_len);
		if (fd == -1)
			break;

		ASSERT(setNonBlocking(fd) && disableNagle(fd),
			   "Failed to set non-blocking or no-delay on socket:" + std::to_string(fd));

		auto socket			   = new TCPSocket();
		socket->socket_fd_	   = fd;
		socket->recv_callback_ = recv_callback_;
		ASSERT(addToEpollList(socket),
			   "Unable to add socket. error:" + std::string(std::strerror(errno)));

		if (std::find(receive_sockets_.begin(), receive_sockets_.end(), socket) ==
			receive_sockets_.end())
			receive_sockets_.push_back(socket);
	}
}
