
#include <sys/fcntl.h>
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
		logger.info("Listening on " + listenPairs[i].first + ":" +
					std::to_string(listenPairs[i].second));

		int flags = fcntl(sockets[i], F_SETFL, O_NONBLOCK);
		if (flags < 0) {
			logger.error("Failed to set socket to non-blocking");
			return;
		}
	}
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);
}


int Server::run() {
	this->logger.setLevel(0);
	for (int n = 0; n < (int) listenPairs.size() && n < 1024; n++) {
		FD_SET(sockets[n], &rfds);
		FD_SET(sockets[n], &efds);
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
				if ((new_socket = accept(sockets[n], NULL, NULL)) < 0) {
					logger.error("Failed to accept connection");
					return -1;
				}
				logger.debug("Accepted connection");
				int flags = fcntl(new_socket, F_SETFL, O_NONBLOCK);
				if (flags < 0) {
					logger.error("Failed to set socket to non-blocking");
					return -1;
				}
				FD_SET(new_socket, &rfds);
				FD_SET(new_socket, &efds);
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
		logger.debug("Checking client " + std::to_string(client));
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
				logger.error("Error while reading from client " +
							 std::to_string(client) + " on socket " +
							 std::to_string(sock));
				error = true;
			} else if (valread < READ_BUFFER_SIZE) {
				logger.debug("Client disconnected");
				// Remove client from map
				clients.erase(clients.begin() + i);
				client_to_socket.erase(client);
				i--;
			}
//			if (FD_ISSET(client, &efds)) {
//				logger.error("Error on socket " + std::to_string(client));
//				error = true;
//			}
			if (!error) {
				Response response = getResponse(ss.str());
				std::string responsestr = response.toString();
				logger.debug("Sending response: " + responsestr);
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

Response Server::getResponse(const std::string &bufferstr) {

	logger.debug("Received request: " + bufferstr);

	Response response;
	if (bufferstr.empty()) {
		response = Response(400);
		logger.error("Empty request");
	} else if (bufferstr.find('\r') == std::string::npos) {
		response = Response(400);
		logger.error("Invalid request");
	} else {
		Request request(bufferstr);
		logger.log(
				request.getMethod() + " request on '" + request.getPath() + "'",
				1);
		try {
			response = handle_request(request);
		} catch (std::exception &e) {
			logger.error("Exception while handling request: " +
						 std::string(e.what()));
			response = Response(500);
		}
	}

	if (response.getStatus() >= 400) {
		response.setBody(getDefaultErrorPage(response.getStatus()));
		response.addHeader("Content-Length",
						   std::to_string(response.getBody().size()));
	}
	return response;
}

Response Server::handle_request(Request request) {

	Response response;
	response.addHeader("Content-Type", "text/html");

	response.setBody("<html>\n"
					 "<body>\n"
					 "<h1>Hello from " + this->name + "</h1>\n"
													  "<p> You requested " +
					 request.getPath() + "</p>\n"
										 "</body>\n"
										 "</html>");
	response.addHeader("Content-Length",
					   std::to_string(response.getBody().size()));
	response.addHeader("Connection", "close");
	response.addHeader("Server", "webserver");

	response.addHeader("Date", datetime("%a, %d %b %Y %H:%M:%S %Z"));

	return response;
}

const std::string &Server::getDefaultErrorPage(int status) {
	return this->error_pages[status];
}

void Server::initDefaultErrorPages() {
	for (int i = 0; i < 1000; i++) {
		if (Response(i).getStatusString() == "") {
			this->error_pages[i] = "<html>\n"
								   "<body>\n"
								   "<h1>" + std::to_string(i) +
								   " Unknown error</h1>\n"
								   "</body>\n"
								   "</html>";
			continue;
		} else {
			this->error_pages[i] = "<html>\n"
								   "<body>\n"
								   "<h1>" + Response(i).getStatusString() +
								   "</h1>\n"
								   "</body>\n"
								   "</html>";
		}
	}
}
