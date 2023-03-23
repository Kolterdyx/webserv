
#include "XML/XMLDocument.hpp"
#include "XML/XMLAccessError.hpp"
#include "XML/XMLParseError.hpp"

const XMLElement * XMLDocument::getRoot() const {
    return root->getChildren()[0];
}

void XMLDocument::setRoot(XMLElement *elem) {
	this->root->clearChildren();
    this->root->addChild(elem);
}

XMLDocument::XMLDocument(const std::string &rootName) {
	root = new XMLElement("__root__");
    setRoot(new XMLElement(rootName));
}


std::string XMLDocument::toString() const {
    return toPrettyString(4);
}

std::ostream &operator<<(std::ostream &os, const XMLDocument &xml) {
    os << xml.toString();
    return os;
}

void XMLDocument::fromString(const std::string &xml) {
    setRoot(XMLElement::fromString(xml));
}

std::string XMLDocument::toPrettyString(int indent) const {
    return root->getChildren()[0]->toPrettyString(indent);
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

    std::vector<std::string> queries = custom::split(query, ';');
	std::string q;

    for (size_t i = 0; i < queries.size(); i++) {
		q = custom::trim(queries[i], " ");
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
	*this = copy;
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
