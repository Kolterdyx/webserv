//
// Created by Ciro Garcia belmonte on 3/24/23.
//

#include <arpa/inet.h>
#include "Socket.hpp"

Socket::Socket() : _address() {
	_socket = 0;
	_port = 0;
	_ip = "";

}

Socket::Socket(int port, const std::string &ip) : _address() {
	_socket = 0;
	_port = port;
	_ip = ip;

	init();
}

Socket::Socket(const Socket &copy) {
	_socket = copy._socket;
	_port = copy._port;
	_ip = copy._ip;
	_address = copy._address;

	init();

}

Socket &Socket::operator=(const Socket &copy) {
	_socket = copy._socket;
	_port = copy._port;
	_ip = copy._ip;
	_address = copy._address;

	init();

	return *this;
}

Socket::~Socket() {

}

int Socket::getSocket() const {
	return _socket;
}

int Socket::getPort() const {
	return _port;
}

const std::string &Socket::getIp() const {
	return _ip;
}

void Socket::init() {

	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0) {
		logger.error("Error opening socket");
		return;
	}

	int flags = fcntl(_socket, F_SETFL | O_NONBLOCK, 0);
	if (flags < 0) {
		logger.error("Error setting socket to non-blocking");
		return;
	}

	int opt = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		logger.error("Error setting socket options");
		return;
	}

	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = inet_addr(_ip.c_str());
	_address.sin_port = htons(_port);

	if (bind(_socket, (struct sockaddr *) &_address, sizeof(_address)) < 0) {
		logger.error("Error binding socket");
		return;
	}

	if (listen(_socket, 5) < 0) {
		logger.error("Error listening on socket");
		return;
	}

	logger.info("Socket initialized");
}

void Socket::setLogger(const Logger &logger) {
	this->logger = logger;
}

std::string Socket::receive() {
	char buffer[1024];
	std::string message;

	fd_set read_fds;
	FD_ZERO(&read_fds);

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 10000;

	int client_socket = accept(_socket, NULL, NULL);
	if (client_socket < 0) {
		logger.error("Error accepting connection");
		return "";
	}
	int flags = fcntl(client_socket, F_SETFL | O_NONBLOCK, 0);
	if (flags < 0) {
		logger.error("Error setting socket to non-blocking");
		return "";
	}

	FD_SET(client_socket, &read_fds);

	while (true) {
		// wait for data to become available
		int select_result = select(client_socket + 1, &read_fds, NULL, NULL, &timeout);
		if (select_result == -1) {
			logger.error("Error in select");
			return message;
		} else if (select_result == 0) {
			// timeout
			logger.debug("Timeout waiting for data");
			break;
		}

		ssize_t bytes = recv(client_socket, buffer, 1024, 0);
		if (bytes < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				// no more data available
				break;
			} else {
				logger.error("Error receiving message");
				return message;
			}
		} else if (bytes == 0) {
			// client closed connection
			break;
		} else {
			message += std::string(buffer, bytes);
			logger.debug("Received " + std::to_string(bytes) + " bytes");
		}
	}

	int close_result = close(client_socket);
	if (close_result < 0) {
		logger.error("Error closing socket");
		return message;
	}

	return message;
}


