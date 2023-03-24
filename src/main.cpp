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


	Webserver server(config);
	server.run();

	return 0;
}
