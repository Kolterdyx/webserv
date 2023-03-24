
#include <sys/fcntl.h>
#include "Server.hpp"


Server::Server() {
	this->listenPairs.push_back(std::make_pair("0.0.0.0", 80));
	this->name = "Server";
	init();
}

Server::~Server() {

}


Server::Server(const std::vector<std::pair<std::string, int> >& listen,
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

	for (int i=0; i < (int)listenPairs.size() && i < 1024; i++) {
		std::string ip = listenPairs[i].first;
		int port = listenPairs[i].second;
		int opt = 1;

		// Creating socket file descriptor
		if ((sockets[i] = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		{
			logger.error("Failed to create socket");
			return;
		}

		if (setsockopt(sockets[i], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
		{
			logger.error("Failed to set socket options");
			return;
		}

		addresses[i].sin_family = AF_INET;
		addresses[i].sin_addr.s_addr = inet_addr(ip.c_str());
		addresses[i].sin_port = htons(port);

		if (bind(sockets[i], (struct sockaddr *)&addresses[i],
				 sizeof(addresses[i]))<0)
		{
			logger.error("Failed to bind socket");
			return;
		}

		if (listen(sockets[i], 3) < 0)
		{
			logger.error("Failed to listen on socket");
			return;
		}
		logger.info("Listening on " + listenPairs[i].first + ":" + std::to_string(listenPairs[i].second));
	}
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);
}


int Server::run() {
	this->logger.setLevel(0);
	for (int n=0; n < (int)listenPairs.size() && n < 1024; n++) {
//		logger.log("Listening on " + listenPairs[i].first + ":" + std::to_string(listenPairs[i].second), 1);

		struct timeval tv;
		tv.tv_usec = 10000;
		tv.tv_sec = 0;
		int largest_fd = 0;
		for (int i = 0; i < (int)listenPairs.size(); i++) {
			if (this->sockets[i] > largest_fd) {
				largest_fd = this->sockets[i];
			}
		}
		FD_SET(sockets[n], &rfds);
		FD_SET(sockets[n], &wfds);
		FD_SET(sockets[n], &efds);
		int select_result = select(sockets[n] + 1, &wfds, &rfds, &efds, &tv);

		if (FD_ISSET(sockets[n], &efds)) {
			logger.error("Error on socket");
			continue;
		}
		if (select_result < 0) {
			logger.error("Failed to run");
			continue;
		} else if (select_result == 0) {
			logger.log("Poll timed out", -1);
			continue;
		}

		std::string bufferstr;

		if (FD_ISSET(sockets[n], &rfds)) {
			ssize_t read_result;
			do {
				read_result = recv(sockets[n], this->buffer, 1024, 0);
				bufferstr += std::string(this->buffer);
				std::memset(this->buffer, 0, 1024);
				logger.debug("Read " + std::to_string(read_result) + " bytes from socket");
			} while (read_result > 0);
			if (read_result < 0) {
				logger.error("Failed to read from socket");
				continue;
			}
		} else {
			logger.log("No data to read", -1);
		}


		int client_socket = accept(sockets[n], (struct sockaddr *)&addresses[n], (socklen_t*)&addresses[n]);

		logger.debug("Request: " + bufferstr);
		Response response;

		if (bufferstr.empty()) {
			response = Response(400);
		} else if (bufferstr.find('\r') == std::string::npos) {
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

		if (response.getStatus() >= 400) {
			response.setBody(getDefaultErrorPage(response.getStatus()));
			response.addHeader("Content-Length", std::to_string(response.getBody().size()));
		}

		logger.debug("Response: " + response.getStatusString());

		if (FD_ISSET(sockets[n], &wfds)) {
			ssize_t send_result = send(client_socket, response.toString().c_str(), response.toString().size(), 0);
			if (send_result < 0) {
				logger.error("Failed to write to socket");
			}
		} else {
			logger.log("No data to write", 0);
		}


		FD_ZERO(&wfds);
		FD_ZERO(&rfds);
		FD_ZERO(&efds);

		close(client_socket);

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
					 "<h1>Hello from " + this->name + "</h1>\n"
					 "</body>\n"
					 "</html>");
	response.addHeader("Content-Length", std::to_string(response.getBody().size()));
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
								   "<h1>" + std::to_string(i) + " Unknown error</h1>\n"
								   "</body>\n"
								   "</html>";
			continue;
		} else {
			this->error_pages[i] = "<html>\n"
								   "<body>\n"
								   "<h1>" + Response(i).getStatusString() + "</h1>\n"
								   "</body>\n"
								   "</html>";
		}
	}
}
