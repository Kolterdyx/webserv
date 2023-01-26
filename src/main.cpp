#include <iostream>
#include <fstream>
#include <unistd.h>
#include "XML.hpp"
#include "ordered_map.hpp"

int main() {
	std::ifstream file;

	file.open("test.xml");
	std::string xml_string;
	std::string line;
	if (file.is_open()) {
		while (std::getline(file, line)) {
			xml_string += line + "\n";
		}
	}
	file.close();

	XML test("<xml>\n    <server/>\n</xml>\n");

	std::cout << "Hello, World!" << std::endl;

	std::cout << test << std::endl;
}
