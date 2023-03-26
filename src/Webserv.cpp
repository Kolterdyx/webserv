
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

	logger.log("Creating " + util::to_string(serverConfigs.size()) + " servers",
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
			int port = util::stoi((*listen)->getAttribute("port"));

			if (!(*listen)->hasAttribute("port")) {
				port = 80;
			} else {
				try {
					port = util::stoi((*listen)->getAttribute("port"));
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
		if (!initServer(&server, *it)) {
			continue;
		}
		logger.info("Created server with name '" + name + "'");
		this->servers.push_back(server);

	}
	logger.info("Created " + util::to_string(this->servers.size()) + " servers");
}

bool Webserver::initServer(Server *server, XMLElement* element)
{
	Route rootRoute;

	XMLElementVector rootPaths = element->query("root");
	XMLElementVector routes = element->query("route");
	XMLElementVector errorPages = element->query("error_page");
	XMLElementVector indexFiles = element->query("index");
	if (rootPaths.empty()) {
		logger.warn("No root directive found. Ignoring server.");
		return false;
	}
	else if (rootPaths.size() > 1) {
		logger.warn("Multiple root directives found. Ignoring server.");
		return false;
	}

	// Load routes. They can override values from the main server
	for (XMLElementVector::iterator it = routes.begin(); it != routes.end(); it++) {
		if (!(*it)->hasAttribute("path") || !(*it)->hasAttribute("location")) {
			logger.warn("Invalid route directive. Ignoring route.");
			continue;
		}
		server->addRoute(Route(*it, server->getLogger()));
	}

	// Load error pages for the whole server
	for (XMLElementVector::iterator it = errorPages.begin(); it != errorPages.end(); it++) {
		if (!(*it)->hasAttribute("code") || !(*it)->hasAttribute("location")) {
			logger.warn("Invalid error_page directive. Ignoring error_page.");
			continue;
		}

		int status;

		if (!(*it)->hasAttribute("status")) {
			logger.warn("Invalid error_page directive. Ignoring error_page.");
			continue;
		}

		try {
			status = util::stoi((*it)->getAttribute("status"));
		} catch (std::invalid_argument &e) {
			logger.warn("Invalid error code. Ignoring error_page.");
			continue;
		}

		if (!(*it)->hasAttribute("path")) {
			logger.warn("Invalid error_page directive. Ignoring error_page.");
			continue;
		}
		std::string path = (*it)->getAttribute("path");
		rootRoute.setErrorPage(status, path);
	}

	// Load index files
	std::string index;
	if (indexFiles.empty()) {
		logger.warn("No index directive found. Using default (index.html).");
		index = "index.html";
	}
	else if (indexFiles.size() > 1) {
		logger.warn("Multiple index directives found. Using first one.");
		index = indexFiles[0]->getContent();
	}

	// Load cgi stuff
	XMLElementVector cgiConfigs = element->query("cgi");
	if (cgiConfigs.size() == 1) {
		XMLElement *cgiConfig = cgiConfigs[0];
		rootRoute.setCgiBinPath(cgiConfig->getAttribute("path"));
		rootRoute.setCgiExtension(cgiConfig->getAttribute("ext"));
		bool cgiEnabled;
		if (cgiConfig->getAttribute("enabled") == "true") {
			cgiEnabled = true;
		} else if (!cgiConfig->hasAttribute("enabled") || cgiConfig->getAttribute("enabled") == "false") {
			cgiEnabled = false;
		} else {
			logger.warn("Invalid cgi directive. Ignoring cgi.");
			cgiEnabled = false;
		}
		rootRoute.setCgiEnabled(cgiEnabled);
	}

	// Directory listing
	XMLElementVector dirListingConfigs = element->query("directory_listing");
	bool dirListingEnabled;
	std::string dirListingIndex;
	if (dirListingConfigs.size() == 1) {
		XMLElement *dirListingConfig = dirListingConfigs[0];
		if (dirListingConfig->getAttribute("enabled") == "true") {
			dirListingEnabled = true;
		} else if (!dirListingConfig->hasAttribute("enabled") || dirListingConfig->getAttribute("enabled") == "false") {
			dirListingEnabled = false;
		} else {
			logger.warn("Invalid directory_listing directive. Ignoring directory_listing.");
			dirListingEnabled = false;
		}
		if (dirListingConfig->hasAttribute("index")) {
			dirListingIndex = dirListingConfig->getContent();
		} else {
			dirListingIndex = "index.html";
		}
	} else {
		dirListingEnabled = false;
		dirListingIndex = "index.html";
	}
	rootRoute.setDirectoryListingEnabled(dirListingEnabled);
	rootRoute.setDirectoryListingPath(dirListingIndex);

	rootRoute.setPath("*");
	rootRoute.setIndex(index);

	server->addRoute(rootRoute);
	server->setRootPath(rootPaths[0]->getContent());
	server->initDefaultErrorPages();
	return true;
}
