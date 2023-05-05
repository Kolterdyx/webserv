
#include <sys/fcntl.h>
#include <sys/stat.h>
#include "Server.hpp"


Server::Server() {
	this->listeners.push_back(Listener("0.0.0.0", 80));
	this->name = "Server";
	init();
}

Server::~Server() {

}


Server::Server(const std::vector<std::pair<std::string, int> > &listen,
			   const std::string &name) {
	this->name = name;
	for (int i = 0; i < (int) listen.size() && i < 1024; i++) {
		listeners.push_back(Listener(listen[i].first, listen[i].second));
	}
	init();
}

void Server::init() {

	initDefaultErrorPages();

	logger.debug("Creating server");

	// Listen on a given port and address for incoming connections.
	// This must not block, so we need to use a non-blocking socket.
	// We can use run() to wait for incoming connections.
}


int Server::run() {
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	int maxfd = 0;
	// Add sockets of the servers
	for (int n = 0; n < (int) listeners.size() && n < 1024; n++) {
		int socket = listeners[n].getSocket();
		FD_SET(socket, &rfds);
		if (socket > maxfd) {
			maxfd = socket;
		}
	}
	// Add open connections
	std::vector<Connection>::iterator connectIt = connections.begin();
	for (; connectIt != connections.end(); connectIt++) {
		int client = connectIt->getSocket();
		if (connectIt->isFinishRequest())
			FD_SET(client, &wfds);
		else
			FD_SET(client, &rfds);
		if (client > maxfd) {
			maxfd = client;
		}
	}

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	int retval = select(maxfd + 1, &rfds, &wfds, NULL, &tv);
	if (retval == -1) {
		logger.error("Error while waiting for socket");
		return -1;
	}
	if (retval) {
		// Check servers connections
		for (int n = 0; n < (int) listeners.size() && n < 1024; n++) {
			int socket = listeners[n].getSocket();
			if (FD_ISSET(socket, &rfds)) {
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
				connections.push_back(Connection(new_socket));
				if (new_socket > maxfd) {
					maxfd = new_socket;
				}
			}
		}

		// Check open connections
		connectIt = connections.begin();
		for (; connectIt != connections.end(); connectIt++) {
			int client = connectIt->getSocket();

			if (FD_ISSET(client, &rfds)) {
				// If the request is fully received
				if (connectIt->recv() == 0) {
					Response response = getResponse(connectIt->getRequest(), 0);   // TODO siempre se pasa 0, para que el address?
					connectIt->setResponse(response.toString());
					logger.log("Response: " + response.getStatusString(), 9);
				}
			}

			if (FD_ISSET(client, &wfds)) {
				// If the response is fully send
				if (connectIt->send() == 0) {
					connections.erase(connectIt);
					break;
				}
			}
		}
	}
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
	// TODO hardcodeo la configuracion para el tester
	if (request.getPath().find("post_body") != std::string::npos) {
		if (request.getBodySize() > 100)
			return Response(413);
	}
	else if (!request.getBodySize()) {
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
	if (request.getHeader("Transfer-Encoding").find("chunked") != std::string::npos) {
		size_t size = 1;
		int i = 0;
		while (size) {
			size_t count = file_content.find("\r\n", i) - i;
			size = util::hex_str_to_dec(file_content.substr(i, count));
			size_t start = file_content.find("\r\n", i) + 2;
			file << file_content.substr(start, size);
			i = start + size + 2;
		}
	} else {
		file << file_content;
	}
	file.close();

	std::string cgiBinPath = getCgiPath(file_path); // TODO: hardcodeado para tester (busca extension .bla)
	if (cgiBinPath.size()) {
		file_content = util::executeCgi(request, cgiBinPath);
		// Quito los headers del cgi_tester
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
	std::string file_path = util::combine_path(getRootPath(), path, true);
	std::string directory = file_path.substr(0, file_path.rfind("/"));
	struct stat st = {};

	if (directory.size() && stat(directory.c_str(), &st) == -1)
		mkdir(directory.c_str(), 0700);
	std::ofstream file(file_path);
	if (!file.is_open())
		return Response(500);

	std::string file_content = request.getBody();
	if (request.getHeader("Transfer-Encoding").find("chunked") != std::string::npos) {
		size_t size = 1;
		int i = 0;
		while (size) {
			size_t count = file_content.find("\r\n", i) - i;
			size = util::hex_str_to_dec(file_content.substr(i, count));
			size_t start = file_content.find("\r\n", i) + 2;
			file << file_content.substr(start, size);
			i = start + size + 2;
		}
	} else {
		file << file_content;
	}
	file.close();

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

// TODO Si se va a usar mover a Connection
void saveBody(std::string body, int client_socket, ssize_t valread) {
    char buffer[BUFFER_SIZE + 1];
    std::ofstream file("temp.txt");
    file << body;

    while (valread > 0) {
        valread = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (valread < 0 && errno == EAGAIN) {
            valread = 1;
            file.close();
            break;
        }
        file << buffer;
        memset(buffer, 0, BUFFER_SIZE + 1);
    }
    file.close();
}