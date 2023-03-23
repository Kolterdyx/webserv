
#include "XML/XMLDocument.hpp"
#include "XML/XMLAccessError.hpp"
#include "XML/XMLParseError.hpp"

const XMLElement * XMLDocument::getRoot() const {
    return root;
}

void XMLDocument::setRoot(XMLElement *elem) {
	delete root;
    root = new XMLElement(*elem);
}

XMLDocument::XMLDocument(const std::string &rootName) {
	root = new XMLElement(rootName);
}


std::string XMLDocument::toString() const {
    return toPrettyString(4);
}

std::ostream &operator<<(std::ostream &os, const XMLDocument &xml) {
    os << xml.toString();
    return os;
}

void XMLDocument::fromString(const std::string &xml) {
    root = XMLElement::fromString(xml);
}

std::string XMLDocument::toPrettyString(int indent) const {
    return root->toPrettyString(indent);
}

XMLDocument::~XMLDocument() {
    delete root;
}

void XMLDocument::fromFile(const std::string &filename) {
    std::ifstream file(filename.c_str());
    std::string xml;
    std::string line;
    while (std::getline(file, line)) {
        xml += line + "\n";
    }
    fromString(xml);
}


XMLElementVector XMLDocument::query(const std::string &query) const {
    XMLElementVector result;

    std::vector<std::string> queries = split(query, ';');
	std::string q;

    for (size_t i = 0; i < queries.size(); i++) {
		q = trim(queries[i], " ");
		if (q[0] != '/') {
			throw XMLParseError("Query at position " + std::to_string(i) + " does not start with a slash. (/)");
		}
        std::string selector = q.substr(1, q.size() - 1);
        XMLElementVector elements = root->query(selector);
        result.insert(result.end(), elements.begin(), elements.end());
    }
    return result;
}

XMLDocument::XMLDocument(const XMLDocument &copy) : root() {
	std::string xml = copy.toString();
	fromString(xml);
}

XMLDocument &XMLDocument::operator=(const XMLDocument &copy) {
	if (this != &copy) {
		std::string xml = copy.toString();
		fromString(xml);
	}
	return *this;
}

XMLDocument::XMLDocument() {
	root = new XMLElement("root");
}

std::string XMLDocument::toRawString() const {
	return root->toRawString();
}
