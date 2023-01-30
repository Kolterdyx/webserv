#include <iostream>
#include "XML/XMLDocument.hpp"

int main() {
	XMLDocument xml;
    xml.fromFile("run/config.xml");

    // TODO: Add support for multiple attributes and :not()
    XMLElementVector result = xml.query("/xml/server");
    std::cout << result << "\n";
	return 0;
}
