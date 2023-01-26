
#ifndef WEBSERVCONFIG_XML_HPP
#define WEBSERVCONFIG_XML_HPP

#include <iostream>
#include "ordered_map.hpp"
#include <vector>
#include <fstream>

#define DEFAULT_XML_INDENT 4

class XML {

private:
	ordered_map<std::string, std::string> attributes;
	ordered_map<std::string, XML*> children;
	std::string tag;
	const size_t id;
	bool has_content;

	static bool is_whitespace(char c);

	void save_children(std::string xml_content);
	std::string to_string_n(int indent, int level) const;

	XML(const std::string &xml_string, size_t id);
	XML(const XML &xml, size_t id);

	void parse_xml(const std::string &xml_string);
	std::vector<XML *> _get_children(const std::string &child_name) const;
	bool _has_child(const std::string &child_name) const;


public:
	XML();
	explicit XML(const std::string &xml_string);
	XML(const XML &xml);
	XML &operator=(const XML &xml);
	~XML();

	static XML root(const std::string &tag);

	const std::string & get_name() const;
	std::string get_attribute(const std::string &attribute_name) const;
	std::vector<std::string> get_attribute_names() const;

	std::string get_value() const;
	std::vector<XML *> get_children(const std::string &child_name) const;
	XML *get_child(const std::string &child_name) const;
	bool has_child(const std::string &child_name) const;
	XML *add_child(const std::string &child_name);
	XML *add_child(XML *child);
	void remove_children(const std::string &child_tag);
	void remove_child(const std::string &child_tag, size_t child_id);
	void add_attribute(const std::string &attribute_name, const std::string &attribute_value);

	std::string to_string(int indent) const;

	friend std::ostream &operator<<(std::ostream &os, const XML &xml);



};


#endif //WEBSERVCONFIG_XML_HPP
