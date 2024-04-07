#include "tcp_socket.h"

/// Create TCPSocket with provided attributes to either listen-on / connect-to.
auto TCPSocket::connect(const std::string &ip, const std::string &iface, int port,
						bool is_listening) -> int {
	// Note that needs_so_timestamp=true for FIFOSequencer.
	const SocketCfg socket_cfg{ip, iface, port, false, is_listening, true};
	
	socket_fd_ = createSocket(socket_cfg);
	
	socket_attrib_.sin_addr.s_addr = INADDR_ANY;
	socket_attrib_.sin_port		   = htons(port);
	socket_attrib_.sin_family	   = AF_INET;

	return socket_fd_;
}

auto TCPSocket::sendAndRecv() noexcept -> bool {
	char ctrl[CMSG_SPACE(sizeof(struct timeval))];
	auto cmsg = reinterpret_cast<struct cmsghdr *>(&ctrl);

	iovec  iov{inbound_data_.data() + next_rcv_valid_index_, TCPBufferSize - next_rcv_valid_index_};
	msghdr msg{&socket_attrib_, sizeof(socket_attrib_), &iov, 1, ctrl, sizeof(ctrl), 0};

	// Non-blocking call to read available data.
	const auto read_size = recvmsg(socket_fd_, &msg, MSG_DONTWAIT);
	if (read_size > 0) {
		next_rcv_valid_index_ += read_size;
		timeval time_kernel;
		if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMP &&
			cmsg->cmsg_len == CMSG_LEN(sizeof(time_kernel))) {
			memcpy(&time_kernel, CMSG_DATA(cmsg), sizeof(time_kernel));
		}

		recv_callback_(this);
	}

	if (next_send_valid_index_ > 0) {
		// Non-blocking call to send data.
		const auto n = ::send(socket_fd_, outbound_data_.data(), next_send_valid_index_, MSG_DONTWAIT | MSG_NOSIGNAL);
	}
	next_send_valid_index_ = 0;

	return (read_size > 0);
}

/// Write outgoing data to the send buffers.
auto TCPSocket::send(const void *data, size_t len) noexcept -> void {
	memcpy(outbound_data_.data() + next_send_valid_index_, data, len);
	next_send_valid_index_ += len;
}