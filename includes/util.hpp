#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>

// For executeCgi
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "Request.hpp"
class Request;
#define CGI_BUFSIZE 20

namespace std {


    template<typename T>
    std::ostream &operator<<(std::ostream &os, const std::vector<T> vector) {
		int i = 0;
        for (typename std::vector<T>::const_iterator it = vector.begin(); it != vector.end(); it++) {
            os << i++ << ": " << *it << "\n";
        }
        return os;
    }

    template<typename K, typename V>
    std::ostream &operator<<(std::ostream &os, const std::map<K, V> map) {
        for (typename std::map<K, V>::const_iterator it = map.begin(); it != map.end(); it++) {
            os << it->first << ": " << it->second << "\n";
        }
        return os;
    }
}

namespace util {

    /**
     * @brief Converts a string to an integer. Here because stoi is not available in C++98
     * @param str String to convert
     * @return Integer
     */
    int stoi(const std::string &str) throw(std::invalid_argument);

    /**
     * @brief Splits a string into a vector of strings
     * @param str String to split
     * @param split_char Character to split on
     * @return Vector of strings
     */
    std::vector<std::string> split(const std::string &str, char split_char);

    /**
     * @brief Removes all characters in chars from the beginning and end of str
     * @param str String to trim
     * @param chars Characters to remove
     * @return Trimmed string
     */
    std::string trim(std::string str, std::string chars);

    /**
     * @brief Converts a string to lowercase
     * @param str String to convert
     * @return Lowercase string
     */
    std::string to_lower(const std::string &str);

    /**
     * @brief Returns the current date and/or time in the format specified by the format parameter. See 'man strftime' for more info.
     * @param format Format of the date and time. See strftime for more info. Default is "%Y%m%dT%H%M%SZ". Not all formats are required.
     * @return Current date and time
     */
    std::string datetime(const std::string &format = "%Y%m%dT%H%M%SZ");

    /**
     * @brief Combines two paths.
     * @param path1 First path
     * @param path2 Second path
     * @param simplify If true, it will remove double slashes and interpret ".."
     * @return Combined path (path1 + path2)
     */
    std::string combine_path(const std::string &path1, const std::string &path2, bool simplify = true);

    /**
	 * @brief Converts a given variable to a string using an std::stringstream. Added here because we can't have C++11
	 * @tparam T
	 * @param var variable to convert. If it's a class instance, it must have a << operator overload.
	 * @return
	 */
	template<typename T>
	std::string to_string(T var) {
		std::stringstream ss;
		ss << var;
		return ss.str();
	}

	/**
	 * @brief Returns whether the given path is a file or a directory. Should be cross-platform.
	 * @param path The path to check.
	 * @return true if the path is a directory, false otherwise.
	 */
	bool is_dir(const std::string &path);

	bool path_exists(const std::string& path);

    /**
	 * @brief Returns 'str' in decimal or 0.
	 * @param str a hex number.
	 * @return the number in decimal or 0.
    */
    int hex_str_to_dec(const std::string &str);

    // TODO: maybe should be in Request.hpp or in an new class
    std::string executeCgi(const Request &request, const std::string &cgiBinPath, std::string file_content);

}

#endif //UTILS_HPP
