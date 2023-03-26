
#include <sys/fcntl.h>
#include <sys/stat.h>
#include "Server.hpp"


Server::Server() {
	this->listenPairs.push_back(std::make_pair("0.0.0.0", 80));
	this->name = "Server";
	init();
}

Server::~Server() {

}


Server::Server(const std::vector<std::pair<std::string, int> > &listen,
			   const std::string &name) {
	this->listenPairs = listen;
	this->name = name;
	init();
}

void Server::init() {

	initDefaultErrorPages();

	logger = Logger(name);

	logger.setPath("logs/" + name + ".log");
	logger.setUseFile(true);
	logger.debug("Creating server");

	// Listen on a given port and address for incoming connections.
	// This must not block, so we need to use a non-blocking socket.
	// We can use run() to wait for incoming connections.

	for (int i = 0; i < (int) listenPairs.size() && i < 1024; i++) {
		std::string ip = listenPairs[i].first;
		int port = listenPairs[i].second;
		int opt = 1;

		// Creating socket file descriptor
		if ((sockets[i] = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
			logger.error("Failed to create socket");
			return;
		}

		if (setsockopt(sockets[i], SOL_SOCKET, SO_REUSEADDR, &opt,
					   sizeof(opt))) {
			logger.error("Failed to set socket options");
			return;
		}

		addresses[i].sin_family = AF_INET;
		addresses[i].sin_addr.s_addr = inet_addr(ip.c_str());
		addresses[i].sin_port = htons(port);

		if (bind(sockets[i], (struct sockaddr *) &addresses[i],
				 sizeof(addresses[i])) < 0) {
			logger.error("Failed to bind socket");
			return;
		}

		if (listen(sockets[i], 3) < 0) {
			logger.error("Failed to listen on socket");
			return;
		}
		//TODO: This needs to be somewhere else
//		logger.info("Listening on " + listenPairs[i].first + ":" +
//					std::to_string(listenPairs[i].second));

		int flags = fcntl(sockets[i], F_SETFL, O_NONBLOCK);
		if (flags < 0) {
			logger.error("Failed to set socket to non-blocking");
			return;
		}
	}
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
//	FD_ZERO(&efds);
}


int Server::run() {
	this->logger.setLevel(0);
	for (int n = 0; n < (int) listenPairs.size() && n < 1024; n++) {
		FD_SET(sockets[n], &rfds);
//		FD_SET(sockets[n], &efds);
		FD_SET(sockets[n], &wfds);

	}
	int maxfd = sockets[0];
	for (int n = 1; n < (int) listenPairs.size() && n < 1024; n++) {
		if (sockets[n] > maxfd) {
			maxfd = sockets[n];
		}
	}
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	int retval = select(maxfd + 1, &rfds, &wfds, &efds, &tv);
	if (retval == -1) {
		logger.error("Error while waiting for socket");
		return -1;
	} else if (retval) {
		for (int n = 0; n < (int) listenPairs.size() && n < 1024; n++) {
			if (FD_ISSET(sockets[n], &rfds)) {
				int new_socket;
				struct sockaddr address;
				socklen_t addrlen = sizeof(address);
				if ((new_socket = accept(sockets[n], &address, &addrlen)) < 0) {
					logger.error("Failed to accept connection");
					return -1;
				}
				client_addresses[new_socket] = address;
				logger.debug("Accepted connection");
				int flags = fcntl(new_socket, F_SETFL, O_NONBLOCK);
				if (flags < 0) {
					logger.error("Failed to set socket to non-blocking");
					return -1;
				}
				FD_SET(new_socket, &rfds);
//				FD_SET(new_socket, &efds);
				FD_SET(new_socket, &wfds);
				client_to_socket[new_socket] = sockets[n];
				clients.push_back(new_socket);
				if (new_socket > maxfd) {
					maxfd = new_socket;
				}
			}
			if (FD_ISSET(sockets[n], &efds)) {
				logger.error("Error on socket");
				return -1;
			}
		}
	} else {
//		logger.debug("No data within 100ms");
	}

	bool error = false;
	for (int i = 0; i < (int) clients.size(); i++) {

		int client = clients[i];
		if (client_to_socket.find(client) == client_to_socket.end()) {
			continue;
		}
		int sock = client_to_socket[client];
		if (sock == 0) {
			continue;
		}
		error = false;
		if (FD_ISSET(client, &rfds)) {
			char buffer[READ_BUFFER_SIZE] = {0};
			ssize_t valread = recv(client, buffer, READ_BUFFER_SIZE, 0);

			std::stringstream ss;
			// Read until we get an error or the client disconnects
			while (valread >= READ_BUFFER_SIZE) {
				ss << std::string(buffer);
				valread = recv(client, buffer, READ_BUFFER_SIZE, 0);
			}

			if (valread < 0) {
				// These two lines stop the stream buffer from being corrupted
				// for some reason. I don't know why.
				util::to_string(client);
				util::to_string(sock);
				// This is needed to not panic and return 400 for anything at
				// random.
				error = true;
			} else if (valread < READ_BUFFER_SIZE) {
				logger.debug("Client disconnected");
				// Remove client from map
				clients.erase(clients.begin() + i);
				client_to_socket.erase(client);
				i--;
			}
			if (!error) {
				Response response = getResponse(ss.str(), 0);
				std::string responsestr = response.toString();
				logger.log("Response: " + response.getStatusString(), 9);
//				logger.debug("Sending response: " + responsestr);
				if (FD_ISSET(client, &wfds)) {
					send(client, responsestr.c_str(), responsestr.length(), 0);
					close(client);
				} else {
					logger.error("Client not ready for writing");
				}
			}
			i--;
		}
	}

	clients.clear();
	client_to_socket.clear();

	return 0;
}

Response Server::getResponse(const std::string &bufferstr, int client) {

	Response response;
	if (bufferstr.empty()) {
		response = Response(400);
		logger.error("Empty request");
	} else if (bufferstr.find('\r') == std::string::npos) {
		response = Response(400);
		logger.error("Invalid request");
	} else {
		Request request(bufferstr, client_addresses[client]);
		try {
			response = handle_request(request);
		} catch (std::exception &e) {
			logger.error("Exception while handling request: " +
						 std::string(e.what()));
			response = Response(500);
		}
	}

	if (response.getStatus() >= 400) {
		response.setBody(getErrorPage(response.getStatus()));
		response.addHeader("Content-Length",
						   util::to_string(response.getBody().size()));
		response.addHeader("Content-Type", "text/html");
	}

	logger.debug("Response raw: " + response.toString());

	return response;
}

Response Server::handle_request(Request request) {

	logger.log("Request: " + request.getMethod() + " " + request.getPath(), 9);

	Response response;
	std::string path = request.getPath();
	if (request.getMethod() == "GET") {
		response = handle_get(request, path);
	} else if (request.getMethod() == "POST") {
		response = handle_post(request, path);
	} else if (request.getMethod() == "DELETE") {
		response = handle_delete(request, path);
	}


	response.addHeader("Connection", "close");
	response.addHeader("Server", "webserver");
	response.addHeader("Date", util::datetime("%a, %d %b %Y %H:%M:%S %Z"));

	return response;
}

std::string Server::getErrorPage(int status) {
	return this->routes["*"].getErrorPage(status);
}

void Server::initDefaultErrorPages() {
	for (int i = 0; i < 600; i++) {
		if (!this->routes["*"].getErrorPage(i).empty())
			continue;
		if (Response(i).getStatusString().empty()) {
			this->routes["*"].setRawErrorPage(i, "<html>\n"
								   "<body>\n"
								   "<h1>" + util::to_string(i) +
								   " Unknown error</h1>\n"
								   "</body>\n"
								   "</html>");
			continue;
		} else {
			this->routes["*"].setRawErrorPage(i, "<html>\n"
								   "<body>\n"
								   "<h1>" + Response(i).getStatusString() +
								   "</h1>\n"
								   "</body>\n"
								   "</html>");
		}
	}
}

const std::string &Server::getRootPath() const {
	return root_path;
}

void Server::setRootPath(const std::string &rootPath) {
	root_path = rootPath;
}

void Server::addRoute(const Route& route) {
	this->routes[route.getPath()] = route;
}

void Server::setErrorPage(int status, const std::string& path) {
	this->routes["*"].setErrorPage(status, path);
}

void Server::setIndex(const std::string& index) {
	this->routes["*"].setIndex(index);
}

Logger &Server::getLogger() {
	return logger;
}

Response Server::handle_get(const Request& request, const std::string& path) {
	Response response;
	UNUSED(request);

	std::string file_path = util::combine_path(getRootPath(), path, true);
	logger.debug("File path: " + file_path);
	if (file_path.find(getRootPath()) != 0) {
		logger.error("Invalid path");
		return Response(403);
	}
	struct stat statbuf;
	if (stat(file_path.c_str(), &statbuf) != 0) {
		logger.error("File not found");
		return Response(404);
	}
	if (S_ISDIR(statbuf.st_mode)) {
		file_path = util::combine_path(file_path, this->routes["*"].getIndex(), true);
	}
	// Check if file exists
	std::ifstream file(file_path.c_str());
	if (!file.good()) {
		logger.error("File not found");
		return Response(404);
	}
	std::string file_content;
	std::string line;
	while (std::getline(file, line, '\n')) {
		file_content += line + "\n";
	}
	response.setBody(file_content);
	std::string content_type = MimeTypes::getType(file_path);
	response.addHeader("Content-Type", content_type);

	return response;
}

Response Server::handle_post(const Request& request, const std::string& path) {
	Response response(200);
	UNUSED(request);
	UNUSED(path);

	return response;
}

Response Server::handle_delete(const Request& request, const std::string& path) {
	Response response(200);
	UNUSED(request);
	UNUSED(path);

	return response;
}
