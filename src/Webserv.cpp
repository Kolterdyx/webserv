#include "Webserv.hpp"

void Webserv::run()
{
	printf("Hello World!\n");
}

Webserv::Webserv() {

}

Webserv::Webserv(const Webserv &copy) {
	*this = copy;
}

Webserv &Webserv::operator=(const Webserv &copy) {
	if (this != &copy) {
		this->config = copy.config;
	}
	return *this;
}

Webserv::~Webserv() {

}

Webserv::Webserv(const Config &config) {
	this->config = config;
}
