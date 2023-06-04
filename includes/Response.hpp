//
// Created by Ciro Garcia belmonte on 3/24/23.
//

#ifndef WEBSERV_RESPONSE_HPP
#define WEBSERV_RESPONSE_HPP

#include "macros.hpp"
#include <map>
#include <string>
#include <iostream>
#include "util.hpp"
#include "Logger.hpp"

class Response {

private:

	std::map<std::string, std::string> headers;
	std::string body;
	int status;
	std::string version;

	static std::map<int, std::string> status_names;

	void parse_header(const std::string &header_string);

public:

	Response();
	explicit Response(int status);
	~Response();

	void setHeader(const std::string &key, const std::string &value);

	void setBody(const std::string &body);

	void setStatus(int status);

	std::string getHeader(const std::string &key) const;

	std::string getBody();

	int getStatus() const;

	friend std::ostream &operator<<(std::ostream &os, const Response &response);

	int getBodySize();

	std::string toString();

	void init();

	std::string getStatusString() const;

	void addHeader(const std::string& key, const std::string& value);
};


#endif //WEBSERV_RESPONSE_HPP
