//
// Created by Ciro Garcia belmonte on 3/24/23.
//

#include <netinet/in.h>
#include <arpa/inet.h>
#include "Request.hpp"

Request::Request() {

}

Request::~Request() {

}

void Request::setHeader(const std::string &key, const std::string &value) {
	headers[key] = value;
}

void Request::setBody(const std::string &body) {
	this->body = body;
	if (headers.find("Content-Length") != headers.end()) {
		headers["Content-Length"] = util::to_string(body.size());
	}
}

void Request::setMethod(const std::string &method) {
	this->method = method;
}

std::string Request::getHeader(const std::string &key) const {
	// Case insensitive
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
		 it != headers.end(); ++it) {
		if (util::to_lower(it->first) == util::to_lower(key)) {
			return it->second;
		}
	}
	return "";
}

std::string Request::getBody() const {
	return body;
}

std::string Request::getMethod() const {
	return method;
}

Request::Request(const std::string& raw_request, sockaddr addr) {
	parse_http_request(raw_request);
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(((sockaddr_in *)&addr)->sin_addr), ip, INET_ADDRSTRLEN);
	origin_ip = std::string(ip);
}

void Request::parse_header(const std::string &header_string) {

	std::stringstream ss(header_string);
	std::string line;
	std::string key;
	std::string value;
	bool first_line = true;
	while(std::getline(ss, line, '\n')) {
		if (line.empty()) {
			break;
		}
		if (first_line) {
			first_line = false;
			method = util::trim(line.substr(0, line.find(" ")), " ");
			path = util::trim(line.substr(line.find(" ") + 1, line.find("HTTP") - line.find(" ") - 1), " ");
			version = util::trim(line.substr(line.find("HTTP") + 5), " ");
			continue;
		}
		key = line.substr(0, line.find(":"));
		value = util::trim(line.substr(line.find(":") + 1), " ");
		headers[key] = value;
	}
}


int Request::getBodySize() const {

	if (!getHeader("Content-Length").empty()) {
		return util::stoi(getHeader("Content-Length"));
	}
	if (!getHeader("Transfer-Encoding").empty()) {
		if (getBody().size() == 0)
			return 0;
		size_t count = getBody().find("\r\n");
		int size = util::hex_str_to_dec(getBody().substr(0, count));
		return size;
	}
	return 0;
}

std::string Request::toString() const {
	std::string s;

	// HTTP format

	s += method + " " + path + " HTTP/" + version + "\n";
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		s += it->first + ": " + it->second + "\n";
	}
	s += "\n";
	s += body;

	return s;
}

std::ostream &operator<<(std::ostream &os, const Request &request) {
	std::string s = request.toString();
	// replace all \r\n with \n
	for (std::string::iterator it = s.begin(); it != s.end(); ++it) {
		if (*it == '\r') {
			*it = ' ';
		}
	}
	os << s;
	return os;
}

void Request::parse_http_request(std::string request) {
	std::string header = request.substr(0, request.find("\r\n\r\n"));
	if (request.find("\r\n\r\n") + 4 >= request.size()) {
		setBody("");
	} else {
		std::string body = request.substr(request.find("\r\n\r\n") + 4);
		setBody(body);
	}
	parse_header(header);
}


std::string Request::getPath() const {
	return path;
}

const std::string &Request::getOriginIp() const {
	return origin_ip;
}


