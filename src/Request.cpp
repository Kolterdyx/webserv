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

std::string Request::toString() {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

std::ostream &operator<<(std::ostream &os, const Request &request) {
	os << request.method << " / HTTP/1.1\n";
	for (std::map<std::string, std::string>::const_iterator it = request.headers.begin();
		 it != request.headers.end(); ++it) {
		os << it->first << ": " << it->second << "\n";
	}
	os << "\n";

	if (request.body.size() > 0) {
		os << request.body;
	}
	return os;
}

void Request::parse_http_request(std::string request) {
	std::string header = request.substr(0, request.find("\r\n\r\n"));
	std::string body = request.substr(request.find("\r\n\r\n") + 4);
	setBody(body);
	parse_header(header);
}


std::string Request::getPath() {
	return path;
}
