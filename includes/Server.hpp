#ifndef SERVER_HPP
#define SERVER_HPP


#include <string>
#include <sys/socket.h>
#include "macros.hpp"
#include "Logger.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <unistd.h>
#include "Request.hpp"
#include "Response.hpp"


class Server {

private:
	std::string ip;
	int port;
	std::string name;

	Logger logger;

	int server_fd;
	struct sockaddr_in address;
	struct pollfd fds[1];

	char buffer[8192];

	Response handle_request(Request buffer);


public:
	Server();
	Server(const std::string &ip, int port, const std::string &name);
	~Server();

	int run();

};


#endif //SERVER_HPP
