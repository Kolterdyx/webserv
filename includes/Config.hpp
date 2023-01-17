#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "json.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

class Config {
private:
	json::jobject config_json;

public:
	Config();
	explicit Config(const char *config_file);
	Config(const Config &copy);
	Config &operator=(const Config &copy);
	~Config();

	// stream redirection operator
	friend std::ostream &operator<<(std::ostream &out, const Config &config);

	// Config getters
};


#endif //CONFIG_HPP
