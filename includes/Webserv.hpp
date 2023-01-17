#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include "Config.hpp"

class Webserv {
private:
	Config config;

public:
	Webserv();
	explicit Webserv(const Config& config);
	Webserv(const Webserv &copy);
	Webserv &operator=(const Webserv &copy);
	~Webserv();

	void run();

};


#endif //WEBSERV_HPP
