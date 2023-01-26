#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <iostream>
#include "Config.hpp"

class Webserver {
private:
	Config config;

public:
	Webserver();
	explicit Webserver(const Config& config);
	Webserver(const Webserver &copy);
	Webserver &operator=(const Webserver &copy);
	~Webserver();

	void run();

};


#endif //WEBSERV_HPP
