#include "Webserver.hpp"

void Webserver::run()
{
	printf("Hello World!\n");
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

Webserver::Webserver(const XML &config) {
	this->config = config;
}
