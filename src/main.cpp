#include <iostream>
#include "XMLDocument.hpp"


int main() {
	XMLDocument test;
    test.fromFile("run/config.xml");

//	std::cout << "---------------------\n";
//	std::cout << test << "\n";
//	std::cout << "---------------------\n";

//    std::cout << test.query("/xml") << "\n";
//    std::cout << test.query("/xml/server") << "\n";
//    std::cout << test.query("/xml/server:not([@name='dscf'])[]") << "\n";
//    std::cout << test.query("/xml/server[@name='test']/root") << "\n";
//    std::cout << test.query("/xml/server[@name='test']/index") << "\n";
//    std::cout << test.query("/xml/server[@name='test']/client_max_body_size") << "\n";
//    std::cout << test.query("/xml/server[@name='test']/error_pages") << "\n";
//    std::cout << test.query("/xml/server[@name='test']/routes/route[directory_listing])") << "\n";
//    std::cout << test.query("/xml/server[@name='test']/listen") << "\n";
//    std::cout << test.query("/xml/server[@name='test']/error_pages/error_page:not([@status='500'])[@path='path/to/50x.html']") << "\n";
//    std::cout << test.query("/xml/server[@name='test']/error_pages/error_page[@status='404']") << "\n";
//    std::cout << test.query("/xml/server[@name='test']/error_pages/error_page[@status='501']") << "\n";
//    std::cout << test.query("/xml/server[@name='test']/error_pages/error_page[@status='502']") << "\n";
//    std::cout << test.query("/xml/server[@name='test']/routes/route[@path='/someotherpath']") << "\n";


}
