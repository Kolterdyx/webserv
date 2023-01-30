#include <iostream>
#include "XML/XMLDocument.hpp"


int main() {
	XMLDocument test;
    test.fromFile("run/config.xml");

//	std::cout << "---------------------\n";
//	std::cout << test.toPrettyString(4) << "\n";
//	std::cout << "---------------------\n";

    // TODO: Add support for multiple attributes and :not()
    std::vector<XMLElement *> elements = test.query("/xml/server/routes/*/[@enabled='true']");
    std::cout << elements << "\n";
//    for (size_t i = 0; i < elements.size(); i++) {
//        std::cout << "Removing " << elements[i]->getName() << "\n";
//        elements[i]->remove();
//    }
//    std::cout << test.toPrettyString(4) << "\n";

//    std::string query = "/xml";
//    std::cout << "Query: " << query << "\n";
//    std::cout << "Result: \n" << test.query(query) << "\n";
//	std::cout << "---------------------\n";
//    query = "/xml/server";
//    std::cout << "Query: " << query << "\n";
//    std::cout << "Result: \n" << test.query(query) << "\n";
//	std::cout << "---------------------\n";
//    query = "/xml/server[@name='test']";
//    std::cout << "Query: " << query << "\n";
//    std::cout << "Result: \n" << test.query(query) << "\n";
//	std::cout << "---------------------\n";
//    query = "/xml/server[@name='test']/root";
//    std::cout << "Query: " << query << "\n";
//    std::cout << "Result: \n" << test.query(query) << "\n";
//	std::cout << "---------------------\n";
//    query = "/xml/server[@name='test']/index";
//    std::cout << "Query: " << query << "\n";
//    std::cout << "Result: \n" << test.query(query) << "\n";
//	std::cout << "---------------------\n";
//    query = "/xml/server[@name='test']/client_max_body_size";
//    std::cout << "Query: " << query << "\n";
//    std::cout << "Result: \n" << test.query(query) << "\n";
//	std::cout << "---------------------\n";
//    query = "/xml/server[@name='test']/error_pages";
//    std::cout << "Query: " << query << "\n";
//    std::cout << "Result: \n" << test.query(query) << "\n";
//	std::cout << "---------------------\n";
//    query = "/xml/server[@name='test']/error_pages/error_page";
//    std::cout << "Query: " << query << "\n";
//    std::cout << "Result: \n" << test.query(query) << "\n";
//	std::cout << "---------------------\n";
//    query = "/xml/server[@name='test']/error_pages/error_page[@status='404']";
//    std::cout << "Query: " << query << "\n";
//    std::cout << "Result: \n" << test.query(query) << "\n";
//	std::cout << "---------------------\n";
//    query = "/xml/server[@name='test']/error_pages/error_page[@path='path/to/50x.html'']";
//    std::cout << "Query: " << query << "\n";
//    std::cout << "Result: \n" << test.query(query) << "\n";
//    std::cout << "---------------------\n";
//    query = "/xml/server[@name='test']/error_pages/error_page[@status='500']";
//    std::cout << "Query: " << query << "\n";
//    std::cout << "Result: \n" << test.query(query) << "\n";
//    std::cout << "---------------------\n";
//    query = "/xml/server[@name='test']/error_pages/error_page[@status='501']";
//    std::cout << "Query: " << query << "\n";
//    std::cout << "Result: \n" << test.query(query) << "\n";
//    std::cout << "---------------------\n";
//    query = "/xml/server[@name='test']/error_pages/error_page[@status='502']";
//    std::cout << "Query: " << query << "\n";
//    std::cout << "Result: \n" << test.query(query) << "\n";
//    std::cout << "---------------------\n";
//    query = "/xml/server[@name='test']/error_pages/error_page[@status='502']";
//    std::cout << "Query: " << query << "\n";
//    std::cout << "Result: \n" << test.query(query) << "\n";
//    std::cout << "---------------------\n";
//    query = "/xml/server[@name='test']/error_pages/error_page:not([@status='500'])";
//    std::cout << "Query: " << query << "\n";
//    std::cout << "Result: \n" << test.query(query) << "\n";
//    std::cout << "---------------------\n";

}
