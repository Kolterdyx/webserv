#include <iostream>
#include <fstream>
#include <unistd.h>
#include "XML.hpp"
#include "ordered_map.hpp"



int main() {
	std::ifstream file;

	file.open("run/config.xml");
	std::string xml_string;
	std::string line;
	if (file.is_open()) {
		while (std::getline(file, line)) {
			xml_string += line + "\n";
		}
	}
	file.close();

	XML test(xml_string);

	std::cout << "---------------------\n";
	std::cout << test.to_string(1) << "\n";
	std::cout << "---------------------\n";


}
