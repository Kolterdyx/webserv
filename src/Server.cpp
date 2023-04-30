
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

	logger.debug("Creating server");

	// Listen on a given port and address for incoming connections.
	// This must not block, so we need to use a non-blocking socket.
	// We can use run() to wait for incoming connections.

	for (int i = 0; i < (int) listenPairs.size() && i < 1024; i++) {
		listeners.push_back(Listener(listenPairs[i].first, listenPairs[i].second));
		sockets[i] = listeners[i].getSocket();
	}
}


int Server::run() {
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
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
				if ((new_socket = listeners[n].newConnection()) < 0) {
					logger.error("Failed to accept connection");
					return -1;
				}
				client_addresses[new_socket] = listeners[n].getClientAddress();
				logger.debug("Accepted connection");
				int flags = fcntl(new_socket, F_SETFL, fcntl(new_socket, F_GETFL) | O_NONBLOCK);
				if (flags < 0) {
					logger.error("Failed to set socket to non-blocking");
					return -1;
				}
				FD_SET(new_socket, &rfds);
//				FD_SET(new_socket, &efds);
				FD_SET(new_socket, &wfds);
				client_to_socket[new_socket] = sockets[n];
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
//		logger.debug("No site1 within 100ms");
	}

	bool error = false;
	std::map<int, int>::iterator clientsIt = client_to_socket.begin();
	for (; clientsIt != client_to_socket.end(); clientsIt++) {
		int client = clientsIt->first;
		int sock = clientsIt->second;
		if (sock == 0) {
			continue;
		}
		error = false;
		if (FD_ISSET(client, &rfds)) {
			char buffer[READ_BUFFER_SIZE + 1] = {0};
			ssize_t valread = 1;

			std::string req;
			// Read until we get an error or the client disconnects
			while (valread > 0) {
				valread = recv(client, buffer, READ_BUFFER_SIZE, 0);
				// Non-blocking fds send a 'Try again' error when there is still nothing in the buffer
				if (valread < 0 && errno == EAGAIN) {
					valread = 1;
					usleep(1000);
					continue;
				}
				req += buffer;
				memset(buffer, 0, READ_BUFFER_SIZE + 1);
				// TODO: si el cliente no cierra la conexion ni manda "\r\n\r\n" como sigue?
				// size_t h_end;
				// if ((h_end = req.find("\r\n\r\n")) != std::string::npos) {
				// 	size_t c_length = req.find("Content-Length");
				// 	int len = -1;

				// 	if (c_length != std::string::npos) {
				// 		len = util::stoi(req.substr(c_length + 16, req.find("\r\n", c_length)));
				// 	}
				// 	if (req.find("Transfer-Encoding: chunked") != std::string::npos) {
				// 		len = 0;
				// 		saveBody(req.substr(h_end + 4), client, valread);
				// 	}
				// 	if (len > 0 && req.size() - h_end - 4 - len == 0) {
				// 		break;
				// 	}
				// 	if (len == -1) break;
				// 	std::string b = req.substr(h_end + 4);
				// 	std::cout << "body: " << b << "\nlen: " << len << std::endl;
				// 	break;
				// }
				if (req.size() > 4 && req.substr(req.size() - 4) == "\r\n\r\n")
					break;
			}

			if (valread < 0) {
				// This is needed to not panic and return 400 for anything at
				// random.
				error = true;
			} else {
				logger.debug("Client disconnected");
				// Remove client from map
				client_to_socket[client] = 0;
			}
			if (!error) {
				Response response = getResponse(req, 0);
				std::string responsestr = response.toString();
				logger.log("Response: " + response.getStatusString(), 9);
//				logger.debug("Sending response: " + responsestr);
				if (FD_ISSET(client, &wfds)) {
					ssize_t lens;
					size_t pos = 0;
					while ((lens = send(client, &responsestr.c_str()[pos], responsestr.size() - pos, 0)) > 0) {
						pos += lens;
						if (responsestr.size() <= pos)
							break;
					}
					close(client);
				} else {
					logger.error("Client not ready for writing");
				}
			}
		}
	}

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

	// logger.debug("Response raw: " + response.toString());
	logger.debug("Content-Length: " + response.getHeader("Content-Length"));

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
	} else if (request.getMethod() == "PUT") {
		response = handle_put(request, path);
	} else {
		response.setStatus(405);
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
    if (rootPath.empty()) {
        logger.error("Root path cannot be empty");
        return;
    } else if (rootPath[rootPath.length() - 1] != '/') {
        logger.error("Root path must end with a slash");
        return;
    } else if (rootPath[0] != '/') {
        std::string absolutePathCwd = std::string(getcwd(NULL, 0));
        root_path = util::combine_path(absolutePathCwd, rootPath, true);
        return;
    }
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

Response Server::handle_get(const Request& request, const std::string& path) {
	Response response;
	UNUSED(request);

	std::string file_path = util::combine_path(getRootPath(), path, true);
	logger.debug("File path: " + file_path);
	if (file_path.find(getRootPath()) != 0) {
		logger.error("Invalid path");
		return Response(403);
	}
	struct stat statbuf = {};
	if (stat(file_path.c_str(), &statbuf) != 0) {
		logger.error("File not found");
		return Response(404);
	}
	if (S_ISDIR(statbuf.st_mode)) {
		file_path = util::combine_path(file_path, this->routes["*"].getIndex(), true);
		logger.debug("File path if is dir: " + file_path);
		logger.debug(this->routes["*"].getIndex());
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

	std::string cgiBinPath = getCgiPath(file_path);
	if (cgiBinPath.size()) {
		file_content = util::executeCgi(request, cgiBinPath, file_content);
		if (file_content.find("\r\n\r\n") + 4 < file_content.size()) {
			file_content = file_content.substr(file_content.find("\r\n\r\n") + 4);
		}
		// response.parse_http_response(file_content);
		// return response;
	}
	response.setBody(file_content);

	std::string content_type = MimeTypes::getType(file_path);
	response.addHeader("Content-Type", content_type);

	return response;
}

Response Server::handle_post(const Request& request, const std::string& path) {
	if (!request.getBodySize()) {
		return Response(405);
	}

	Response response(200);
	std::string file_path = util::combine_path(getRootPath(), path, true);
	// logger.debug("File path: " + file_path);
	// if (file_path.find(getRootPath()) != 0) {
	// 	logger.error("Invalid path");
	// 	return Response(403);
	// }
	// struct stat statbuf = {};
	// if (stat(file_path.c_str(), &statbuf) != 0) {
	// 	logger.error("File not found");
	// 	return Response(404);
	// }
	// if (S_ISDIR(statbuf.st_mode)) {
	// 	file_path = util::combine_path(file_path, this->routes["*"].getIndex(), true);
	// 	logger.debug("File path if is dir: " + file_path);
	// 	logger.debug(this->routes["*"].getIndex());
	// }

	// // Check if file exists
	// std::ifstream file(file_path.c_str());
	// if (!file.good()) {
	// 	logger.error("File not found");
	// 	return Response(404);
	// }
	std::string file_content;
	// std::string line;
	// while (std::getline(file, line, '\n')) {
	// 	file_content += line + "\n";
	// }

	file_content = request.getBody();
	std::ofstream file("cgiInput");
	size_t size = 1;
	int i = 0;
	while (size) {
		size_t count = file_content.find("\r\n", i) - i;
		size = util::hex_str_to_dec(file_content.substr(i, count));
		size_t start = file_content.find("\r\n", i) + 2;
		file << file_content.substr(start, size);
		i = start + size + 2;
	}
	file.close();

	std::string cgiBinPath = getCgiPath(file_path);
	if (cgiBinPath.size()) {
		file_content = util::executeCgi(request, cgiBinPath);
		if (file_content.find("\r\n\r\n") + 4 < file_content.size()) {
			file_content = file_content.substr(file_content.find("\r\n\r\n") + 4);
		}
	}
	response.setBody(file_content);
	return response;
}

Response Server::handle_delete(const Request& request, const std::string& path) {
	Response response(405);
	UNUSED(request);
	UNUSED(path);

	return response;
}

Response Server::handle_put(const Request& request, const std::string& path) {
	Response response(201);
	UNUSED(request);
	UNUSED(path);

	return response;
}

// Return the path of the cgi binary or an empty string
std::string Server::getCgiPath(const std::string &file_path) {
	std::string::size_type n = file_path.rfind(".");
	if (n == std::string::npos)
        return "";
	std::string extension = file_path.substr(n);

	std::map<std::string, Route>::iterator it = routes.begin();
	for (; it != routes.end(); it++) {
		if (it->second.getCgiExtension() == extension)
			return it->second.getCgiBinPath();
	}
	return "";
}

void saveBody(std::string body, int client_socket, ssize_t valread) {
    char buffer[READ_BUFFER_SIZE + 1];
    std::ofstream file("temp.txt");
    file << body;

    while (valread > 0) {
        valread = recv(client_socket, buffer, READ_BUFFER_SIZE, 0);
        std::cout << "valread: " << valread << std::endl;
        if (valread < 0 && errno == EAGAIN) {
            valread = 1;
            file.close();
            break;
        }
        file << buffer;
        memset(buffer, 0, READ_BUFFER_SIZE + 1);
    }
    file.close();
}