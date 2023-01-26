
#include "XML.hpp"

XML::XML() : id(0), has_content(false) {}

XML::XML(const std::string &xml_string, size_t id) : id(id) {
	has_content = false;
	try {
		parse_xml(xml_string);
	} catch (std::out_of_range &e) {
		std::cerr << e.what() << std::endl;
	}
}

XML::XML(const std::string &xml_string) : id(0) {
	has_content = false;
	try {
		parse_xml(xml_string);
	} catch (std::out_of_range &e) {
		std::cerr << e.what() << std::endl;
	}
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
	int i = 0;
	// There is only one root tag, so we can assume that the first tag is the root tag
	while (i < xml_content.length() && is_whitespace(xml_content[i])) i++;
	printf("i: %d\n", i);
	if (xml_content[i] != '<') {
		throw std::invalid_argument("XML string must start with '<'");
	}
	i++;
	while (i < xml_content.length() && is_whitespace(xml_content[i])) i++;
	printf("i: %d\n", i);
	std::string name;
	while (i < xml_content.length() && !is_whitespace(xml_content[i]) && (xml_content[i] != '>' || xml_content.substr(i, 2) != "/>")) {
		name += xml_content[i];
		i++;
	}
	printf("i: %d\n", i);
	this->tag = name;
	while (i < xml_content.length() && is_whitespace(xml_content[i])) i++;
	printf("ii: %d\n", i);
	// Parse attributes
	while (i < xml_content.length() && (xml_content[i] != '>' || xml_content.substr(i, 2) != "/>")) {
		std::string attribute_name;
		printf("i: %d\n", i);
		while (i < xml_content.length() && !is_whitespace(xml_content[i]) && xml_content[i] != '=') {
			attribute_name += xml_content[i];
			i++;
		}
		printf("i: %d\n", i);
		while (i < xml_content.length() && is_whitespace(xml_content[i])) i++;
		if (xml_content[i] != '=') {
			throw std::invalid_argument("XML attribute must have a value");
		}
		i++;
		while (i < xml_content.length() && is_whitespace(xml_content[i])) i++;
		if (xml_content[i] != '"') {
			throw std::invalid_argument("XML attribute value must be enclosed in double quotes");
		}
		i++;
		std::string attribute_value;
		while (i < xml_content.length() && xml_content[i] != '"') {
			attribute_value += xml_content[i];
			i++;
		}
		i++;
		attributes.insert(attribute_name, attribute_value);
		while (i < xml_content.length() && is_whitespace(xml_content[i])) i++;
	}
	printf("i: %d\n", i);
	// See if this is a self-closing tag
	if (xml_content[i] == '/') {
		i++;
		if (xml_content[i] != '>') {
			throw std::invalid_argument("XML tag must end with '>'");
		}
		return;
	} else {
		// If we get here, this is not a self-closing tag, so it must have either children or text content
		// First, subtract the content until the end tag
		if (i < xml_content.length() && xml_content[i] != '>') {
			throw std::invalid_argument("XML tag must end with '>'");
		}
		i++;
		std::string content;
		std::string end_tag = "</" + name + ">";
		while (i < xml_content.length() && xml_content.substr(i, end_tag.size()) != end_tag && i < xml_content.length()) {
			content += xml_content[i];
			i++;
		}
		if (i == xml_content.length()) {
			throw std::invalid_argument("XML tag must have an end tag");
		}
		std::cout << "Content: " << content << std::endl;
		// Now, see if the content is a valid XML string
		try {
			save_children(content);
		} catch (std::invalid_argument &e) {
			// If it's not a valid XML string, it's just text content
			this->has_content = true;
			attributes.insert("content", content);
		}
	}
}

void XML::save_children(std::string xml_content) {
	// Separate the content into children. Must be compliant with C++98.
	// Each child is a valid XML string. Each child may have children of its own, which we need to ignore.
	// A child is delimited by its start tag and its end tag. The end tag is the same as the start tag, but with a '/' before the tag.
	// This is how a child looks like: <child [attributes[="value"]>...</child>.
	// We can find a child by looking for its start tag, and then finding the end tag that matches it.

	// First, find the start tag
	std::vector<std::string> children_strings;
	size_t i = 0;

//	std::cout << "size: " << xml_content.size() << std::endl;

	while (i < xml_content.length()) {
		std::string child_string;
		std::string child_content;
		std::string child_attributes;
		std::string child_name;
		bool self_closing = false;
		while (is_whitespace(xml_content[i])) i++;
		if (i == xml_content.length()) break;
		if (xml_content[i] == '<') {
			// We found the start of the start tag. Now, find the end of the start tag
			i++;
			while (is_whitespace(xml_content[i])) i++;
			while (!is_whitespace(xml_content[i]) && xml_content[i] != '>') {
				child_name += xml_content[i];
				i++;
			}
			// We also need to save the attributes. We can do it in a string.
			while (xml_content[i] != '>') {
				if (xml_content[i] == '/' && xml_content[i+1] == '>') {
					// This is a self-closing tag. We can save the child and move on
					child_string = "<" + child_name + child_attributes + "/>";
					children_strings.push_back(child_string);
					i += 2;
					self_closing = true;
					break;
				}
				child_attributes += xml_content[i];
				i++;
			}
		}
		if (self_closing) {
			continue;
		}
		if (child_name.empty() && i != xml_content.length()) {
			// We didn't find a start tag. This means that the content is not valid XML
			throw std::invalid_argument("XML content is not valid XML");
		}
		// Now, find the end tag. While we're at it, save the child's content.
		std::string end_tag = "</" + child_name + ">";
		i++;
		while (xml_content.substr(i, end_tag.size()) != end_tag) {
			child_content += xml_content[i];
			i++;
		}
		// We found the end tag. Now, save the child
		std::string child_xml_string = "<" + child_name + child_attributes + ">" + child_content + end_tag;
		children_strings.push_back(child_xml_string);

		// skip to the end of the end tag
		i += end_tag.size();

		// If we're at the end of the content, we're done
		if (i == xml_content.length()) break;
	}
	for (std::vector<std::string>::iterator it = children_strings.begin(); it != children_strings.end(); it++) {
		XML child(*it, -1);
		add_child(&child);
	}
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
		return attributes["content"];
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
		if (*it != "content") {
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
			xml_string += attributes["content"] + "</" + tag + ">";
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
	if (!_has_child(_tag)) {
		throw std::invalid_argument("No child with name \"" + _tag + "\"");
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
	throw std::invalid_argument("No child with name \"" + _tag + "\"");
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
