#include "webserv.hpp"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " <config>" << std::endl;
		return 1;
	}

	XMLDocument configXML;
	configXML.fromFile(argv[1]);

	// TODO: Add support for multiple attributes and :not()
	XMLElementVector result = configXML.query("/@*");
//	std::cout << result << "\n";
	return 0;
}
