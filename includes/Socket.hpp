//
// Created by Ciro Garcia belmonte on 3/24/23.
//

#ifndef WEBSERV_SOCKET_HPP
#define WEBSERV_SOCKET_HPP

#include <iostream>
#include <string>
#include <netinet/in.h>
#include "Logger.hpp"
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <unistd.h>

class Socket {

protected:
	int _socket;
	int _port;
	std::string _ip;
	struct sockaddr_in _address;

	Logger logger;

	void init();

public:
	Socket();
	Socket(int port, const std::string &ip);
	Socket(const Socket &copy);
	Socket &operator=(const Socket &copy);
	~Socket();

	int getSocket() const;
	int getPort() const;
	const std::string &getIp() const;

	void setLogger(const Logger &logger);

	std::string receive();
};


#endif //WEBSERV_SOCKET_HPP
