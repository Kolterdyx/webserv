//
// Created by Ciro Garcia belmonte on 3/24/23.
//

#include "Response.hpp"

Response::Response() {
	status = 200;
	init();

}

Response::Response(int status) {
	this->status = status;
	init();
}

std::map<int, std::string> Response::status_names = std::map<int, std::string>();

void Response::init() {
	version = "HTTP/1.1";
	status_names[200] = "OK";
	status_names[201] = "Created";
	status_names[202] = "Accepted";
	status_names[204] = "No Content";
	status_names[301] = "Moved Permanently";
	status_names[302] = "Found";
	status_names[304] = "Not Modified";
	status_names[400] = "Bad Request";
	status_names[401] = "Unauthorized";
	status_names[403] = "Forbidden";
	status_names[404] = "Not Found";
	status_names[405] = "Method Not Allowed";
	status_names[406] = "Not Acceptable";
	status_names[408] = "Request Timeout";
	status_names[409] = "Conflict";
	status_names[410] = "Gone";
	status_names[411] = "Length Required";
	status_names[412] = "Precondition Failed";
	status_names[413] = "Payload Too Large";
	status_names[414] = "URI Too Long";
	status_names[415] = "Unsupported Media Type";
	status_names[416] = "Range Not Satisfiable";
	status_names[417] = "Expectation Failed";
	status_names[418] = "I'm a teapot";
	status_names[421] = "Misdirected Request";
	status_names[422] = "Unprocessable Entity";
	status_names[423] = "Locked";
	status_names[424] = "Failed Dependency";
	status_names[426] = "Upgrade Required";
	status_names[428] = "Precondition Required";
	status_names[429] = "Too Many Requests";
	status_names[431] = "Request Header Fields Too Large";
	status_names[451] = "Unavailable For Legal Reasons";
	status_names[500] = "Internal Server Error";
	status_names[501] = "Not Implemented";
	status_names[502] = "Bad Gateway";
	status_names[503] = "Service Unavailable";
	status_names[504] = "Gateway Timeout";
	status_names[505] = "HTTP Version Not Supported";
	status_names[506] = "Variant Also Negotiates";
	status_names[507] = "Insufficient Storage";
	status_names[508] = "Loop Detected";
	status_names[510] = "Not Extended";
	status_names[511] = "Network Authentication Required";
}

Response::~Response() {

}

void Response::setHeader(const std::string &key, const std::string &value) {
	headers[key] = value;
}

void Response::setBody(const std::string &body) {
	this->body = body;
	headers["Content-Length"] = std::to_string(body.size());
}

void Response::setStatus(int status) {
	this->status = status;
}

std::string Response::getHeader(const std::string &key) const {
	// Case insensitive
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		if (to_lower(it->first) == to_lower(key)) {
			return it->second;
		}
	}
	return "";
}

std::string Response::toString() {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

int Response::getBodySize() {
	return body.size();
}

std::ostream &operator<<(std::ostream &os, const Response &response) {
	os << response.version << " " << response.getStatusString() << " " << response.getHeader("Content-Type") << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = response.headers.begin();
		 it != response.headers.end(); ++it) {
		os << it->first << ": " << it->second << std::endl;
	}
	os << "\n";
	os << response.body;
	return os;
}

std::string Response::getBody() {
	return body;
}

int Response::getStatus() const {
	return status;
}

std::string Response::getStatusString() const {
	return std::to_string(status) + " " + status_names[status];
}

void Response::addHeader(const std::string& key, const std::string& value) {
	setHeader(key, value);
}
