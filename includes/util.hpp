#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <map>
#include <iostream>
#include <sstream>

namespace std {

    int stoi(const std::string &str);

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
}

std::vector<std::string> split(const std::string &str, char split_char);
std::string trim(std::string str, std::string chars);
std::string to_lower(const std::string &str);
std::string datetime(const std::string &format = "%Y%m%dT%H%M%SZ");
std::string combine_path(const std::string &path1, const std::string &path2, bool sanitize = true);

#endif //UTILS_HPP
