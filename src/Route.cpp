
#include <fstream>
#include <Logger.hpp>
#include "Route.hpp"

const std::string &Route::getRootPath() const {
	return root_path;
}

void Route::setRootPath(const std::string &rootPath) {
	root_path = rootPath;
}

const std::string &Route::getPath() const {
	return path;
}

void Route::setPath(const std::string &path) {
	Route::path = path;
}

const std::vector<std::string> &Route::getMethods() const {
	return methods;
}

void Route::setMethods(const std::vector<std::string> &methods) {
	Route::methods = methods;
}

const std::string &Route::getIndex() const {
	return index;
}

void Route::setIndex(const std::string &index) {
	Route::index = index;
}

const std::string &Route::getCgiBinPath() const {
	return cgi_bin_path;
}

void Route::setCgiBinPath(const std::string &cgiBinPath) {
	cgi_bin_path = cgiBinPath;
}

const std::string &Route::getCgiExtension() const {
	return cgi_extension;
}

void Route::setCgiExtension(const std::string &cgiExtension) {
	cgi_extension = cgiExtension;
}

bool Route::isCgiEnabled() const {
	return cgi_enabled;
}

void Route::setCgiEnabled(bool cgiEnabled) {
	cgi_enabled = cgiEnabled;
}

const std::string &Route::getRedirectUrl() const {
	return redirect_url;
}

void Route::setRedirectUrl(const std::string &redirectUrl) {
	redirect_url = redirectUrl;
}

bool Route::isRedirectEnabled() const {
	return redirect_enabled;
}

void Route::setRedirectEnabled(bool redirectEnabled) {
	redirect_enabled = redirectEnabled;
}

const std::string &Route::getAutoindexPath() const {
	return autoindex_path;
}

void Route::setAutoindexPath(const std::string &autoindexPath) {
	autoindex_path = autoindexPath;
}

bool Route::isAutoindexEnabled() const {
	return autoindex_enabled;
}

void Route::setAutoindexEnabled(bool autoindexEnabled) {
	autoindex_enabled = autoindexEnabled;
}

const std::string &Route::getDirectoryListingPath() const {
	return directory_listing_path;
}

void Route::setDirectoryListingPath(const std::string &directoryListingPath) {
	directory_listing_path = directoryListingPath;
}

bool Route::isDirectoryListingEnabled() const {
	return directory_listing_enabled;
}

void Route::setDirectoryListingEnabled(bool directoryListingEnabled) {
	directory_listing_enabled = directoryListingEnabled;
}

const std::map<int, std::string> &Route::getErrorPages() const {
	return error_pages;
}

void Route::setErrorPages(const std::map<int, std::string> &errorPages) {
	error_pages = errorPages;
}

unsigned long Route::getMaxBodySize() const {
	return max_body_size;
}

void Route::setMaxBodySize(unsigned long maxBodySize) {
	max_body_size = maxBodySize;
}

const std::string &Route::getUploadPath() const {
	return upload_path;
}

void Route::setUploadPath(const std::string &uploadPath) {
	upload_path = uploadPath;
}

bool Route::isUploadEnabled() const {
	return upload_enabled;
}

void Route::setUploadEnabled(bool uploadEnabled) {
	upload_enabled = uploadEnabled;
}

Route::Route(const std::string &path) : path(path) {}

void Route::addMethod(const std::string &method) {
	methods.push_back(method);
}

void Route::setRawErrorPage(int code, const std::string &content) {
	error_pages[code] = content;
}

void Route::setErrorPage(int code, const std::string &filepath) {
	std::ifstream file(filepath.c_str());
	if (!file.good()) {
		throw std::runtime_error("Error page file not found");
	}
	std::string content((std::istreambuf_iterator<char>(file)),
						std::istreambuf_iterator<char>());
	error_pages[code] = content;
}

void Route::removeMethod(const std::string &method) {
    for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); it++) {
        if (*it == method) {
            methods.erase(it);
            return;
        }
    }
}

void Route::removeErrorPage(int code) {
	error_pages.erase(code);
}

std::string Route::getErrorPage(int code) {
	return error_pages[code];
}

Route::Route(XMLElement *config, Logger &logger) : cgi_enabled(), redirect_enabled(), autoindex_enabled(), directory_listing_enabled(), max_body_size(), upload_enabled() {

	XMLElementVector rootPaths = config->query("root");
	XMLElementVector errorPages = config->query("error_page");
	XMLElementVector indexFiles = config->query("index");
	if (rootPaths.empty()) {
		logger.warn("No root directive found. Ignoring route.");
		return;
	}
	else if (rootPaths.size() > 1) {
		logger.warn("Multiple root directives found. Ignoring route.");
		return;
	}

	// Load error pages for the whole server
	for (XMLElementVector::iterator it = errorPages.begin(); it != errorPages.end(); it++) {
		if (!(*it)->hasAttribute("code") || !(*it)->hasAttribute("location")) {
			logger.warn("Invalid error_page directive. Ignoring error_page.");
			continue;
		}

		int status;

		if (!(*it)->hasAttribute("status")) {
			logger.warn("Invalid error_page directive. Ignoring error_page.");
			continue;
		}

		try {
			status = util::stoi((*it)->getAttribute("status"));
		} catch (std::invalid_argument &e) {
			logger.warn("Invalid error code. Ignoring error_page.");
			continue;
		}

		if (!(*it)->hasAttribute("path")) {
			logger.warn("Invalid error_page directive. Ignoring error_page.");
			continue;
		}
		std::string path = (*it)->getAttribute("path");
		setErrorPage(status, path);
	}

	// Load index files
	std::string index;
	if (indexFiles.empty()) {
		logger.warn("No index directive found. Using default (index.html).");
		index = "index.html";
	}
	else if (indexFiles.size() > 1) {
		logger.warn("Multiple index directives found. Using first one.");
		index = indexFiles[0]->getContent();
	}

	// Load cgi stuff
	XMLElementVector cgiConfigs = config->query("cgi");
	if (cgiConfigs.size() == 1) {
		XMLElement *cgiConfig = cgiConfigs[0];
		setCgiBinPath(cgiConfig->getAttribute("path"));
		setCgiExtension(cgiConfig->getAttribute("ext"));
		bool cgiEnabled;
		if (cgiConfig->getAttribute("enabled") == "true") {
			cgiEnabled = true;
		} else if (!cgiConfig->hasAttribute("enabled") || cgiConfig->getAttribute("enabled") == "false") {
			cgiEnabled = false;
		} else {
			logger.warn("Invalid cgi directive. Ignoring cgi.");
			cgiEnabled = false;
		}
		setCgiEnabled(cgiEnabled);
	}

	// Directory listing
	XMLElementVector dirListingConfigs = config->query("directory_listing");
	bool dirListingEnabled;
	std::string dirListingIndex;
	if (dirListingConfigs.size() == 1) {
		XMLElement *dirListingConfig = dirListingConfigs[0];
		if (dirListingConfig->getAttribute("enabled") == "true") {
			dirListingEnabled = true;
		} else if (!dirListingConfig->hasAttribute("enabled") || dirListingConfig->getAttribute("enabled") == "false") {
			dirListingEnabled = false;
		} else {
			logger.warn("Invalid directory_listing directive. Ignoring directory_listing.");
			dirListingEnabled = false;
		}
		if (dirListingConfig->hasAttribute("index")) {
			dirListingIndex = dirListingConfig->getContent();
		} else {
			dirListingIndex = "index.html";
		}
	} else {
		dirListingEnabled = false;
		dirListingIndex = "index.html";
	}
	setDirectoryListingEnabled(dirListingEnabled);
	setDirectoryListingPath(dirListingIndex);

	setPath(root_path);
	setIndex(index);

}

Route::Route()
		: cgi_enabled(), redirect_enabled(), autoindex_enabled(),
		  directory_listing_enabled(), max_body_size(), upload_enabled() {

}
