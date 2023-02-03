#ifndef CUSTOM_SPECIFICATIONS_HPP
#define CUSTOM_SPECIFICATIONS_HPP

#include <vector>
#include <map>
#include <iostream>
#include <sstream>

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

	/**
	 * @brief Converts a given variable to a string using an std::stringstream
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

namespace custom {

	/**
	 * Splits a string (str) by the given character (split_char), ignoring occurences between single or double quotes.
	 * @param str The string to split
	 * @param split_char The char by which to split the string
	 * @return An std::vector containing all of the substrings. All occurences of 'split_char' will be removed.
	 */
	std::vector<std::string> split(const std::string &str, char split_char) {
    // This function splits the specified string into a vector of strings, using the specified character as a separator.
    // For example, split("foo.bar", '.') will return a vector containing "foo" and "bar".
    // The separator character will be ignored if it is between double or single quotes.

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

}

#endif //CUSTOM_SPECIFICATIONS_HPP
