#include <sys/stat.h>
#include "util.hpp"

int util::stoi(const std::string &s) throw (std::invalid_argument) {
    int i;
    try {
        std::istringstream(s) >> i;
    } catch (std::exception &e) {
        throw std::invalid_argument("stoi: Invalid argument");
    }
    return i;
}

/**
 * Splits a string (str) by the given character (split_char), ignoring occurences between single or double quotes.
 * @param str The string to split
 * @param split_char The char by which to split the string
 * @return An std::vector containing all of the substrings. All occurences of 'split_char' will be removed.
 */
std::vector<std::string> util::split(const std::string &str, char split_char) {

    std::vector<std::string> result;
    std::string current;
    bool in_quotes = false;
    char quote_char = 0;
    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];
        if (c == '\'' || c == '"') {
            if (in_quotes) {
                if (c == quote_char) {
                    in_quotes = false;
                }
                current += c;
            } else {
                in_quotes = true;
                quote_char = c;
                current += c;
            }
        } else if (c == split_char) {
            if (in_quotes) {
                current += c;
            } else {
                result.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty())
        result.push_back(current);
    return result;
}

/**
 * @brief Trim the given set of characters off of both ends of a string.
 * @param str String to trim
 * @param chars Characters to trim off from both ends
 * @return Trimmed string
 */
std::string util::trim(std::string str, std::string chars) {
    // This function removes all the characters in the string "chars" from the beginning and end of the string "str".

    size_t start = str.find_first_not_of(chars);
    if (start == std::string::npos)
        return "";
    size_t end = str.find_last_not_of(chars);
    return str.substr(start, end - start + 1);
}

/**
 * @brief Converts a string to lowercase
 * @param str String to convert
 * @return Lowercase string
 */
std::string util::to_lower(const std::string &str) {
    std::string lower = str;
    for (std::string::iterator it = lower.begin(); it != lower.end(); ++it) {
        *it = tolower(*it);
    }
    return lower;
}

std::string util::datetime(const std::string &format) {
    time_t t = time(NULL);
    tm *now = localtime(&t);
    char buf[format.size() * 2];
    strftime(buf, sizeof(buf), format.c_str(), now);
    return std::string(buf);
}

std::string util::combine_path(const std::string &path1, const std::string &path2, bool simplify) {
    std::string path = path1;
    if (path1[path1.size() - 1] != '/' && path2[0] != '/')
        path += '/';
    else if (path1[path1.size() - 1] == '/' && path2[0] == '/')
        path = path.substr(0, path.size() - 1);
    path += path2;

    // Remove double slashes and double dots
    if (simplify) {
        std::vector<std::string> parts = split(path, '/');
        std::vector<std::string> new_parts;
        for (std::vector<std::string>::iterator it = parts.begin();
             it != parts.end(); ++it) {
            if (*it == "..") {
                if (!new_parts.empty())
                    new_parts.pop_back();
            } else if (*it != "." && !(*it).empty()) {
                new_parts.push_back(*it);
            }
        }
        path.clear();
        int i = 0;
        for (std::vector<std::string>::iterator it = new_parts.begin();
             it != new_parts.end(); ++it) {
            if (i++ > 0 || path1[0] == '/')
                path += "/" + *it;
            else
                path += *it;
        }
    }

    return path;
}


bool util::is_dir(const std::string &path) {

	struct stat st = {};
	if (path_exists(path)) {
		return S_ISDIR(st.st_mode);
	}
	return false;
}

bool util::path_exists(const std::string& path) {
	struct stat st = {};
	return stat(path.c_str(), &st) == 0;
}
