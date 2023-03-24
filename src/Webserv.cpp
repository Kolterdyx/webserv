#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include "Webserver.hpp"

void Webserver::run()
{
	logger.debug("Running webserver");

	while (true) {
		for (std::vector<Server>::iterator it = this->servers.begin(); it != this->servers.end(); it++) {
			it->run();
		}
	}
}

Webserver::Webserver() {
	logger = Logger("Webserver");
}

Webserver::Webserver(const Webserver &copy) {
	*this = copy;
}

Webserver &Webserver::operator=(const Webserver &copy) {
	if (this != &copy) {
		this->config = XMLDocument(copy.config);
		this->servers = std::vector<Server>(copy.servers);
		logger = copy.logger;
	}
	return *this;
}

Webserver::~Webserver() {

}

Webserver::Webserver(const XMLDocument &config) {
	logger = Logger("Webserver");
	this->config = config;

	// Create servers

	XMLElementVector serverConfigs = config.query("/server");

	logger.log("Creating " + std::to_string(serverConfigs.size()) + " servers",
			   9);
	for (XMLElementVector::iterator it = serverConfigs.begin(); it != serverConfigs.end(); it++) {
		XMLElementVector listens = (*it)->query("listen");
		if (listens.size() > 1) {
			logger.error("Multiple listen directives found. Ignoring server.");
			continue;
		}
		if (listens.empty()) {
			logger.error("No listen directive found. Ignoring server.");
			continue;
		}
		XMLElement *listen = listens[0];
		if (!listen) {
			logger.error("No listen directive found. Ignoring server.");
			continue;
		}
		if (!listen->hasAttribute("ip") && !listen->hasAttribute("port")) {
			logger.error("Invalid listen directive. Ignoring server.");
			continue;
		}
		std::string ip;
		if (!listen->hasAttribute("ip")) {
			ip = "127.0.0.1";
		} else {
			ip = listen->getAttribute("ip");
		}
		int port;
		try {
			if (!listen->hasAttribute("port")) {
				port = 8080;
			} else {
				port = std::stoi(listen->getAttribute("port"));
			}
		} catch (std::exception &e) {
			logger.error("Invalid port number. Ignoring server.");
			continue;
		}

		if (port < 0 || port > 65535) {
			logger.error("Invalid port number. Ignoring server.");
			continue;
		}

		std::string name;
		if ((*it)->hasAttribute("name")) {
			name = (*it)->getAttribute("name");
		} else {
			name = "Unnamed server";
		}

		Server server(ip, port, name);
		logger.info("Created server on " + ip + ":" + std::to_string(port) + " with name " + name);
		this->servers.push_back(server);
	}
	logger.info("Created " + std::to_string(this->servers.size()) + " servers");
}
