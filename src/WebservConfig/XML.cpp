
#include "XML.hpp"

XML::XML() : id(0), has_content(false) {}

XML::XML(const std::string &xml_string, size_t id) : id(id) {
	has_content = false;
	parse_xml(xml_string);
}

XML::XML(const std::string &xml_string) : id(0) {
	has_content = false;
	parse_xml(xml_string);
}

XML::XML(const XML &xml, size_t id) : id(id) {
	*this = xml;
}

XML::XML(const XML &xml) : id(xml.id) {
	*this = xml;
}

XML XML::root(const std::string &tag) {
	XML root;
	root.tag = tag;
	return root;
}

XML::~XML() {
	for (std::vector<std::string>::const_iterator it = children.begin(); it != children.end(); it++) {
		delete children[*it];
	}
}

XML &XML::operator=(const XML &xml) {
	if (this != &xml) {
		attributes = xml.attributes;
		children = xml.children;
		has_content = xml.has_content;
		tag = xml.tag;
	}
	return *this;
}

void XML::parse_xml(const std::string &xml_content) {
	std::string xml_str = get_first_xml_object(xml_content);
	std::string xml_tag = get_first_tag(xml_str);
	tag = get_tag_name(xml_tag);
	attributes = get_attributes(xml_tag);
	std::string xml_content_str = get_xml_content(xml_str);
	attributes.insert("__content__", xml_content_str);
	if (!xml_content_str.empty()) {
		try {
			save_children(xml_content_str);
		} catch (std::runtime_error &e) {
			has_content = true;
		}
	}
}

void XML::save_children(const std::string& xml_content) {
	std::vector<std::string> objects;
	size_t len = 0;
	std::string obj = "1";
	while (len < xml_content.length() && !obj.empty()) {
		obj = get_first_xml_object(xml_content, len);
		len += obj.length();
		if (!obj.empty())
			objects.push_back(obj);
	}

	for (size_t i = 0; i < objects.size(); i++) {
		obj = objects[i];
		std::string xml_tag = get_first_tag(obj);
		std::string tag_name = get_tag_name(xml_tag);
		XML *child = new XML(trim(obj, " \n\t"), children.size());
		children.insert(tag_name + std::to_string(children.size()), child);
	}


}

char XML::stri(std::string str, size_t i)
{
	if (i < 0 || i >= str.length())
		return 0;
	return str[i];
}

std::string XML::clean_tag(const std::string &_tag)
{
	std::string tag_clean1 = _tag;
	while (is_whitespace(stri(tag_clean1, 0)))
		tag_clean1 = tag_clean1.substr(1);
	while (is_whitespace(stri(tag_clean1, tag_clean1.length() - 1)))
		tag_clean1 = tag_clean1.substr(0, tag_clean1.length() - 1);

	std::string tag_clean2;
	bool in_quotes = false;
	for (size_t i = 0; i < tag_clean1.length(); i++)
	{
		if (stri(tag_clean1, i) == '"')
			in_quotes = !in_quotes;
		if (is_whitespace(stri(tag_clean1, i)) && !in_quotes)
		{
			if (!is_whitespace(stri(tag_clean1, i - 1)) && stri(tag_clean1, i - 1) != '=' && stri(tag_clean1, i + 1) != '=' && stri(tag_clean1, i + 1) != '>')
				tag_clean2 += ' ';
		}
		else
			tag_clean2 += stri(tag_clean1, i);
	}

	return tag_clean2;
}

std::string XML::trim(std::string str, std::string chars)
{
	// Remove all characters in chars from the ends of str

	size_t start = 0;
	size_t end = str.length() - 1;
	while (start < str.length() && chars.find(str[start]) != std::string::npos)
		start++;
	while (end > 0 && chars.find(str[end]) != std::string::npos)
		end--;
	return str.substr(start, end - start + 1);
}

std::string XML::get_first_tag(const std::string &xml_str)
{
	size_t i = 0;
	if (trim(xml_str, "\n \t").empty()) {
		throw std::runtime_error("Empty xml string.");
	}
	while (stri(xml_str, i) && stri(xml_str, i) != '<')
		i++;
	if (stri(xml_str, i + 1) == '/')
		throw std::runtime_error("First tag is a closing tag.");
	std::string _tag;
	while (stri(xml_str, i) && stri(xml_str, i) != '>')
	{
		_tag += stri(xml_str, i);
		i++;
	}
	_tag += '>';
	std::string tag_clean = clean_tag(clean_tag(_tag));
	return tag_clean;
}

std::string XML::get_tag_name(const std::string &xml_tag)
{
	size_t i = 1;
	std::string tag_name;
	if (xml_tag.empty()) {
		throw std::runtime_error("Empty xml tag.");
	}
	while (!is_whitespace(stri(xml_tag, i)) && stri(xml_tag, i) && stri(xml_tag, i) != '>')
	{
		tag_name += stri(xml_tag, i);
		i++;
	}

	return tag_name;
}

ordered_map<std::string, std::string> XML::get_attributes(const std::string &xml_str)
{
	ordered_map<std::string, std::string> attrs;

	if (xml_str.empty()) {
		return attrs;
	}

	std::string allowed_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
	size_t i = 1;

	while (!is_whitespace(stri(xml_str, i)) && stri(xml_str, i) != '>')
		i++;

	while (stri(xml_str, i) && stri(xml_str, i) != '>')
	{
		std::string attribute_name;
		std::string attribute_value;
		while (stri(xml_str, i) && is_whitespace(stri(xml_str, i)))
			i++;
		if (stri(xml_str, i) == '>' || (stri(xml_str, i) == '/' && stri(xml_str, i + 1) == '>'))
			break;
		while (stri(xml_str, i) && stri(xml_str, i) != '=')
		{
			attribute_name += stri(xml_str, i);
			i++;
		}
		if (stri(xml_str, i) != '=')
			throw std::invalid_argument("Attribute name not followed by '='.");
		i++;
		while (stri(xml_str, i) && stri(xml_str, i) != '"')
			i++;
		if (stri(xml_str, i) != '"')
			throw std::invalid_argument("Attribute value not enclosed in '\"'.");
		i++;
		while (stri(xml_str, i) && stri(xml_str, i) != '"')
		{
			attribute_value += stri(xml_str, i);
			i++;
		}
		if (stri(xml_str, i) != '"')
			throw std::invalid_argument("Attribute value not enclosed in '\"'.");
		i++;
		if (attribute_name.empty() || attribute_value.empty())
			throw std::invalid_argument("Attribute name or value is empty.");
		else if (attrs.contains(attribute_name))
			throw std::invalid_argument("Attribute name is not unique.");
		else if (attribute_name == "content")
			throw std::invalid_argument("Attribute name \"content\" is reserved.");
		for (size_t j = 0; j < attribute_name.length(); j++)
			if (allowed_chars.find(attribute_name[j]) == std::string::npos)
				throw std::invalid_argument("Attribute name contains invalid characters.");
		attrs.insert(attribute_name, attribute_value);
	}

	return attrs;
}

std::string XML::get_xml_content(const std::string &xml_string) {
	if (trim(xml_string, " \n\t").empty()) {
		return "";
	}

	std::string content;
	std::string tag = XML::get_first_tag(xml_string);
	std::string tag_name = XML::get_tag_name(tag);

	std::string end_tag = "</" + tag_name + ">";
	size_t end_tag_pos = xml_string.find(end_tag);
	if (end_tag_pos == std::string::npos) {
		return "";
	}
	size_t start_tag_pos = xml_string.find(tag);
	if (start_tag_pos == std::string::npos) {
		return "";
	}
	size_t content_start = start_tag_pos + tag.length();
	size_t content_end = end_tag_pos;
	content = xml_string.substr(content_start, content_end - content_start);

	return content;
}

std::string XML::get_first_xml_object(const std::string &xml_string, size_t pos)
{
	if (pos <= xml_string.length()) {
		std::string new_xml_string = xml_string.substr(pos);
		return get_first_xml_object_raw(new_xml_string);
	}
	return "";
}

std::string XML::get_first_xml_object_raw(const std::string &xml_string)
{
	if (trim(xml_string, "\n \t").empty()) {
		return "";
	}

	// TODO: Fix parsing to allow self-closing tags to be at the end of an xml
	//  string. Currently, this will make the parser think that all the children
	//  alongside the self-closing tag and the self-closing tag itself are all
	//  just raw text instead of parseable xml objects.

	std::string content;
	std::string tag = XML::get_first_tag(xml_string);
	std::string tag_name = XML::get_tag_name(tag);
	if (tag_name.empty())
		throw std::runtime_error("No opening tag was found or was empty. (tag: " + tag + ", tag_name: " + tag_name +")\nString:\n"+xml_string);

	// Check if self-closing
	if (stri(tag, tag.length() - 2) == '/') {
		return tag;
	}

	std::string end_tag = "</" + tag_name + ">";
	size_t end_tag_pos = xml_string.find(end_tag);
	if (end_tag_pos == std::string::npos) {
		throw std::runtime_error("No closing tag (" + end_tag + ") was found.");
	}
	size_t start_tag_pos = xml_string.find(tag);
	if (start_tag_pos == std::string::npos) {
		throw std::runtime_error("No opening tag was found.");
	}
	size_t content_end = end_tag_pos + end_tag.length();
	content = xml_string.substr(0, content_end);

	return content;
}

std::string XML::get_first_xml_object(const std::string &xml_string)
{
	if (xml_string.empty()) {
		return "";
	}

	std::string content;
	std::string tag = XML::get_first_tag(xml_string);
	std::string tag_name = XML::get_tag_name(tag);

	// Check if self-closing
	if (tag.substr(tag.length() - 2) == "/>") {
		return tag;
	}

	std::string end_tag = "</" + tag_name + ">";
	size_t end_tag_pos = xml_string.find(end_tag);
	if (end_tag_pos == std::string::npos) {
		return "";
	}
	size_t start_tag_pos = xml_string.find(tag);
	if (start_tag_pos == std::string::npos) {
		return "";
	}
	size_t content_start = start_tag_pos;
	size_t content_end = end_tag_pos + end_tag.length();
	content = xml_string.substr(content_start, content_end - content_start);

	return content;
}

bool XML::is_whitespace(char c) {
	return c == ' ' || c == '\n' || c == '\t';
}

const std::string & XML::get_name() const {
	return tag;
}

std::string XML::get_attribute(const std::string &attribute_name) const {
	return attributes[attribute_name];
}

std::vector<std::string> XML::get_attribute_names() const {
	std::vector<std::string> attribute_names;
	for (std::vector<std::string>::const_iterator it = attributes.begin(); it != attributes.end(); it++) {
		attribute_names.push_back(*it);
	}
	return attribute_names;
}

std::string XML::get_value() const {
	if (has_content) {
		return attributes["__content__"];
	} else {
		return "";
	}
}

std::vector<XML*> XML::_get_children(const std::string &child_name) const {
	std::vector<XML*> children_;
	for (std::vector<std::string>::const_iterator it = children.begin(); it != children.end(); it++) {
		std::string name = children[*it]->tag;
		if (name == child_name) {
			children_.push_back(children[*it]);
		}
	}
	return children_;
}

std::vector<XML*> XML::get_children(const std::string &child_name) const {
	std::vector<XML*> children_;
	std::string _tag = child_name.substr(0, child_name.find_last_of('.'));
	XML *child = get_child(_tag);
	for (std::vector<std::string>::const_iterator it = child->children.begin(); it != child->children.end(); it++) {
		std::string name = child->children[*it]->tag;
		if (name == child_name.substr(child_name.find_last_of('.')+ 1)) {
			children_.push_back(child->children[*it]);
		}
	}
	return children_;
}

std::string XML::to_string(int indent) const {
	return to_string_n(indent, 0);
}

std::string XML::to_string_n(int indent, int level) const {
	std::string xml_string;
	std::string indent_string;
	if (indent < 0)
		throw std::invalid_argument("Indent must be a greater than or equal to 0");
	if (indent > 0) {
		for (int i = 0; i < indent * level; i++) {
			indent_string += " ";
		}
	}
	xml_string += indent_string + "<" + tag;
	for (std::vector<std::string>::const_iterator it = attributes.begin(); it != attributes.end(); it++) {
		if (*it != "__content__") {
			xml_string += " " + *it + "=\"" + attributes[*it] + "\"";
		}
	}
	if (children.empty() && !has_content) {
		xml_string += "/>";
		if (indent > 0) {
			xml_string += "\n";
		}
	} else {
		xml_string += ">";
		if (!children.empty()) {
			if (indent > 0) {
				xml_string += "\n";
			}
			for (std::vector<std::string>::const_iterator it = children.begin(); it != children.end(); it++) {
				xml_string += children[*it]->to_string_n(indent, level + 1);
			}
			xml_string += indent_string + "</" + tag + ">";
		} else if (has_content) {
			xml_string += attributes["__content__"] + "</" + tag + ">";
		}
		if (indent > 0) {
			xml_string += "\n";
		}
	}
	return xml_string;
}

std::ostream &operator<<(std::ostream &os, const XML &xml) {
	os << xml.to_string(DEFAULT_XML_INDENT);
	return os;
}

XML *XML::get_child(const std::string &child_name) const {
	std::string _tag = child_name.substr(0, child_name.find('.'));
	std::string _child_name = child_name.substr(child_name.find('.') + 1);
	if (!_has_child(_tag)) {
		throw std::invalid_argument("\"" + _tag + "\" has no child with name \"" + _child_name + "\"");
	}
	std::vector<XML*> _children = _get_children(_tag);
	for (std::vector<XML*>::iterator it = _children.begin(); it != _children.end(); it++) {
		try {
			if ((*it)->tag == _tag) {
				return child_name.find('.') == std::string::npos ? *it : (*it)->get_child(child_name.substr(child_name.find('.') + 1));
			}
		} catch (std::invalid_argument &e) {
			continue;
		}
	}
	throw std::invalid_argument("\"" + _tag + "\" has no child with name \"" + _child_name + "\"");
}

bool XML::has_child(const std::string &child_name) const {
	try {
		get_child(child_name);
		return true;
	} catch (std::invalid_argument &e) {
		return false;
	}
}

bool XML::_has_child(const std::string &child_name) const {
	return !_get_children(child_name).empty();
}

XML *XML::add_child(const std::string &child_name) {
	size_t new_id = children.size();
	XML *child = new XML("<" + child_name + "/>", new_id);
	children.insert(child_name + "." + std::to_string(new_id), child);
	return child;
}

XML *XML::add_child(XML *child) {
	size_t new_id = children.size();
	XML *new_child = new XML(*child, new_id);
	children.insert(child->tag + "." + std::to_string(new_id), new_child);
	return new_child;
}

void XML::remove_children(const std::string &child_tag) {
	for (std::vector<std::string>::const_iterator it = children.begin(); it != children.end(); it++) {
		if (children[*it]->tag == child_tag) {
			children.erase(*it);
			return;
		}
	}
}

void XML::remove_child(const std::string &child_tag, const size_t child_id) {
	children.erase(child_tag + "." + std::to_string(child_id));
}

void XML::add_attribute(const std::string &attribute_name, const std::string &attribute_value) {
	attributes.insert(attribute_name, attribute_value);
}

const std::string &XML::get_tag() const {
	return tag;
}

size_t XML::get_id() const {
	return id;
}

void XML::set_tag(const std::string &tag) {
	this->tag = tag;
}
