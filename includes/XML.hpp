
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

	void save_children(const std::string& xml_content);
	std::string to_string_n(int indent, int level) const;

	XML(const std::string &xml_string, size_t id);
	XML(const XML &xml, size_t id);

	void parse_xml(const std::string &xml_string);
	std::vector<XML *> _get_children(const std::string &child_name) const;
	bool _has_child(const std::string &child_name) const;

	static char stri(std::string str, size_t i);
	static std::string clean_tag(const std::string &_tag);
	static std::string get_first_tag(const std::string &xml_str);
	static std::string get_tag_name(const std::string &xml_tag);
	static ordered_map<std::string, std::string> get_attributes(const std::string &xml_str);
	static std::string get_xml_content(const std::string &xml_string);
	static std::string get_first_xml_object(const std::string &xml_string);
	static std::string get_first_xml_object(const std::string &xml_string, size_t pos);
	static std::string get_first_xml_object_raw(const std::string &xml_string);

	static std::string trim(std::string str, std::string chars);


public:
	XML();
	explicit XML(const std::string &xml_string);
	XML(const XML &xml);
	XML &operator=(const XML &xml);
	~XML();

	static XML root(const std::string &tag);

	friend std::ostream &operator<<(std::ostream &os, const XML &xml);

	std::string to_string(int indent) const;
	const std::string &get_tag() const;
	size_t get_id() const;
	void set_tag(const std::string &tag);
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
};


#endif //WEBSERVCONFIG_XML_HPP
