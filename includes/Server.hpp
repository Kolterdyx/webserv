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
	std::vector<std::pair<std::string, int> > listenPairs;
	std::string name;


	Logger logger;

	int sockets[1024];
	struct sockaddr_in addresses[1024];
	struct fd_set rfds;
	struct fd_set wfds;
	struct fd_set efds;

	char buffer[1024];

	std::map<int, std::string> error_pages;

	Response handle_request(Request buffer);


public:
	Server();
	Server(const std::vector<std::pair<std::string, int> >& listen, const std::string &name);
	~Server();

	int run();

	const std::string &getDefaultErrorPage(int status);

	void init();

	void initDefaultErrorPages();
};


#endif //SERVER_HPP
