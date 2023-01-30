#include "Webserver.hpp"

void Webserver::run()
{
    std::cout << "Running webserver" << std::endl;
}

Webserver::Webserver() {

}

Webserver::Webserver(const Webserver &copy) {
	*this = copy;
}

Webserver &Webserver::operator=(const Webserver &copy) {
	if (this != &copy) {
		this->config = copy.config;
	}
	return *this;
}

Webserver::~Webserver() {

}

Webserver::Webserver(const XMLDocument &config) {
	this->config = config;
}
