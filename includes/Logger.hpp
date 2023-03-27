//
// Created by Ciro Garcia belmonte on 3/23/23.
//

#ifndef WEBSERV_LOGGER_HPP
#define WEBSERV_LOGGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include "macros.hpp"

class Logger {

private:
	std::string _name;
	std::string _path;
	std::ofstream _file;

	std::string _format;

	std::map<std::string, std::string> _patterns;
	static std::map<std::string, std::string> _text_format;

	static std::map<int, std::string> _levelnames_color;
	static std::map<int, std::string> _levelnames;


	int _level;

	bool useStdout;
	bool useFile;

	void print(const std::string &message, int level);

	std::string applyFormat(const std::string &message, bool color, int level);

public:

	Logger();
	explicit Logger(const std::string &name);
	Logger(const Logger &copy);
	Logger &operator=(const Logger &copy);
	~Logger();

	void setPath(const std::string &path);
	void setUseFile(bool useFile);
	void setUseStdout(bool useStdout);

	void setLevel(int level);

	void setFormat(const std::string &format);

	void debug(const std::string &message);
	void log(const std::string &message, int level);
	void info(const std::string &message);
	void warn(const std::string &message);
	void warning(std::string message);
	void error(const std::string &message);
	void critical(const std::string &message);

	void init();

	static const int DEBUG;
	static const int INFO;
	static const int WARN;
	static const int ERROR;
	static const int CRITICAL;

	unsigned int getLevel();
};

extern Logger logger;
#endif //WEBSERV_LOGGER_HPP
