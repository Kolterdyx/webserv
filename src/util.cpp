#include "util.hpp"

/**
 * Splits a string (str) by the given character (split_char), ignoring occurences between single or double quotes.
 * @param str The string to split
 * @param split_char The char by which to split the string
 * @return An std::vector containing all of the substrings. All occurences of 'split_char' will be removed.
 */
std::vector<std::string> split(const std::string &str, char split_char) {

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
std::string trim(std::string str, std::string chars) {
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
std::string to_lower(const std::string &str) {
	std::string lower = str;
	for (std::string::iterator it = lower.begin(); it != lower.end(); ++it) {
		*it = tolower(*it);
	}
	return lower;
}

std::string datetime(const std::string &format) {
	time_t t = time(NULL);
	tm* now = localtime(&t);
	char buf[format.size() * 2];
	strftime(buf, sizeof(buf), format.c_str(), now);
	return std::string(buf);
}