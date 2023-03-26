//
// Created by Ciro Garcia belmonte on 3/24/23.
//

#ifndef WEBSERV_ROUTE_HPP
#define WEBSERV_ROUTE_HPP

#include <string>
#include <vector>
#include <map>
#include <XML/XMLElement.hpp>
#include "util.hpp"

class Route {

private:

	std::string root_path;
	std::string path;
	std::vector<std::string> methods;
	std::string index;

	std::string cgi_bin_path;
	std::string cgi_extension;
	bool cgi_enabled;

	std::string redirect_url;
	bool redirect_enabled;

	std::string autoindex_path;
	bool autoindex_enabled;

	std::string directory_listing_path;
	bool directory_listing_enabled;

	std::map<int, std::string> error_pages;

	unsigned long max_body_size;

	std::string upload_path;
	bool upload_enabled;

public:

	Route(const std::string &path);

	const std::string &getRootPath() const;

	void setRootPath(const std::string &rootPath);

	const std::string &getPath() const;

	void setPath(const std::string &path);

	const std::vector<std::string> &getMethods() const;

	void setMethods(const std::vector<std::string> &methods);

	const std::string &getIndex() const;

	void setIndex(const std::string &index);

	const std::string &getCgiBinPath() const;

	void setCgiBinPath(const std::string &cgiBinPath);

	const std::string &getCgiExtension() const;

	void setCgiExtension(const std::string &cgiExtension);

	bool isCgiEnabled() const;

	void setCgiEnabled(bool cgiEnabled);

	const std::string &getRedirectUrl() const;

	void setRedirectUrl(const std::string &redirectUrl);

	bool isRedirectEnabled() const;

	void setRedirectEnabled(bool redirectEnabled);

	const std::string &getAutoindexPath() const;

	void setAutoindexPath(const std::string &autoindexPath);

	bool isAutoindexEnabled() const;

	void setAutoindexEnabled(bool autoindexEnabled);

	const std::string &getDirectoryListingPath() const;

	void setDirectoryListingPath(const std::string &directoryListingPath);

	bool isDirectoryListingEnabled() const;

	void setDirectoryListingEnabled(bool directoryListingEnabled);

	const std::map<int, std::string> &getErrorPages() const;

	void setErrorPages(const std::map<int, std::string> &errorPages);

	unsigned long getMaxBodySize() const;

	void setMaxBodySize(unsigned long maxBodySize);

	const std::string &getUploadPath() const;

	void setUploadPath(const std::string &uploadPath);

	bool isUploadEnabled() const;

	void setUploadEnabled(bool uploadEnabled);

	void addMethod(const std::string &method);

	void removeMethod(const std::string &method);

	void setErrorPage(int code, const std::string &filepath);

	std::string getErrorPage(int code);

	void removeErrorPage(int code);

	void setRawErrorPage(int code, const std::string &content);

	explicit Route(XMLElement *config);

	Route();

	Route(XMLElement *config, Logger &logger);
};


#endif //WEBSERV_ROUTE_HPP
