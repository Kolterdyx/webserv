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
#include "MimeTypes.hpp"
#include "Route.hpp"
#include "Listener.hpp"
#include "util.hpp"

#define READ_BUFFER_SIZE 1024

class Server {

private:
	std::vector<std::pair<std::string, int> > listenPairs;
	std::vector<Listener> listeners;
	std::string name;
//	Logger logger;

	int sockets[1024];
	struct sockaddr_in addresses[1024];
	fd_set rfds;
	fd_set wfds;
	fd_set efds;

	std::map<int, int> client_to_socket;
	std::map<int, struct sockaddr> client_addresses;

	std::string root_path;
	std::map<std::string, Route> routes;

	Response handle_request(Request buffer);

public:


	Server();

	Server(const std::vector<std::pair<std::string, int> > &listen,
		   const std::string &name);

	~Server();

	const std::string &getRootPath() const;

	void setRootPath(const std::string &rootPath);

	int run();

	std::string getErrorPage(int status);

	void init();

	void initDefaultErrorPages();

	Response getResponse(const std::string &bufferstr, int client);

	void addRoute(const Route &route);

	void setErrorPage(int status, const std::string& path);

	void setIndex(const std::string& index);

	Logger &getLogger();

	Response handle_get(const Request& request, const std::string& path);

	Response handle_post(const Request& request, const std::string& path);

	Response handle_delete(const Request& request, const std::string& path);

	Response handle_put(const Request& request, const std::string& path);

	std::string getCgiPath(const std::string &file_path);
};

void saveBody(std::string req, int client_socket, ssize_t valread);

#endif //SERVER_HPP
