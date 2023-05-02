//
// Created by Ciro Garcia belmonte on 3/24/23.
//

#ifndef WEBSERV_REQUEST_HPP
#define WEBSERV_REQUEST_HPP

#include <map>
#include <string>
#include <ostream>
#include <sstream>
#include <iostream>
#include <sys/socket.h>
#include "util.hpp"

class Request {

private:

	std::map<std::string, std::string> headers;
	std::string body;
	std::string method;
	std::string path;
	std::string version;

	std::string origin_ip;
public:
	const std::string &getOriginIp() const;

private:

	void parse_header(const std::string &header_string);

public:

	Request();
	Request(const std::string& raw_request, struct sockaddr_in addr);

	~Request();

	void setHeader(const std::string &key, const std::string &value);

	void setBody(const std::string &body);

	void setMethod(const std::string &method);

	std::string getHeader(const std::string &key) const;

	std::string getBody() const;

	std::string getMethod() const;

	friend std::ostream &operator<<(std::ostream &os, const Request &request);


	int getBodySize() const;

	std::string toString() const;

	void parse_http_request(std::string request);

	std::string getPath() const;

};


#endif //WEBSERV_REQUEST_HPP
