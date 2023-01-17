#include "Config.hpp"
#include "../includes/Config.hpp"

Config::Config() {

}

Config::Config(const char *config_file) {
	std::ifstream file(config_file);
	if (!file.is_open()) {
		std::cerr << "Error: Could not open config file" << std::endl;
		exit(1);
	}
	std::string config_str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	this->config_json = json::jobject::parse(config_str);
	this->init();
}



Config::Config(const Config &copy) {
	*this = copy;

}

Config &Config::operator=(const Config &copy) {
	if (this != &copy) {
		this->config_json = copy.config_json;
		init();
	}
	return *this;
}

Config::~Config() {

}

void Config::init() {
}

std::ostream &operator<<(std::ostream &out, const Config &config) {
	out << "Config {\n";
	out << "}\n";
	return out;
}
