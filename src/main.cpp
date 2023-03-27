#include <iostream>
#include <Webserver.hpp>
#include "XML/XMLDocument.hpp"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " <config>" << std::endl;
		return 1;
	}


	XMLDocument config;
	config.fromFile(argv[1]);

    XMLElementVector logConfigs = config.query("/log");

    if (!logConfigs.empty()) {
        XMLElement *logConfig = logConfigs[0];
        if (logConfig->hasAttribute("level") && !logConfig->hasContent()) {
            std::cout << "Invalid log config" << std::endl;
            return 1;
        }
        std::string logLevel = logConfig->getAttribute("level");
        std::string logFile = logConfig->getContent();

        int level;
        try {
            level = util::stoi(logLevel);
        } catch (std::invalid_argument &e) {
            level = 0;
        }
        logger.setLevel(level);
        std::string path = util::combine_path(logFile, "webserv.log");
        logger.setPath(path);
        std::cout << "Log path: " << path << std::endl;
        logger.setUseFile(true);
    }


	Webserver server(config);
	server.run();

	return 0;
}
