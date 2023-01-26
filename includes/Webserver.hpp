#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <iostream>
#include "XML.hpp"

class Webserver {
private:
	XML config;

public:
	Webserver();
	explicit Webserver(const XML& config);
	Webserver(const Webserver &copy);
	Webserver &operator=(const Webserver &copy);
	~Webserver();

	void run();

};


#endif //WEBSERV_HPP
