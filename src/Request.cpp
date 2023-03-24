//
// Created by Ciro Garcia belmonte on 3/24/23.
//

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
		headers["Content-Length"] = std::to_string(body.size());
	}
}

void Request::setMethod(const std::string &method) {
	this->method = method;
}

std::string Request::getHeader(const std::string &key) {
	// Case insensitive
	for (std::map<std::string, std::string>::iterator it = headers.begin();
		 it != headers.end(); ++it) {
		if (to_lower(it->first) == to_lower(key)) {
			return it->second;
		}
	}
	return "";
}

std::string Request::getBody() {
	return body;
}

std::string Request::getMethod() {
	return method;
}

Request::Request(const std::string &raw_request) {
	parse_http_request(raw_request);
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
			method = trim(line.substr(0, line.find(" ")), " ");
			path = trim(line.substr(line.find(" ") + 1, line.find("HTTP") - line.find(" ") - 1), " ");
			version = trim(line.substr(line.find("HTTP") + 5), " ");
			continue;
		}
		key = line.substr(0, line.find(":"));
		value = trim(line.substr(line.find(":") + 1), " ");
		headers[key] = value;
	}
}


int Request::getBodySize() {

	if (getHeader("Content-Length").empty()) {
		return 0;
	}
	return std::stoi(getHeader("Content-Length"));
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


std::string Request::getPath() {
	return path;
}
