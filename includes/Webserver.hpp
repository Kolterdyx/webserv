#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <iostream>
#include "XML/XMLDocument.hpp"

class Webserver {
private:
	XMLDocument config;

public:
	Webserver();
	explicit Webserver(const XMLDocument& config);
	Webserver(const Webserver &copy);
	Webserver &operator=(const Webserver &copy);
	~Webserver();

	void run();

};


#endif //WEBSERV_HPP
