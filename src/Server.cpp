
#include "Server.hpp"

Server::Server(const std::string &ip, int port, const std::string &name) {

	this->ip = ip;
	this->port = port;
	this->name = name;

	this->logger = Logger(name);

	logger.debug("Creating server");

	// Listen on a given port and address for incoming connections.
	// This must not block, so we need to use a non-blocking socket.
	// We can use run() to wait for incoming connections.

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
	{
		logger.error("Failed to create socket");
		return;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip.c_str());
	address.sin_port = htons(port);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		logger.error("Failed to bind socket");
		return;
	}

	if (listen(server_fd, 3) < 0)
	{
		logger.error("Failed to listen on socket");
		return;
	}

	fds[0].fd = server_fd;
	fds[0].events = POLLIN;
}

Server::Server() {

}

Server::~Server() {

}


int Server::run() {
	this->logger.setLevel(1);
	int poll_result = poll(this->fds, 1, -1);
	if (poll_result < 0) {
		logger.error("Failed to run");
		return 1;
	}

	if (this->fds[0].revents & POLLIN) {
		int client_fd = accept(this->server_fd, NULL, NULL);
		if (client_fd < 0) {
			logger.error("Failed to accept connection");
			return 1;
		}

		// Clear buffer
		for (int i = 0; i < 8192; i++) {
			this->buffer[i] = 0;
		}
		int read_result = read(client_fd, this->buffer, 8192);
		if (read_result < 0) {
			logger.error("Failed to read from socket");
			return 1;
		}

		logger.debug("Raw request: " + std::string(this->buffer));

		std::string bufferstr = std::string(this->buffer);

		Response response;

		if (bufferstr.size() == 0) {
			response = Response(400);
		} else if (bufferstr.find("\r") == std::string::npos) {
			response = Response(400);
		} else {
			Request request(this->buffer);
			logger.log(request.getMethod() + " request on '" + request.getPath() + "'", 1);
			try {
				response = handle_request(request);
			} catch (std::exception &e) {
				logger.error("Exception while handling request: " + std::string(e.what()));
				response = Response(500);
			}
		}

		logger.debug("Response: " + response.toString());

		logger.log("Response: " + response.getStatusString(), 1);

		int write_result = write(client_fd, response.toString().c_str(),
								 response.toString().size());
		if (write_result < 0) {
			logger.error("Failed to write to socket");
			return 1;
		}

		close(client_fd);

		logger.debug("Finished handling request");
	}
	return 0;
}

Response Server::handle_request(Request request) {

	logger.debug("Request: " + request.toString());

	Response response(200);

	response.addHeader("Content-Type", "text/html");

	response.setBody("<html>\n"
					 "<body>\n"
					 "<h1>Hello, World!</h1>\n"
					 "</body>\n"
					 "</html>");
	response.addHeader("Content-Length", std::to_string(response.getBody().size()));
	response.addHeader("Connection", "close");
	response.addHeader("Server", "webserver");

	response.addHeader("Date", datetime("%a, %d %b %Y %H:%M:%S %Z"));

	return response;
}
