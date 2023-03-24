
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
		if (listens.empty()) {
			logger.error("No listen directive found. Ignoring server.");
			continue;
		}
		std::vector<std::pair<std::string, int> > listenPairs;
		for (XMLElementVector::iterator listen = listens.begin(); listen != listens.end(); listen++) {
			if (!(*listen)->hasAttribute("ip") &&
				!(*listen)->hasAttribute("port")) {
				logger.error("Invalid listen directive. Ignoring server.");
				continue;
			}
			std::string ip;
			if (!(*listen)->hasAttribute("ip")) {
				ip = "0.0.0.0";
			} else {
				ip = (*listen)->getAttribute("ip");
			}
			int port = std::stoi((*listen)->getAttribute("port"));

			if (!(*listen)->hasAttribute("port")) {
				port = 80;
			} else {
				try {
					port = std::stoi((*listen)->getAttribute("port"));
				} catch (std::invalid_argument &e) {
					logger.error("Invalid port number. Ignoring server.");
					continue;
				}
			}
			listenPairs.push_back(std::pair<std::string, int>(ip, port));
		}
		std::string name;
		if (!(*it)->hasAttribute("name")) {
			name = "Unnamed server";
		} else {
			name = (*it)->getAttribute("name");
		}

		Server server(listenPairs, name);
		logger.info("Created server with name '" + name + "'");
		this->servers.push_back(server);

	}
	logger.info("Created " + std::to_string(this->servers.size()) + " servers");
}
