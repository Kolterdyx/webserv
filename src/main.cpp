#include <iostream>
#include "XML/XMLDocument.hpp"

std::vector<std::string> charpp_to_stringvec(char **charpp, int size) {
	std::vector<std::string> stringvec;
	stringvec.reserve(size);
	for (int i = 0; i < size; i++) {
		stringvec.push_back(charpp[i]);
	}
	return stringvec;
}

int main(int argc, char *argv[]) {
	std::vector<std::string> args = charpp_to_stringvec(argv, argc);
	std::cout << "Arguments: " << std::endl;
	std::cout << args << std::endl;

	if (args.size() < 2) {
		std::cout << "Usage: " << args[0] << " <config>" << std::endl;
		return 1;
	}

	XMLDocument xml;
	xml.fromFile(args[1]);

	// TODO: Add support for multiple attributes and :not()
	XMLElementVector result = xml.query("/@/error_page");
	std::cout << result << "\n";
	return 0;
}
